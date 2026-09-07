#ifndef WCC_ASSETS_CPP
#define WCC_ASSETS_CPP

#ifndef WCC_ASSETS_H
#include "assets.h"
#endif

#ifndef ICACHE_H
#include "icache.h"
#endif

#ifndef WCC_LOADER_H
#include "loader.h"
#endif

#ifndef WCC_CONFIG_H
#include "config.h"
#endif

#ifndef SEARCH_LIST_H
#include "search_list.h"
#endif

#ifndef WCC_SERVICES_H
#include "services.h"
#endif

#ifndef WCC_REPLACE_H
#include "replace.h"
#endif

#ifndef WCC_RUNSA_H
#include "run.h"
#endif

#ifndef FN_CALL_H
#include "zpp/fn_call.h"
#endif

#ifndef WCC_CACHEMGR_H
#include "cachemgr.h"
#endif

#ifndef MODULE_WCC_H
#include "module.h"
#endif

//#define DBG_ASSETS

// For message about duplicate asset name
#ifndef WCC_DEBUGLOG_H
#include "debuglog.h"
#endif

#ifndef ASSETS_ARGINFO_H
#define ASSETS_ARGINFO_H

extern "C" {
	#include "stub/assets_arginfo.h"
}
#endif

namespace wcc {

using namespace zpp;

base_obj_mgr<Assets> Assets::omg;



void ASinit::init() 
	{
		assets_cfg = "assets_cfg";
		assets_str = "assets";
		body_blob = "bodyBlobs";

		config_dir = "config_dir";
		classname_str = "classname";

		cache_mgr =  "cache_mgr";
		css_str =    "css";
		file_cache = "file_cache";
		fwd_slash = "/";

		head_blob = "headBlobs";
		inline_styles = "inline_styles";

		js_inline = "js-inline";
		js_str = "js";
		link_str = "link";
		mark_str = "mark";
		order_str = "order";

		prop_expr = R"(#@([a-zA-Z][\w\d]*)#)";
		read_cache = "readcache";
		requires_str = "requires";
		run_str = "run";

		script_tag = "<script>";
		source = "source";

		src_paths = "src_paths";
		script_end = "</script>";
		style_end = "</style>";
		style_tag = "<style>";
		web_dir = "web_dir";

		active_str = "active";
		modules_str = "modules";
		modcfg_str = "modcfg";
		loaded_str = "loaded";
	}

ASinit  ASI;


str_rc 
Assets::findSourceFile(str_ptr path)
{
	SearchList* sp = zobj_toc<SearchList>(src_paths_);
	str_rc test = sp->findLeaf(path);
	return test;
}

htab_rc  
Assets::getWebList(str_ptr selector,
	 val_ptr names, bool aslist, bool unset)
{
	htab_rc result;
	htab_rc order;
	if (names.isNull() || names.empty())
	{
		order = order_;
	}
	else if (names.isString())
	{
		htab_rw hw(order);
		hw.push_back(names.zstr());
	}
	else {
		order = names.zarray();
	}

	for_key_value w1;
	htab_rw hw(result);

	for(w1.start(order); w1.ok(); w1.next())
	{
		str_ptr name = w1.value();
		// get the actual zval address of the property, 
		// so it can be updated (like a reference)
		// and make it writeable.
		// as assets_ is functionally private, 
		// this copy is hopefully not actually done ( if rc is still 1)
		htab_rw asset(assets_.property_ptr(name));
		// if (!empty)
		if (asset.size())
		{
			htab_rc items = asset.get(selector);

			if (items.size())
			{
				if (unset)
				{
					asset.unset(selector);
				}

				if (aslist)
				{
					htab_walk w2;
					auto wpath = w2.value();
					for(w2.start(items); w2.ok(); w2.next())
					{
						hw.push_back(wpath);
					}
				}
				else {
					hw.push_back(items);
				}
			}
		}
	}
	return result;
}

void 
Assets::setRun(obj_ptr obj)
{
	run_ = obj;
	obj_ptr run = run_;

	web_ = run.str_property(ASI.web_dir);

	str_rc assets_file = run.str_property(ASI.assets_cfg);

	if (assets_file.size())
	{
		this->loadAssetFile(assets_file);
	}
}

str_rc 
Assets::jsPut()
{
	str_buf buf;

	render_lock_ = true;
	htab_rc paths = getWebList(ASI.js_str);
	for_key_value kv1;

	for(kv1.start(paths); kv1.ok(); kv1.next())
	{
		str_rc wpath = kv1.value();
		if (verify_path(wpath)) {
			buf << script_wrap(wpath);
		}
		else 
			break;
	}
	return buf.zstr();
}

str_rc 
Assets::jsPull(str_ptr name)
{
	str_buf buf;

	render_lock_ = true;
	val_rc arg(name);
	htab_rc paths = getWebList(ASI.js_str, arg, true, true);
	for_key_value kv1;

	for(kv1.start(paths); kv1.ok(); kv1.next())
	{
		str_rc wpath = kv1.value();
		if (verify_path(wpath)) {
			buf << script_wrap(wpath);
		}
		else 
			break;
	}
	return buf.zstr();
}

bool
Assets::jsInline()
{
	htab_rc ipaths = getWebList(ASI.js_inline, val_ptr(), false);
	for_key_value w1;
	for(w1.start(ipaths); w1.ok(); w1.next())
	{
		htab_rc jsline = w1.value();
		htab_rc slist = jsline.get(ASI.source);
		if (slist.size())
		{
			for_key_value w2;
			for(w2.start(slist); w2.ok(); w2.next())
			{
				str_rc wpath = w2.value();

				if ( web_path(wpath) )
				{
					str_rc script =  file_content(wpath);
					if (script.size())
					{
						str_buf buf;
						buf << ASI.script_tag << script << ASI.script_end;
						script = buf.zstr();
						addBlob(script);
					}
				}
				else {
					zend_throw_error(zend_ce_error,"js-inline source %s not found.", wpath.data());
					return false;
				}
			}
		}	
		
	}
	return true;
}

Assets::Assets() : base_d(), render_lock_(false)
{
}

error_return 
Assets::construct()
{
	//zend_printf("Assets::construct\n");
	error_return result;

	val_rc nullval; // null value


	src_paths_ = SearchList::omg.new_zobj();
	SearchList* slist = zobj_toc<SearchList>(src_paths_);
	slist->construct(nullval);

	assets_ = Config::omg.new_zobj();

	val_return vret = Services::service(ASI.run_str);

	if (vret.has_errors())
	{
		result = vret.move_error();
		return result;
	}

	obj_rc run_app = vret.value_.zobject();
	if (run_app.instanceof(Run::omg.classEntry()))
	{
		setRun(run_app);
	}
	else if (run_app.ok())
	{ 
		run_ = run_app;
	}
	else {
		// from some really old code
		run_ = Config::omg.new_zobj();
	}

	// modules management
	throwIfDuplicate_ = false;
	modules_ = htab_ptr::empty_array();
	moduleCfg_  = htab_ptr::empty_array();
	loaded_ = htab_ptr::empty_array();

	return result;

}

void 
Assets::debug_info(htab_rw di)
{
	di.set(ASI.src_paths, src_paths_);
	di.set(ASI.assets_str, assets_);
	di.set(ASI.order_str, order_);
	di.set(ASI.mark_str, mark_);
	di.set(ASI.inline_styles, inline_styles_);
	di.set(ASI.web_dir, web_);
	di.set(ASI.head_blob, headBlob_);
	di.set(ASI.body_blob, bodyBlob_);

	di.set(ASI.modules_str, modules_);
	di.set(ASI.active_str, activeModule_);
	di.set(ASI.modcfg_str, moduleCfg_);
	di.set(ASI.loaded_str, loaded_);

}

void Assets::destruct()
{
	run_.init();
	src_paths_.init();
}

str_rc 
Assets::cssHeader()
{
	render_lock_ = true;
	str_buf buf;
	htab_rc paths = getWebList(ASI.css_str);
	
	if (paths.size())
	{
		for_key_value kv1;

		for(kv1.start(paths); kv1.ok(); kv1.next())
		{
			str_rc wpath = kv1.value();
			if (!verify_path(wpath))
			{
				break;
			}
			buf << link_css(wpath);
		}
	}
	return buf.zstr();
}

str_rc //static 
Assets::script_wrap(str_ptr path)
{
	str_buf buf;

	buf << R"(<script src=")" << path << R"("></script>)" << endl;

	return buf.zstr();
}

str_rc //static 
Assets::link_css(str_ptr webpath)
{
	str_buf buf;

	buf << R"(<link rel="stylesheet" type="text/css" href=")"
	    << webpath << R"(">)" << '\n';
	return buf.zstr();
}

