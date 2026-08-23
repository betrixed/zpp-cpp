#ifndef WCC_RUN_CPP
#define WCC_RUN_CPP

#ifndef WCC_RUN_H
#include "run.h"
#endif

#ifndef WCC_FINDER_H
#include "wcc/finder.h"
#endif

#ifndef REFLECT_CACHE_H
#include "wcc/reflect_cache.h"
#endif

#ifndef WCC_CACHEMGR_H
#include "wcc/cachemgr.h"
#endif

#ifndef WCC_DOS_H
#include "dos.h"
#endif

#ifndef WCC_SERVICES_H
#include "wcc/services.h"
#endif

#ifndef WCC_CONFIG_H
#include "wcc/config.h"
#endif

#ifndef WCC_LOADER_H
#include "wcc/loader.h"
#endif

//#define DBG_LOG_RUN

#ifdef DBG_LOG_RUN
#	ifndef WCC_DEBUGLOG_H
#		include "wcc/debuglog.h"
#	endif
#endif

#ifndef REQUEST_GLOBALS_H
#include "wcc/request_globals.h"
#endif


#ifndef RUN_ARGINFO_H
#define RUN_ARGINFO_H
extern "C" {
#include "stub/run_arginfo.h"
};
#endif

#ifndef PHP_SESSION_H
extern "C" {
	#include <ext/session/php_session.h>
}
#endif

