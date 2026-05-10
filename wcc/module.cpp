#ifndef WCC_MODULE_CPP
#define WCC_MODULE_CPP


#ifndef MODULE_WCC_H
#include "module.h"
#endif

#ifndef WCC_SERVICES_H
#include "services.h"
#endif

#ifndef WCC_ROUTE_H
#include "route.h"
#endif

#ifndef WCC_FINDER_H
#include "finder.h"
#endif

#ifndef WCC_ASSETS_H
#include "assets.h"
#endif

#ifndef ARGINFO_MODULE_H
#define ARGINFO_MODULE_H
extern "C" {
	#include "stub/module_arginfo.h"
}
#endif
/*
#ifndef WCC_RUN_H
#include "run.h"
#endif
*/

namespace wcc {

using namespace zpp;

Module::Module_Mgr<Module> Module::omg;


void 
Module_init::init()
{
	DEFAULT_MOD = "default";
	BASE = "base";
	ALIAS = "alias";
	ROUTES = "routes";

	VIEWPATHS = "view_paths";
	NAMESPACES = "namespaces";
	REQUIRES = "requires";
	CLASSFILES = "classfiles";

	DATABASE = "database";
	ASSETS = "assets";
	ASSET_FILE = "asset_file";

	dispatch_str = "dispatch";

	views_str = "views";
	addpatharray_fn = "addpatharray";
	addclasses_fn = "addclasses";
	loadassetfile_fn = "loadassetfile";

	setmodule_fn = "setmodule";
	dot_str = ".";
	active_str = "active";
	cfg_path_str = "cfg_path";

	name_str = "name";

}

Module_init MODi;

void Module::debug_info(htab_rw hw)
{
	hw.set(MODi.name_str, name_);
	hw.set(MODi.REQUIRES, requires_);
	hw.set(MODi.active_str, active_);
	//hw.set(MODi.cfg_path_str, cfg_path_);
	
	Config::debug_info(hw);
}


void
Module::construct(str_ptr name, htab_ptr data)
{
	active_ = false;
	name_ = name;
	Config::construct(data);
}



str_rc
Module::getName()
{
	return name_;
}

void Module::activate(obj_ptr finder)
{
	obj_ptr self = self_;
	
	
	str_rc def_name = self.str_property(MODi.DEFAULT_MOD);
	obj_rc dispatch;
	val_rc temp_arg;
	htab_rc plist;

	if (def_name.size() && (zs_cmp(def_name, MODi.DEFAULT_MOD)!=0))
	{
		showstr("def_name", def_name);
	
		val_rc val_dispatch = Services::service(MODi.dispatch_str);
		if (val_dispatch.isObject())
		{
			dispatch = val_dispatch.zobject();
			val_rc sarg(def_name);
			obj_rc defmod = dispatch.call(MODi.setmodule_fn, sarg);
			addDefaults(defmod);
		}
		
	}

	str_rc base = self.str_property(MODi.BASE);
	if (base.size())
	{
		str_buf buf;

		str_rc rval = self.str_property(MODi.ROUTES);

		if (!rval.size())
		{
			buf << base << '/' << MODi.ROUTES;
			rval = buf.zstr();
			self.property(MODi.ROUTES, rval);
		}

		temp_arg = self.property(MODi.VIEWPATHS);

		if (temp_arg.isNull())
		{
			buf << base << '/' << MODi.views_str;
			rval = buf.zstr();
			self.property(MODi.VIEWPATHS, rval);
		}
	}
	
	plist = self.array_property(MODi.NAMESPACES);

	if (plist.size())
	{

		temp_arg = plist;
		finder.call(MODi.addpatharray_fn, temp_arg);
	}
	
	plist = self.array_property(MODi.CLASSFILES);

	if (plist.size())
	{

		temp_arg = plist;
		finder.call(MODi.addclasses_fn, temp_arg);
	}

	temp_arg  = self.property(MODi.REQUIRES);

// TODO: Should this be array merge instead of assign?
	//showmem("requires", temp_arg);

	// transfer from property to hidden property
	if (temp_arg.isArray())
	{
		requires_ = temp_arg.zarray();
	}
	else if (temp_arg.isString())
	{
		// add to array
		htab_rw req(requires_);
		req.clear();
		req.push_back(temp_arg);
	}




	str_rc asset_file = self.str_property(MODi.ASSET_FILE);

	if (asset_file.size())
	{
		
		str_rc dir = dirname(asset_file);

		if (!dir.size() || (zs_cmp(dir, MODi.dot_str)==0))
		{
			// absolute path
			str_buf buf;

			buf << cfg_path_ << '/' << asset_file;

			asset_file = buf.zstr();

			self.property(MODi.ASSET_FILE, asset_file);
		}

		obj_rc asset_mgr = Services::service(MODi.ASSETS);

		if (asset_mgr.ok())
		{
			
			Assets* asmgr = zobj_toc<Assets>(asset_mgr);
			htab_rc added = asmgr->loadAssetFile(asset_file);

			htab_rc asset_keyslist = self.array_property(MODi.ASSETS);
			if (!asset_keyslist.size())
			{
				asset_keyslist = htab_ptr::empty_array();
			}

			htab_rw asset_keys(asset_keyslist);

			asset_keys.merge(added);
			self.property(MODi.ASSETS, asset_keyslist);
		}
	}
	
	active_ = true;

}

void
Module::addDefaults(obj_ptr defmod)
{
	Module* def = zobj_toc<Module>(defmod);

	htab_rc list = def->toArray();

	htab_walk  wk;

	obj_ptr self = self_;

	auto key = wk.key();
	auto value = wk.value();
	for(wk.start(list); wk.ok(); wk.next())
	{
		if (!self.has_property(key))
		{
			self.property(key, value);
		}
	}
}

// convert string value into list[value]
htab_rc 
Module::getValueList(str_ptr key)
{
	htab_rc list;

	obj_ptr self = self_;
	val_rc  value = self.property(key);
	if (value.isString())
	{
		htab_rw item(list);
		item.push_back(value.zstr());
		return list;
	}
	if (value.isArray())
	{
		list = value.zarray();
		return list;
	}

	list = htab_ptr::empty_array();
	return list;
}

htab_rc 
Module::getAssets()
{
	return getValueList(MODi.ASSETS);
}

htab_rc 
Module::getViewPaths()
{
	return getValueList(MODi.VIEWPATHS);
}


str_rc Module::getConfigPath()
{
	return cfg_path_;
}

void Module::setConfigPath(str_ptr path)
{
	cfg_path_ = path;
	//obj_ptr(self_).property(MODi.cfg_path_str, path);
}

void Module::setActive(bool val)
{
	active_ = val;
}

bool Module::getActive()
{
	return active_;
}

htab_rc
Module::getRequires()
{
	return requires_;
}

void Module::setRequires(htab_ptr rlist)
{
	requires_ = rlist;
}
}//end wcc