bool 
Assets::markAdd(str_ptr item)
{
	if (!mark_.has_key(item))
	{
		Config* cfg = zobj_toc<Config>(assets_);

		val_rc data;

		data = cfg->getOrNot(item, data);

		if (data.isNull())
		{
			zend_throw_error(zend_ce_error, "Asset Key '%s' not found.", item.data());
			return false;
		}
		htab_ptr alist = data.zarray();
		if (alist.size())
		{
			val_ptr req_names = alist.get(ASI.requires_str);
			if (req_names.ok())
			{
				this->add(req_names);
			}
			htab_rw wo(order_);
			wo.push_back(item);

			htab_rw wm(mark_);
			wm.setbool(item,true);
		}
	}
	return true;
}

// create file at wpath, by topy from
// one of listed source folders
bool
Assets::source_path(str_ptr wpath)
{

	str_rc srcfile = findSourceFile(wpath);

	bool exists = (srcfile.size()) ? true : false;
	if (exists)
	{
		zend_result copied = php_copy_file(srcfile.data(), wpath.data());
		if (copied != SUCCESS)
		{
			exists = false;
		}
	}
	if (!exists)
	{
		zend_throw_error(zend_ce_error,"File '%s' not found", wpath.data());
	}
	return exists;
}

bool 
Assets::web_path(str_rc& path)
{
	str_rc fpath(path);
	int check = fpath.find('@');
	if (check >= 0)
	{
		Replace path_subst(run_);
		fpath = path_subst.eval(fpath);
	}
	if (fpath.starts_with(ASI.fwd_slash))
	{
		fpath = web_ + fpath;
	}

	bool exists = file_exists(fpath);

	if(!exists)
	{
		exists = this->source_path(fpath);
	}

	if (exists)
	{
		path = fpath;
	}
	return exists;
}

