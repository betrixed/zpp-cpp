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

#ifndef WCC_SERVICES_H
#include "wcc/services.h"
#endif

#ifndef WCC_CONFIG_H
#include "wcc/config.h"
#endif

#ifndef WCC_LOADER_H
#include "wcc/loader.h"
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
	site_leaf_const = "SITE_LEAF";
	site_const = "SITE";
	
	wc_leaf = "wc_leaf";
	vendor_leaf = "vendor_leaf";
	vendor_str = "vendor";
	phproot = "phproot";

	config_dir = "config_dir";
	temp_dir = "temp_dir";
	init_cwd = "init_cwd";
	dos_class = "Wcc\\Dos";
	finder_class = "Wcc\\Finder";

	dos_str = "dos";

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

	//showobj("Loader", loader);

	//return;
	temp.set_bool(is_web);
	self.property(Run_i.is_web, temp);

	Loader* lob = Loader::cpp_global();
	str_rc php_root = lob->getBaseDir();
	self.property(Run_i.phproot, php_root);

	str_rc root_dir = dirname(php_root);
	self.property(Run_i.wc_leaf, root_dir);

	temp = get_constant(Run_i.target_const);
	//showmem("target", temp);

	self.property(Run_i.target, temp);

	self.property(Run_i.vendor_leaf, Run_i.vendor_str);

	str_rc site_leaf = get_constant(Run_i.site_leaf_const);
	self.property(Run_i.site_leaf, site_leaf);

	str_rc tstr = get_constant(Run_i.site_const);
	self.property(Run_i.site_dir, tstr);


	str_buf buf;

	buf << '/' << tstr << "/gallery";
	tstr = buf.zstr();

	self.property(Run_i.gallery_str, tstr);
	buf << tstr << "/site";

	self.property(Run_i.theme_str, buf.zstr());

	buf << site_leaf << "/config";
	self.property(Run_i.config_dir, buf.zstr());

	buf << site_leaf << "/tmp";
	self.property(Run_i.temp_dir, buf.zstr());

	temp = getcwd();
	self.property(Run_i.init_cwd, getcwd());

	temp.set_bool(false);
	self.property(Run_i.page_hits,temp);

	//obj_rc services = Services::instance();
	//self.property(Run_i.services, services);

	Services* sobj = Services::cpp_global();
	sobj->set(Run_i.run_str, self);

	obj_rc config = Config::omg.new_zobj();

	sobj->setObject(config);
	
	//showobj("config", config);
	//config.property(Run_i.services, services);
	self.property(Run_i.config_str, config);
	sobj->set(Run_i.config_str, config);

	htab_rc start_args;
	htab_rw args(start_args);

	args.push_back(Run_i.begin_str);
	args.push_back(stime);

	obj_rc stats = ReflectCache::staticInstanceArgs(Run_i.phpstats_class, start_args);
	sobj->set(Run_i.phpstats_str, stats);

	temp = stats;
	self.property(Run_i.stats_str, temp);

	obj_rc dos = ReflectCache::staticInstance(Run_i.dos_class);
	sobj->set(Run_i.dos_str, dos);
}


obj_ptr 
Run::setup_world()
{
	obj_rc config = Services::service(Run_i.config_str);
	str_rc app_class = config.property(Run_i.app_class);

	obj_rc site = ReflectCache::staticInstance(app_class);

	obj_ptr self = this->self();

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

	obj_ptr config = self.property(Run_i.config_str);
	str_rc  config_dir = self.property(Run_i.config_dir);

	str_rc cryptic_data = config.property(Run_i.cryptic_str);

	if (!cryptic_data.ok())
	{
		return result;
	}
	str_buf buf;

	buf << config_dir << '/' << cryptic_data;
	str_rc path = buf.zstr();
	

	Services* sobj = Services::cpp_global();

	val_return data;
	if (file_exists(path))
	{	
		obj_rc cache_mgr = sobj->get(Run_i.cache_mgr);
		CacheMgr *cmgr = zobj_toc<CacheMgr>(cache_mgr);
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
	return result;
	

}

void Run::temp_folders()
{

	obj_ptr self = this->self();

	obj_rc config = self.property(Run_i.config_str);

	str_rc temp_dir = self.property(Run_i.temp_dir);

	//showstr("temp folder", temp_dir);

	str_buf buf;

	buf << temp_dir << "/log/errors.txt";

	config.property(Run_i.error_log, buf.zstr());

	htab_rc folder_names = config.property(Run_i.temp_folder_names);

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
		return;
	}


}

void Run::shutdown()
{

	Services* sobj = Services::cpp_global();

	if (php_get_session_status() == php_session_active)
	{
		obj_rc user_session = sobj->get(Run_i.user_session);
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
	

	obj_rc cache_mgr = sobj->get(Run_i.cache_mgr);
	CacheMgr *cmgr = zobj_toc<CacheMgr>(cache_mgr);
	cmgr->flush_caches();
	//zend_printf("Caches written\n");

	obj_rc cfg = sobj->get(Run_i.config_str);
	Config* cobj = zobj_toc<Config>(cfg);
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
	htab_rc bcfg;

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
	val_ptr test(value.value_);

	if (test.isArray())
	{
		bcfg = test.zarray();
		str_rc target = self.property(Run_i.target);
		target.lowercase();

		//showstr("target", target);

		tlist = bcfg.get(target);
		if (!tlist.ok())
		{
			tlist = bcfg.get(Run_i.default_str);
		}

	}

	if (!tlist.isArray()) {
		result.error() << "No Array returned from bootstrap " << path;
		return result;
	}
	

	bcfg = tlist.zarray();
	//showdata("bcfg", bcfg);

	test = bcfg.get(Run_i.assets_str);

	if (test.isArray())
	{
		htab_ptr assets = test.zarray();
		transfer_str(Run_i.web_dir, assets, self);
		transfer_str(Run_i.theme_str, assets, self);
	}
	

	tlist = self.property(Run_i.is_web);
	//showmem("is_web", value);


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

		//showmem("php select", value);
		if (tlist.isArray())
		{
			obj_rc cache_mgr = Services::service(Run_i.cache_mgr);
			CacheMgr*  cmgr = zobj_toc<CacheMgr>(cache_mgr);
			Config*    cfg = zobj_toc<Config>(config);

			htab_walk wk;

			auto cfg_path = wk.value();

			for(wk.start(tlist.zarray()); wk.ok(); wk.next())
			{
				buf << config_dir << "/" << cfg_path.zstr();
				path = buf.zstr();
				//showstr("config ", path);

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
						//showarray("config data", tfer);
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
			//showmem("Get config namespaces !\n", ns);
			if (ns.isArray())
			{
				obj_rc finder = Services::service(Run_i.finder);
				//showobj("Finder service", finder);

				Finder* fd = zobj_toc<Finder>(finder);
				fd->addPathArray(ns.zarray());
			}
			else {
				zend_printf("No %s entry in config!\n", Run_i.namespaces_str.data());

			}
			str_buf mpath;

			mpath << config_dir << "/modules";

			str_ptr modules_dir = mpath.zstr();

			//showstr("modules_dir", modules_dir);

			if (is_dir(modules_dir))
			{
				htab_rc flist = Finder::dirList_dir(modules_dir);
				val_rc  flistarg(flist);
				self.property(Run_i.modules_str, flistarg);
			}

		}
		
	}
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

	if (!args.throw_errors())
	{
		Run* cobj = zval_toc<Run>(ZEND_THIS);
		error_return result = cobj->execute(bootstrap);
		result.throw_errors();
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