using namespace wcc;
using namespace zpp;

ZEND_METHOD(Wcc_Module, __construct)
{
	str_ptr  name;
	htab_ptr data;

	zarg_rd args(execute_data);

	name = args.str(args.need(0));
	data = args.htab(args.need(1));

	if (!args.throw_errors())
	{
		Module* cobj = zval_toc<Module>(ZEND_THIS);
		cobj->construct(name, data);
	}

}

ZEND_METHOD(Wcc_Module, activate)
{
	obj_ptr finder;

	zarg_rd args(execute_data);

	args.obj_ofclass(finder,args.need(0), Finder::omg.classEntry());

	if (!args.throw_errors())
	{
		Module* cobj = zval_toc<Module>(ZEND_THIS);
		cobj->activate(finder);
	}
}
	
ZEND_METHOD(Wcc_Module, addDefaults)
{
	obj_ptr defmod;

	zarg_rd args(execute_data);

	args.obj_ofclass(defmod, args.need(0), Module::omg.classEntry());

	if (!args.throw_errors())
	{
		Module* cobj = zval_toc<Module>(ZEND_THIS);
		cobj->addDefaults(defmod);
	}
}

ZEND_METHOD(Wcc_Module, getAssets)
{
	ZEND_PARSE_PARAMETERS_NONE();
	Module* cobj = zval_toc<Module>(ZEND_THIS);
	htab_rc result = cobj->getAssets();
	result.move_zv(return_value);

}
	
ZEND_METHOD(Wcc_Module, getName)
{
	ZEND_PARSE_PARAMETERS_NONE();
	Module* cobj = zval_toc<Module>(ZEND_THIS);
	str_rc result = cobj->getName();
	result.move_zv(return_value);
}
	
ZEND_METHOD(Wcc_Module, getRequires)
{
	ZEND_PARSE_PARAMETERS_NONE();
	Module* cobj = zval_toc<Module>(ZEND_THIS);
	htab_rc result = cobj->getRequires();
	result.move_zv(return_value);
}


ZEND_METHOD(Wcc_Module, setRequires)
{
	zarg_rd args(execute_data);

	htab_ptr rlist = args.htab(args.need(0));

	if (!args.throw_errors())
	{
		Module* cobj = zval_toc<Module>(ZEND_THIS);
		cobj->setRequires(rlist);		
	}
}


ZEND_METHOD(Wcc_Module, getActive)
{
	ZEND_PARSE_PARAMETERS_NONE();
	Module* cobj = zval_toc<Module>(ZEND_THIS);
	bool result = cobj->getActive();
	RETURN_BOOL(result);
}


ZEND_METHOD(Wcc_Module, setActive)
{
	zarg_rd args(execute_data);

	bool val = true;

	args.zbool(val, args.need(0));

	if (!args.throw_errors())
	{
		Module* cobj = zval_toc<Module>(ZEND_THIS);
		cobj->setActive(val);
	}
}

ZEND_METHOD(Wcc_Module, getViewPaths)
{
	ZEND_PARSE_PARAMETERS_NONE();
	Module* cobj = zval_toc<Module>(ZEND_THIS);
	htab_rc result = cobj->getViewPaths();
	result.move_zv(return_value);
}
	
ZEND_METHOD(Wcc_Module, getConfigPath)
{
	ZEND_PARSE_PARAMETERS_NONE();
	Module* cobj = zval_toc<Module>(ZEND_THIS);
	str_rc result = cobj->getConfigPath();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Module, setConfigPath)
{
	str_ptr  path;
	zarg_rd args(execute_data);

	path = args.str(args.need(0));

	if (!args.throw_errors())
	{
		Module* cobj = zval_toc<Module>(ZEND_THIS);
		cobj->setConfigPath(path);
	}
}


PHP_MINIT_FUNCTION(Wcc_Module_reg)
{
	//auto ce = register_class_Wcc_Config(zend_ce_arrayaccess, zend_ce_countable);
	//zend_standard_class_def
	auto ce = register_class_Wcc_Module(Config::omg.classEntry());
	Module::omg.classEntry(ce);

	STATE_INIT_ADD(MODi)

	return SUCCESS;
}


#endif