bool 
Assets::verify_path(str_rc& p_inout)
{
	str_rc fpath(p_inout);

	int check = fpath.find('@');
	if (check >= 0)
	{
		#ifdef DBG_ASSETS
		DebugLog* log = DebugLog::cpp_global();
		if (log)
		{
			log->dump("lookup object @replace", run_);
			log->dump("string replace", fpath);
		}
		#endif

		Replace path_subst(run_);

		fpath = path_subst.eval(fpath);

		#ifdef DBG_ASSETS
		if (log)
		{
			log->dump("result string", fpath);
		}
		#endif

		p_inout = fpath;
	}
	str_rc real_path = fpath;
	// starting with '/'
	if (fpath.starts_with(ASI.fwd_slash)) 
	{
		real_path = web_ + real_path;
	}

	if (!file_exists(real_path)) 
	{
		if (this->source_path(real_path))
		{
			return true;
		}
		else {
			return false;
		}
	}
	return true;
}

void 
Assets::add(val_ptr nlist)
{
	if (render_lock_) {
		zend_throw_error(zend_ce_error, "Assets locked during render");
		return;
	}

	if (nlist.isArray())
	{
		for_key_value loop;
		for(loop.start(nlist.zarray()); loop.ok(); loop.next())
		{
			str_rc name = loop.value();
			if (!this->markAdd(name))
				break;
		}
	}
	else if (nlist.isString())
	{
		this->markAdd(nlist.zstr());
	}
}

htab_return 
Assets::addAssets(htab_ptr data)
{
	htab_return result;

	htab_walk wk;
	auto key = wk.key();
	auto value = wk.value();

	htab_rw hw(result.value_);

	bool duplicate = throwIfDuplicate_;
	Config* asp = zobj_toc<Config> (assets_);

	for(wk.start(data); wk.ok(); wk.next())
	{
		str_rc strkey = key.zstr();

		if (asp->has(strkey)) {
			if (duplicate)
			{
				result.error() << "Duplicate Asset Key: " << strkey;
				return result;
			}
			else {
				DebugLog* log = DebugLog::cpp_global();
				if (log) {
					str_buf buf;
					buf << "Asset set over-write: " << strkey;
					log->line(buf.zstr());
				}
			}
		}
		hw.push_back(strkey);

		assets_.property(key.zstr(), value);
	}
	//showdata("keys_added", keys_added);

	return result;
}