namespace wcc {

using namespace zpp;

base_obj_mgr<Run> Run::omg;

Run_init Run_i;



void Run_init::init()
{
	start_time = "start_time";
	target = "target";
	target_const = "TARGET";


	site_leaf = "site_leaf";
	bootstrap = "bootstrap";

	site_leaf_const = "SITE_LEAF";
	site_const = "SITE";
	
	wc_leaf = "wc_leaf";
	vendor_leaf = "vendor_leaf";
	vendor_str = "vendor";
	phproot = "phproot";

	config_dir = "config_dir";
	temp_dir = "temp_dir";
	init_cwd = "init_cwd";
	dos_str = "dos";

	finder_class = "Wcc\\Finder";

	page_hits = "page_hits";
	user_name = "user_name";
	view_data = "view_data";

	finder = "finder";

	s_services = "services";


	run_str = "run";
	phpstats_class = "Wcc\\PhpStats";
	phpstats_str = "phpstats";
	default_str = "default";

	sapi_str = "sapi";
	cli_str = "cli";
	begin_str = "begin";
	stats_str = "stats";
	is_web = "is_web";

	php_str = "php";
	config_str = "config";
	web_str = "web";
	assets_str = "assets";


	web_dir = "web_dir";
	theme_str = "theme";
	br_eol = "\n";
	br_html = "<br>\n";
	br_eol_str = "BR_EOL";

	site_dir = "site_dir";
	gallery_str = "gallery";

	ns_sep = "\\";
	dir_sep = "/";
	addPathArray = "addpatharray";
	cache_mgr = "cache_mgr";
	file_cache = "file_cache";

	namespaces_str = "namespaces";
	modules_str = "modules";
	error_log = "error_log";
	temp_folder_names = "temp_folder_names";
	temp_folder_paths = "temp_folder_paths";

	app_class = "app_class";
	s_prepare = "prepare";
	site_str = "site";
	cryptic_str = "cryptic";

	user_session = "user_session";
	is_ended = "isended";
}


void Run::debug_info(htab_rw di)
{
	base_d::debug_info(di);
}

void Run::construct()
{

	obj_ptr self = this->self();

	val_rc  temp = datetime_obj::microtime();
	val_rc  stime = temp;

	self.property(Run_i.start_time, temp);

	str_rc sapi = php_sapi_name();
	self.property(Run_i.sapi_str, sapi);

	bool is_web = (zs_cmp(sapi, Run_i.cli_str) != 0);

	if (!defined(Run_i.br_eol_str))
	{


		if (is_web)
		{
			temp = Run_i.br_html;
		}
		else {
			temp = Run_i.br_eol;
		}
		define(Run_i.br_eol_str, temp);
	}

	val_rc nullval;

	self.property(Run_i.cache_mgr, nullval);
	//showobj("Loader", loader);

	//return;
	temp.set_bool(is_web);
	self.property(Run_i.is_web, temp);

	Loader* lob = Loader::cpp_global();
	str_rc php_root = lob->getBaseDir();
	self.property(Run_i.phproot, php_root);

	str_rc root_dir = dirname(php_root);
	self.property(Run_i.wc_leaf, root_dir);


	Services* sobj = Services::cpp_global();

	obj_return config_err = sobj->newInstance(Config::omg.class_name());
	
	if (config_err.throw_errors())
	{
		return;
	}
	obj_rc config = std::move(config_err.value_);
	sobj->set(Run_i.config_str, config);

	self.property(Run_i.config_str, config);

	str_rc site_leaf = constant(Run_i.site_leaf_const);
	self.property(Run_i.site_leaf, site_leaf);

	str_buf buf;
	buf << site_leaf << "/config";
	str_rc config_dir(buf.zstr());

	config.property(Run_i.config_dir, config_dir);
	self.property(Run_i.config_dir, config_dir);

	buf << site_leaf << "/tmp";
	str_rc temp_dir(buf.zstr());



	config.property(Run_i.temp_dir, temp_dir);
	self.property(Run_i.temp_dir, temp_dir);

	#ifdef DBG_LOG_RUN
	DebugLog* log = DebugLog::cpp_global();

	if (!log)
	{
		buf << temp_dir << "/log/debuglog.txt";
		obj_rc debug = DebugLog::omg.new_zobj();
		DebugLog* log = zobj_toc<DebugLog>(debug);
		log->construct(buf.zstr(), DebugLog::TO_FILE);
		DebugLog::setInstance(debug);
		log->line("<pre>Run start");
	}
	#endif

	

	temp = getcwd();
	self.property(Run_i.init_cwd, getcwd());

	temp.set_bool(false);
	self.property(Run_i.page_hits,temp);

	sobj->set(Run_i.run_str, self);
	sobj->setObject(config);

	temp = constant(Run_i.target_const);
	//showmem("target", temp);
	self.property(Run_i.target, temp);

	self.property(Run_i.vendor_leaf, Run_i.vendor_str);

	

	temp = constant(Run_i.site_const);
	self.property(Run_i.site_dir, temp);

	str_rc site_str = temp.zstr();


	buf << '/' << site_str << "/gallery";

	self.property(Run_i.gallery_str, buf.zstr());
	buf << site_str << "/site"; // gallery site theme sub-folder

	self.property(Run_i.theme_str, buf.zstr());

	
	htab_rc start_args;
	htab_rw args(start_args);

	obj_rc stats = ReflectCache::staticInstance(Run_i.phpstats_class);

	sobj->set(Run_i.phpstats_str, stats);

	temp = stats;
	self.property(Run_i.stats_str, temp);



	obj_rc dos = Dos::omg.new_zobj();
	Dos* dptr = zobj_toc<Dos>(dos);
	dptr->construct(str_ptr::empty_str());

	sobj->set(Run_i.dos_str, dos);


}


obj_ptr 
Run::setup_world()
{
	obj_ptr self(self_);
#ifdef DBG_LOG_RUN
	DebugLog* log = DebugLog::cpp_global();
#endif
	val_return ctest = Services::service(Run_i.config_str);

	obj_rc config;

	if (ctest.has_errors())
	{
		ctest.error() << "No config object service";
		return self;
	}

	config = ctest.value_.zobject();

	str_rc app_class = config.str_property(Run_i.app_class);
#ifdef DBG_LOG_RUN
	if (log)
	{
		log->dump("app_class", app_class);
	}
#endif

	obj_rc site = ReflectCache::staticInstance(app_class);

	self.property(Run_i.site_str, site);
	site.call(Run_i.s_prepare);

	return site;
}

void Run::destruct()
{

}


error_return
Run::execute(str_ptr bootstrap)
{
	//zend_printf("In execute\n");
	error_return result;

	result = this->config_init(bootstrap);


	if (result.has_errors())
	{

		return result;
	}

	this->temp_folders();

	result = this->setup_cryptic();
	if (result.has_errors())
	{
		return result;
	}
	obj_ptr site = this->setup_world();
	site.call(Run_i.run_str);
	return result;
	
}

static void transfer_str(str_ptr pname, htab_ptr from, obj_ptr to)
{
	val_ptr test = from.get(pname);
	if (test.ok())
	{
		to.property(pname, test);
	}
	else {
		zend_printf("value not found for %s\n", pname.data());
	}
}

error_return 
Run::setup_cryptic()
{
	error_return result;

	obj_ptr self (this->self());

	obj_ptr config = self.obj_property(Run_i.config_str);

	str_rc  config_dir = self.str_property(Run_i.config_dir);

	str_rc cryptic_data = config.str_property(Run_i.cryptic_str);

	if (!cryptic_data.ok())
	{
		return result;
	}
	str_buf buf;

	buf << config_dir << '/' << cryptic_data;
	str_rc path = buf.zstr();
	
	#ifdef DBG_LOG_RUN
	DebugLog* log = DebugLog::cpp_global();
	#endif

	Services* sobj = Services::cpp_global();

	val_return data;
	if (file_exists(path))
	{	
		 
		CacheMgr *cmgr = CacheMgr::instance(result);
		if (result.has_errors() || !cmgr)
		{
			return result;
		}
		data = cmgr->readCache(path, Run_i.file_cache);

		if (data.has_errors())
		{
			result = data.move_error();
		}
		else {
			sobj->set(Run_i.cryptic_str, data.value_);
		}
	}
	else {
		result.error() << "File " << path << " not found";
	}
	#ifdef DBG_LOG_RUN
	if (log)
		{
			log->line("End setup_cryptic");
		}
	#endif
	return result;
	

}

void Run::temp_folders()
{
	#ifdef DBG_LOG_RUN
	DebugLog* log = DebugLog::cpp_global();
	#endif

	/*if (log)
	{
		log->line("temp_folders");
	}
	*/
	obj_ptr self(self_);

	obj_rc config = self.obj_property(Run_i.config_str);
	
	/*if (log)
	{
		log->dump("config", config);
	}*/

	str_rc temp_dir = self.str_property(Run_i.temp_dir);

	//showstr("temp folder", temp_dir);

	str_buf buf;

	buf << temp_dir << "/log/errors.txt";

	config.property(Run_i.error_log, buf.zstr());

	htab_rc folder_names = config.array_property(Run_i.temp_folder_names);
	/*
	if (log)
	{
		log->dump("folder_names", folder_names);
	}
	*/

	//showdata("folder_names", folder_names);

	htab_rc missing_rc;
	htab_rc temp_path_rc;

	htab_rw missing(missing_rc);
	htab_rw temp_path(temp_path_rc);

	htab_walk wk;
	auto name = wk.value();
	auto vkey = wk.key();

	for(wk.start(folder_names); wk.ok(); wk.next())
	{
		buf << temp_dir << '/' << name.zstr();
		str_ptr path = buf.zstr();

		str_ptr  key = vkey.zstr();

		//showstr("temp folder", path);

		temp_path.set(key, path);
		if (!is_dir(path)) 
		{
			if (! mkdir(path, 0755, true))
			{
				missing.set(key, path);
			}
		}
	}

	val_rc temp(temp_path_rc);
	config.property(Run_i.temp_folder_paths, temp);
	if (missing_rc.size())
	{
		htab_walk w2;

		auto key2 = w2.key();
		auto path2 = w2.value();

		for(w2.start(missing_rc); w2.ok(); w2.next())
		{
			str_ptr key = key2.zstr();
			str_ptr path = path2.zstr();

			buf << key << " => " << path << endl;
		}
		str_rc msg = buf.zstr();

		zend_throw_error(zend_ce_error, "Missing folders: %s", msg.data());
		
	}
	#ifdef DBG_LOG_RUN
	if (log)
	{
		log->line("End temp_folders");
	}
	#endif

}

void Run::shutdown()
{

	Services* sobj = Services::cpp_global();

	if (php_get_session_status() == php_session_active)
	{
		obj_rc user_session;

		val_return utest = sobj->get(Run_i.user_session);
		if (utest.throw_errors())
		{
			return;
		}
		user_session = utest.value_.zobject();

		if (user_session.ok())
		{
			val_rc ended = user_session.call(Run_i.is_ended);
			if (ended.isFalse())
			{
				htab_ptr session = htab_rc::get_global(RQit.G_SESSION);
				if (session.size())
				{
					//showdata("session end", session);
					php_session_flush(1);
				}
			}
		}
	}
	
	error_return erred;

	CacheMgr *cmgr = CacheMgr::instance(erred);
	if (erred.throw_errors() || !cmgr)
	{
		return;
	}

	cmgr->flush_caches();
	//zend_printf("Caches written\n");

	val_return ctest = sobj->get(Run_i.config_str);

	if (ctest.throw_errors())
	{
		return;
	}

	Config* cobj = zobj_toc<Config>(ctest.value_.zobject());
	cobj->clear();

	sobj->clearDefer();
	sobj->clearActive();
	sobj->clearObjects();

	ReflectCache* rc = ReflectCache::cpp();

	rc->clear();
}

error_return
Run::config_init(str_ptr bootstrap)
{
	error_return result;

	obj_ptr self = this->self();
	str_rc config_dir = self.property(Run_i.config_dir);

	str_buf buf;
	buf << config_dir << "/" << bootstrap;

	str_rc path = buf.zstr();
	val_return value;
	val_rc tlist;

	if (file_exists(path))
	{
		Loader* lob = Loader::cpp_global();

		value = lob->require(path);

		if (value.has_errors())
		{
			result = value.move_error();
			return result;
		}
	}
	else {
		result.error() << "No bootstrap file " << bootstrap;
		return result;
	}

	htab_rc bcfg = self.array_property(Run_i.bootstrap);
	#ifdef DBG_LOG_RUN
	DebugLog* log = DebugLog::cpp_global();
	#endif

	if (bcfg.size())
	{


		str_rc target = self.str_property(Run_i.target);

		target.lowercase();


		tlist = bcfg.get(target);
		if (!tlist.ok())
		{
			tlist = bcfg.get(Run_i.default_str);
		}

	}
	else {
		result.error() <<  "bootstrap property not set" << path;
		return result;
	}
		
	bcfg = tlist.zarray();
	//showdata("bcfg", bcfg);

	val_ptr test = bcfg.get(Run_i.assets_str);

	if (test.isArray())
	{
		htab_rc assets = test.zarray();
		transfer_str(Run_i.web_dir, assets, self);
		transfer_str(Run_i.theme_str, assets, self);
	}
	
	
	tlist = self.property(Run_i.is_web);

	if (tlist.isFalse()) {
		tlist = bcfg.get(Run_i.cli_str);
	}
	else {
		tlist = bcfg.get(Run_i.web_str);
	}

	obj_rc config = self.property(Run_i.config_str);
	//showobj("config", config);

	if (tlist.isArray())
	{
		bcfg = tlist.zarray();
		tlist = bcfg.get(Run_i.php_str);

		if (tlist.isArray())
		{
			CacheMgr*  cmgr = CacheMgr::instance(result);

			if (result.has_errors())
			{
				return result;
			}

			Config*    cfg = zobj_toc<Config>(config);

			htab_walk wk;

			auto cfg_path = wk.value();

			for(wk.start(tlist.zarray()); wk.ok(); wk.next())
			{
				buf << config_dir << "/" << cfg_path.zstr();
				path = buf.zstr();


				if (file_exists(path))
				{
					val_return data = cmgr->readCache(path, Run_i.file_cache);
					if (data.has_errors())
					{
						result = data.move_error();
						return result;
					}

					val_ptr vp(data.value_);

					if (vp.isArray())
					{
						htab_rc tfer = vp.zarray();

						cfg->addArray(tfer);
					}
					else {
						result.error() << "Array data expected";
						return result;
					}
				}
				else {
					result.error() << "File not found: " << path;
					return result;
				}
			}

			val_rc ns = config.property(Run_i.namespaces_str);

			if (ns.isArray())
			{
				 obj_rc finder;

				 val_return ftest = Services::service(Run_i.finder);

				 if (ftest.has_errors())
				 {
				 	result = ftest.move_error();
				 	return result;
				 }
				 finder = ftest.value_.zobject();


				 Finder* fd = zobj_toc<Finder>(finder);
				 fd->addPathArray(ns.zarray());
			}
			else {
				zend_printf("No %s entry in config!\n", Run_i.namespaces_str.data());

			}
			str_buf mpath;

			mpath << config_dir << "/modules";

			str_ptr modules_dir = mpath.zstr();


			if (is_dir(modules_dir))
			{
				htab_rc flist = Finder::dirList_dir(modules_dir);
				val_rc  flistarg(flist);
				self.property(Run_i.modules_str, flistarg);
			}

		}
		
	}
	#ifdef DBG_LOG_RUN
	if (log)
	{
		log->line("End bootstrap");
	}
	#endif

	return result;
}

}; // namespace wcc