void 
Assets::addBlob(str_rc blob, bool header)
{
	if (header)
	{
		htab_rw hw(headBlob_);
		hw.push_back(blob);
	}
	else {
		htab_rw hw(bodyBlob_);
		hw.push_back(blob);
	}
}

htab_ptr 
Assets::addSourcePath(str_ptr path)
{
	SearchList* paths = zobj_toc<SearchList>(src_paths_);
	paths->addPath(path);
	return paths->getPaths();
}

void 
Assets::addStyle(str_ptr style)
{
	str_ptr estr = str_ptr::empty_str();

	str_rc sct = str_replace(ASI.style_tag, estr, style);
	sct = str_replace(ASI.style_end, estr, sct);
	htab_rw hw(inline_styles_);

	hw.push_back(sct);
}

str_rc 
Assets::footer()
{
	str_rc result;
	
	render_lock_ = true;
	if (!this->jsInline())
	{
		return result;
	}
	str_buf buf;

	str_rc temp = jsPut();
	buf << temp;

	temp = implode_blob(bodyBlob_);
	buf << temp;
	result = buf.zstr();
	
	return result;
}

str_rc implode_blob(htab_ptr blobs)
{
	str_rc result;

	if (blobs.size())
	{
		result = implode(str_ptr::empty_str(), blobs);
	}
	return result;
}


obj_ptr 
Assets::getSearchList()
{
	return src_paths_;
}


bool 
Assets::has(str_ptr key)
{
	return assets_.has_property(key);
}

str_rc 
Assets::header()
{
	render_lock_ = true;
	return implode_blob(headBlob_);
}


str_rc 
Assets::inline_css(str_ptr name)
{
	val_rc temp(name);
	htab_rc paths = getWebList(ASI.css_str, temp);
	str_buf buf;

	Replace pathnames(run_, ASI.prop_expr);

	if (paths.size())
	{
		for_key_value w1;

		for(w1.start(paths); w1.ok(); w1.next())
		{
			str_rc css_path = w1.value();
			if (css_path.size())
			{
				css_path = pathnames.eval(css_path);
				css_path = findSourceFile(css_path);

				str_rc data = file_content(css_path);
				buf << endl << ASI.style_tag << endl;
				buf << data << endl << ASI.style_end << endl;
			}
			else {
				zend_throw_error(zend_ce_error,"Empty css path for %s", name.data());
				break;
			}
		}
	}
	return buf.zstr();
}

str_rc 
Assets::link()
{
	render_lock_ = true;

	htab_rc list = getWebList(ASI.link_str,val_ptr(), false);

	str_buf buf;

	for_key_value kv1;

	for(kv1.start(list); kv1.ok(); kv1.next())
	{
		htab_ptr ltab = kv1.value();
		if (ltab.size())
		{
			for_key_value kv2;
			buf << "<link";

			for(kv2.start(ltab); kv2.ok(); kv2.next())
			{
				str_ptr attr = kv2.key();
				str_ptr val = kv2.value();
				buf << ' ' << attr << '=';
				buf.quote_name(val);
			}
			buf << '>' << endl;
		}
	} 
	return buf.zstr();
}

htab_rc 
Assets::filterPaths(htab_ptr paths)
{
	htab_rc result;

	if (paths.size())
	{
		htab_rw hw(result);
		Replace pathnames(run_, ASI.prop_expr);
		for_key_value kv1;
		for(kv1.start(paths); kv1.ok(); kv1.next())
		{
			str_rc sp = kv1.value();

			sp = pathnames.eval(sp);
			hw.push_back(sp);
		}
	}
	return result;
}

htab_return 
Assets::loadAssetFile(str_ptr file)
{
	//printf("Assets::loadAssetFile %s\n", file.data());

	htab_return result;

	if (! file_exists(file))
	{
		result.error() << "Asset file '" << file << "' not found";
		return result;
	}

	error_return cret;

	CacheMgr* cmgr = CacheMgr::instance(cret);

	if(cret.has_errors())
	{
		result = cret.move_error();
		return result;
	}

	val_return vdata = cmgr->readCache(file, ASI.file_cache);

	if (vdata.has_errors())
	{
		result = vdata.move_error();
		return result;
	}

	htab_rc data = vdata.value_.zarray();
	if (!data.ok())
	{
		data = htab_ptr::empty_array();
	}

	//showdata("cached data:  ", data);

	htab_rc paths;

	val_ptr paths_v = data.get(ASI.src_paths);
	if (paths_v.isString())
	{
		htab_rw hw(paths);
		hw.push_back(paths_v.zstr());
	}
	else {
		paths = paths_v.zarray();
	}
	if (paths.size())
	{
		paths = filterPaths(paths);
		SearchList* slist = zobj_toc<SearchList>(src_paths_);
		slist->addPaths(paths);
	}

	val_rc temp = data.get(ASI.assets_str);
	if (temp.isObject())
	{
		obj_rc cfg_obj = temp.zobject();
		if (cfg_obj.ok())
		{
			Config* cfg = zobj_toc<Config>(cfg_obj);
			temp = cfg->toArray();
		}
	}
	if (temp.isArray())
	{
		result = this->addAssets(temp.zarray());
		//showdata("result", result);
	}
	return result;
}

void 
Assets::reset()
{
	render_lock_ = false;
	order_.init();
	mark_.init();
}

str_rc 
Assets::styleHeader()
{
	str_buf buf;

	if (inline_styles_.size())
	{
		buf << ASI.style_tag << endl;
		for_key_value kv1;
		for(kv1.start(inline_styles_); kv1.ok(); kv1.next())
		{
			str_ptr s = kv1.value();
			buf << s;
		}
		buf << ASI.style_end << endl;
	}
	return buf.zstr();
}



str_rc 
Assets::implode_blob(htab_ptr blobs)
{
	str_ptr estr = str_ptr::empty_str();
	if (blobs.size())
	{
		return implode(estr, blobs);
	}
	else {
		return estr;
	}
}

void 
Assets::unmark(str_ptr item)
{
	if (mark_.has_key(item)) 
	{
		int ix = order_.value_index(item);
		if (ix >= 0)
		{
			array_splice(this->order_, ix, 1);
		}
		htab_rw hw(mark_);
		hw.unset(item);
	}
}

bool_return 
Assets::clearCache()
{
	bool_return result;

	val_return cache_mgr_err = Services::service(ASI.cache_mgr);
	if (cache_mgr_err.has_errors())
	{
		result = cache_mgr_err.move_error();
		return result;
	}
	obj_ptr cache = cache_mgr_err.value_.zobject();
	if (cache.ok())
	{
		ICache* cobj = zobj_toc<ICache>(cache);

		result = cobj->clear();
	}
	return result;
}
 	// modules

obj_rc
Assets::getActiveModule()
{
	return activeModule_;
}

obj_return
Assets::addModule(str_ptr name, val_ptr data)
{
	obj_return result;
	htab_rc    mcfg;
	str_rc 	dir;

	Loader* loader = Loader::cpp_global();

	if (data.isString())
	{
		
		dir = data.zstr();
		str_buf buf;
		buf << dir << '/' << "module.php";


		
		val_return mdata = loader->require(buf.zstr());
		if (mdata.has_errors())
		{
			result = mdata.move_error();
			return result;
		}
		mcfg = mdata.value_.zarray();
	}
	else {
		dir = run_.str_property(ASI.config_dir);
		mcfg = data.zarray();
	}

	if (mcfg.size())
	{
		str_rc class_name = mcfg.get(ASI.classname_str);
		if (!class_name.size())
		{
			class_name  = Module::omg.class_name();
		}
		htab_rc args_temp;
		htab_rw args(args_temp);

		args.push_back(name);
		args.push_back(mcfg);

		obj_rc mobj = ReflectCache::staticInstanceArgs(class_name, args);
		if (!mobj.ok()) {
			result.error() << "Cannot make new " << class_name;
			return result;
		}
		result.value_ = mobj;

		Module* modo = zobj_toc<Module>(mobj);
		modo->setConfigPath(dir);

		htab_rw arr(modules_);
		arr.set(name, mobj);

		modo->activate( loader->getFinder() );

		htab_rc depends = modo->getRequires();

		if (depends.size())
		{
			for_key_value wk;

			for(wk.start(depends); wk.ok(); wk.next())
			{
				val_ptr mval = wk.value();

				str_ptr modname = mval.zstr();

				this->setModule(modname);
			}
		}
	}
	else {
		result.error() << "No array data for module: " << name;
	}
	return result;
}