using namespace wcc;
using namespace zpp;

ZEND_METHOD(Wcc_Run, __construct)
{
	ZEND_PARSE_PARAMETERS_NONE();
	Run* cobj = zval_toc<Run>(ZEND_THIS);
	cobj->construct();
}

ZEND_METHOD(Wcc_Run, __destruct)
{
	ZEND_PARSE_PARAMETERS_NONE();
	Run* cobj = zval_toc<Run>(ZEND_THIS);
	cobj->destruct();
}

ZEND_METHOD(Wcc_Run, execute)
{
	zarg_rd args(execute_data);
	str_ptr bootstrap;

	args.zstring(bootstrap, args.need(0));

	if (!args.throw_errors(__FUNCTION__))
	{
		Run* cobj = zval_toc<Run>(ZEND_THIS);
		error_return result = cobj->execute(bootstrap);
		result.throw_errors(__FUNCTION__);
	}
}

ZEND_METHOD(Wcc_Run, shutdown)
{
	ZEND_PARSE_PARAMETERS_NONE();
	Run* cobj = zval_toc<Run>(ZEND_THIS);
	cobj->shutdown();
}

PHP_MINIT_FUNCTION(wcc_run_reg)
{
	Run::omg.classEntry(register_class_Wcc_Run());

	STATE_INIT_ADD(Run_i)
	return SUCCESS;
}

#endif