obj_return 
Assets::getDefaultModule()
{
	obj_return result;

	obj_rc mdef = modules_.get(MODi.DEFAULT_MOD);

	if (mdef.ok())
	{
	    result.value_ = mdef;
		return result;
	}

	val_rc defaults = moduleCfg_.get(MODi.DEFAULT_MOD);

	if (defaults.isNull())
	{
		result.error() << "Default module '" << MODi.DEFAULT_MOD << "' must exist";
		return result;
	}
	htab_ptr va = defaults.zarray();
	if (va.size())
	{
		if (va.has_key(MODi.ALIAS)) {
			result.error() << "Default module '" << MODi.DEFAULT_MOD << "' must not have " << MODi.ALIAS;
			return result;
		}
	}
	else {
		str_ptr sd = defaults.zstr();
		if (!sd.size())
		{
			result.error() << "Default module '" << MODi.DEFAULT_MOD << "' must be array or string value";
			return result;
		}
	}
	return addModule(MODi.DEFAULT_MOD, defaults);
}

str_rc 
Assets::getModuleName()
{
	str_rc result;

	if (activeModule_.ok())
	{
		Module* modo = zobj_toc<Module>(activeModule_);
		result = modo->getName();
	}
	return result;
}

obj_rc 
Assets::getModule(str_ptr name)
{
	obj_rc result = modules_.get(name);
	return result;
}

obj_return 
Assets::setModule(str_ptr name)
{

#ifdef DBG_ASSETS
	DebugLog* log = DebugLog::cpp_global();
	if (log)
	{
		log->dump("Assets setModule", name);
	}
#endif

	obj_return result;
	obj_rc modo = modules_.get(name);

	if (modo.ok())
	{
		activeModule_ = modo;
		result.value_ = modo;
		return result;
	}

	obj_return mdef_err = getDefaultModule();

	if (mdef_err.has_errors())
	{
		result = mdef_err.move_error();
		return result;
	}
	obj_rc mdef = mdef_err.value_;

	val_rc mdata = moduleCfg_.get(name);

	if (mdata.isString() && (zs_cmp(name, MODi.DEFAULT_MOD)!=0))
	{
		 result = addModule(name, mdata);
		 return result;
	}
	if (!mdata.isArray())
	{
		activeModule_ = mdef;
		result.value_ = mdef;
		return result;
	}

	htab_ptr mda = mdata.zarray();
	val_rc alias_val = mda.get(MODi.DEFAULT_MOD);
	if(alias_val.isString())
	{
		mdef_err = setModule(alias_val.zstr());
		if (mdef_err.has_errors())
		{
			result = mdef_err.move_error();
			return result;
		}
		mdef = mdef_err.value_;
	}
	result = addModule(name, mdata);
	if (result.has_errors())
	{
		return result;
	}
	modo = result.value_;
	Module* m = zobj_toc<Module>(modo);
	m->addDefaults(mdef);

	activeModule_ = modo;
	return result;
}

void 
Assets::setModuleCfg(htab_ptr modlist)
{
	moduleCfg_ = modlist;
}



}; //end namespace wcc
//============================================================================================

using namespace wcc;
using namespace zpp;

ZEND_METHOD(Wcc_Assets, __construct)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Assets* cobj = zval_toc<Assets>(ZEND_THIS);

	error_return test = cobj->construct();
	if (test.throw_errors())
	{
		// !what to do?
	}
}

ZEND_METHOD(Wcc_Assets, __destruct)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Assets* cobj = zval_toc<Assets>(ZEND_THIS);

	cobj->destruct();
}

ZEND_METHOD(Wcc_Assets, add)
{
	zarg_rd args(execute_data);

	val_ptr list(args.need(0));

	if (!(list.isString() || list.isArray()))
	{
		args.error() << "; Expect String or Array";
	}
	if (!args.throw_errors(__FUNCTION__))
	{
		Assets* cobj = zval_toc<Assets>(ZEND_THIS);
		cobj->add(list);
	}
}

ZEND_METHOD(Wcc_Assets, addAssets)
{
	zarg_rd args(execute_data);
	htab_ptr data;
	args.zarray(data, args.need(0));

	if (!args.throw_errors(__FUNCTION__))
	{
		Assets* cobj = zval_toc<Assets>(ZEND_THIS);
		htab_return result = cobj->addAssets(data);
		if(!result.throw_errors())
		{
			result.value_.move_zv(return_value);
		}
	}
}

ZEND_METHOD(Wcc_Assets, addBlob)
{
	zarg_rd args(execute_data);
	str_ptr blob;
	bool    headblob = false;
	args.zstring(blob, args.need(0));
	args.zbool(headblob, args.option(1));
	if (!args.throw_errors(__FUNCTION__))
	{
		Assets* cobj = zval_toc<Assets>(ZEND_THIS);
		cobj->addBlob(blob, headblob);
	}
}

ZEND_METHOD(Wcc_Assets, addSourcePath)
{
	zarg_rd args(execute_data);
	str_ptr path;
	args.zstring(path, args.need(0));
	if (!args.throw_errors(__FUNCTION__))
	{
		Assets* cobj = zval_toc<Assets>(ZEND_THIS);
		cobj->addSourcePath(path);
	}
}

ZEND_METHOD(Wcc_Assets, addStyle)
{
	zarg_rd args(execute_data);
	str_ptr style;
	args.zstring(style, args.need(0));
	if (!args.throw_errors(__FUNCTION__))
	{
		Assets* cobj = zval_toc<Assets>(ZEND_THIS);
		cobj->addStyle(style);
	}
}

ZEND_METHOD(Wcc_Assets, cssHeader)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Assets* cobj = zval_toc<Assets>(ZEND_THIS);

	str_rc text = cobj->cssHeader();
	text.move_zv(return_value);
}

ZEND_METHOD(Wcc_Assets, footer)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Assets* cobj = zval_toc<Assets>(ZEND_THIS);

	str_rc text = cobj->footer();
	text.move_zv(return_value);
}

ZEND_METHOD(Wcc_Assets, getSearchList)
{
	ZEND_PARSE_PARAMETERS_NONE();
	Assets* cobj = zval_toc<Assets>(ZEND_THIS);
	obj_rc  obj = cobj->getSearchList();
	obj.move_zv(return_value);
}

/*
getWebList(
 		string $selector, 
 		null|string|array $names = null,
 		bool $list = true) : array {} */

ZEND_METHOD(Wcc_Assets, getWebList)
{
	zarg_rd args(execute_data);

	str_rc  typekey;
	val_ptr names;
	bool    aslist = true;
	bool    unset = false;

	htab_rc result;

	args.zstring(typekey, args.need(0));

    names = args.option(1);

    bool hasMore = names.ok() && (names.isString() || names.isArray());
   	// passed a 3rd arg?

	if (hasMore) {
		hasMore = args.zbool(aslist, args.option(2));
	}
	// passed a 4th arg?
	if (hasMore) {
		hasMore = args.zbool(unset, args.option(3));
	}

	if (!args.throw_errors(__FUNCTION__))
	{
		Assets* cobj = zval_toc<Assets>(ZEND_THIS);
		result = cobj->getWebList(typekey, names, aslist, unset);
		result.move_zv(return_value);
	}	
}

ZEND_METHOD(Wcc_Assets, has)
{
	zarg_rd args(execute_data);
	str_rc  namekey;

	args.zstring(namekey, args.need(0));
	if (!args.throw_errors(__FUNCTION__))
	{
		Assets* cobj = zval_toc<Assets>(ZEND_THIS);
		bool result = cobj->has(namekey);
		RETURN_BOOL(result);
	}
}

ZEND_METHOD(Wcc_Assets, header)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Assets* cobj = zval_toc<Assets>(ZEND_THIS);

	str_rc text = cobj->header();

	text.move_zv(return_value);
}

ZEND_METHOD(Wcc_Assets, inline_css)
{
	zarg_rd args(execute_data);

	str_ptr name;

	args.zstring(name, args.need(0));
	if (!args.throw_errors(__FUNCTION__))
	{
		Assets* cobj = zval_toc<Assets>(ZEND_THIS);
		str_rc text = cobj->inline_css(name);
		text.move_zv(return_value);
	}
}

ZEND_METHOD(Wcc_Assets, link)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Assets* cobj = zval_toc<Assets>(ZEND_THIS);
	str_rc text = cobj->link();
	text.move_zv(return_value);
}

ZEND_METHOD(Wcc_Assets, loadAssetFile)
{
	zarg_rd args(execute_data);

	str_ptr file = args.str(args.need(0));
	htab_return result;

	if (!args.throw_errors(__FUNCTION__))
	{
		Assets* cobj = zval_toc<Assets>(ZEND_THIS);
		result = cobj->loadAssetFile(file);
		if (!result.throw_errors())
		{
			result.value_.move_zv(return_value);
		}
	}	
}


ZEND_METHOD(Wcc_Assets, jsPull)
{
	zarg_rd args(execute_data);

	str_ptr name = args.str(args.need(0));

	if (!args.throw_errors(__FUNCTION__))
	{
		Assets* cobj = zval_toc<Assets>(ZEND_THIS);
		str_rc result = cobj->jsPull(name);
		result.move_zv(return_value);
	}	
}

ZEND_METHOD(Wcc_Assets, reset)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Assets* cobj = zval_toc<Assets>(ZEND_THIS);

	cobj->reset();
}

ZEND_METHOD(Wcc_Assets, setRun)
{
	zarg_rd args(execute_data);

	obj_ptr env;

	env = args.obj(args.need(0));

	if (!args.throw_errors(__FUNCTION__))
	{
		Assets* cobj = zval_toc<Assets>(ZEND_THIS);
		cobj->setRun(env);
	}
}

ZEND_METHOD(Wcc_Assets, styleHeader)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Assets* cobj = zval_toc<Assets>(ZEND_THIS);

	str_rc text = cobj->styleHeader();

	text.move_zv(return_value);
}

ZEND_METHOD(Wcc_Assets, unmark)
{
	zarg_rd args(execute_data);

	str_ptr item;

	args.zstring(item, args.need(0));

	if (!args.throw_errors(__FUNCTION__))
	{
		Assets* cobj = zval_toc<Assets>(ZEND_THIS);
		cobj->unmark(item);
	}
}


ZEND_METHOD(Wcc_Assets, addModule)
{
	zarg_rd args(execute_data);
	str_ptr name = args.str(args.need(0));
	val_ptr mspec = args.string_or_array(args.need(1));

	if (!args.throw_errors(__FUNCTION__))
	{
		Assets* cobj = zval_toc<Assets>(ZEND_THIS);
		obj_return result = cobj->addModule(name, mspec);
		if (!result.throw_errors(__FUNCTION__))
		{
			result.value_.move_zv(return_value);
		}
	}
}

ZEND_METHOD(Wcc_Assets, setModule)
{
	zarg_rd args(execute_data);
	str_ptr    value  = args.need(0);

	if (!args.throw_errors(__FUNCTION__))
	{
		Assets* cobj = zval_toc<Assets>(ZEND_THIS);
		obj_return result = cobj->setModule(value);
		result.value_.move_zv(return_value);
	}
}

ZEND_METHOD(Wcc_Assets, setModuleCfg)
{
	zarg_rd args(execute_data);
	htab_ptr    value  = args.htab(args.need(0));

	if (!args.throw_errors(__FUNCTION__))
	{
		Assets* cobj = zval_toc<Assets>(ZEND_THIS);
		cobj->setModuleCfg(value);
	}
}


ZEND_METHOD(Wcc_Assets, getActiveModule)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Assets* cobj = zval_toc<Assets>(ZEND_THIS);
	obj_rc result = cobj->getActiveModule();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Assets, getDefaultModule)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Assets* cobj = zval_toc<Assets>(ZEND_THIS);
	obj_return result = cobj->getDefaultModule();
	if (!result.throw_errors(__FUNCTION__))
	{
		result.value_.move_zv(return_value);
	}
}

ZEND_METHOD(Wcc_Assets, getModuleName)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Assets* cobj = zval_toc<Assets>(ZEND_THIS);
	str_rc result = cobj->getModuleName();
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_Assets, getModule)
{
	zarg_rd args(execute_data);
	str_ptr name = args.str(args.need(0));
	if (!args.throw_errors(__FUNCTION__))
	{
		Assets* cobj = zval_toc<Assets>(ZEND_THIS);
		obj_rc result = cobj->getModule(name);
		result.move_zv(return_value);
	}
}

ZEND_METHOD(Wcc_Assets, clearCache)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Assets* cobj = zval_toc<Assets>(ZEND_THIS);

	bool_return result = cobj->clearCache();

	if (!result.throw_errors(__FUNCTION__))
	{
		RETURN_BOOL(result.value_);
	}
}


PHP_MINIT_FUNCTION(wcc_assets_reg)
{

	Assets::omg.classEntry(register_class_Wcc_Assets());

	STATE_INIT_ADD(ASI)
	
	return SUCCESS;

}
#endif