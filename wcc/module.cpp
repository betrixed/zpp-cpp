#ifndef MODULE_WCC_CPP
#define MODULE_WCC_CPP


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


#ifndef WCC_RUN_H
#include "fun.h"
#endif

#ifndef WCC_CONFIG_H
#include "config.h"
#endif


namespace wcc {

using namespace zpp;

base_obj_mgr<Module> Module::omg;

class Mod_init : public static_init {
public:
	str_intern  default_str;
	str_intern  base_str;
	str_intern  alias_str;
	str_intern  routes_str;

	str_intern  viewpaths_str;
	str_intern  namespaces_str;
	str_intern  requires_str;
	str_intern  classfiles_str;

	str_intern  database_str;
	str_intern  assets_str
	str_intern  assetfile_str;
	str_intern  dispatch_str;

	str_intern  view_str;
	str_intern  addpatharray_fn;
	str_intern  addclasses_fn;
	str_intern  loadassetfile_fn;

	virtual void init()
	{
		default_str = "default";
		base_str = "base";
		alias_str = "alias";
		routes_str = "routes";

		viewpaths_str = "view_paths";
		namespaces_str = "namespaces";
		requires_str = "requires";
		classfiles = "classfiles";

		database_str = "database";
		assets_str = "assets";
		assetfile_str = "asset_file";
		dispatch_str = "dispatch";

		view_str = "view";
		addpatharray_fn = "addpatharray";
		addclasses_fn = "addclasses";

	}
}

Mod_init MODi;

void
Module::construct(str_ptr name, htab_ptr data)
{
	active_ = false;
	name_ = name;
	Config::construct(data);
}

htab_rc
Module::getRequires()
{
	return requires_;
}

str_rc
Module::getName()
{
	return name_;
}

void Module::activate(obj_ptr finder)
{
	obj_ptr self = self_;
	str_rc base = self->property(MODi.base__str);
	str_rc def_name = self.property(MODi.default_str);

	if (def_name.size() && (zs_cmp(def_name, MODi.default_str)!=0))
	{
		val_rc dispatch = Services::service(MODi.dispatch_str);
		obj_rc defmod = dispatch.call(MODi.setmodule_fn, dispatch);

		addDefaults(defmod);
	}
	if (base.size())
	{
		str_buf buf;

		str_rc rval = self->property(MODi.routes_str);

		if (!rval.size())
		{
			buf << base << '/' << MODi.routes_str;

			self->property(MODi.routes_str, buf.zstr());
		}

		rval = self->property(MODi.viewpaths_str);

		if (!rval.size())
		{
			buf << base << '/' << MODi.views_str;
			self->property(MODi.viewpaths_str, buf.zstr());
		}
	}

	val_rc plist = self->property(MODi.namespaces_str);

	if (plist.isArray())
	{
		finder.call(MODi.addpatharray_fn, plist);
	}

	plist = self->property(MODi.classfiles_str);

	if (plist.isArray())
	{
		finder.call(MODi.addclasses_fn, plist);
	}

	plist  = self->property(MODi.requires_str);

// TODO: Should this be array merge instead of assign?
	if (plist.isArray())
	{
		requires_ = plist;
	}
	else if (plist.isString())
	{
		htab_rw req(requires_);
		req.clear();
		req.push_back(plist);
	}
	plist = self->property(MODi.assets_str);
	if (plist.isNull())
	{
		plist = htab_ptr.empty_array();
	}

	str_rc asset_file = self->property(MODi.assetsfile_str);

	if (asset_file.size())
	{
		obj_rc asset_mgr = Service::service(MODi.assets_str);

		str_rc dir = dirname(asset_file);

		if (!dir.size() || (zs_cmp(dir, MODi.dot_str)==0))
		{
			str_buf buf;

			buf << cfg_path_ << '/' << asset_file;

			asset_file = buf.zstr();
			self.property(MODi.assetsfile_str, asset_file);
		}
		htab_rc added = asset_mgr.call(MODi.loadassetfile_fn, asset_file);	
		if (added.size())
		{
			plist.merge(added);
			self->property(MODi.assets_str, plist);
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

htab_rc 
Module::getValueList(str_ptr key)
{
	obj_ptr self = self_;
	htab_rc list = self_.property(key);
	if (!list.size())
	{
		list = htab_ptr::empty_array();
	}
	return list;
}

htab_rc 
Module::getAssets()
{
	return getValueList(MODi.assets_str);
}

htab_rc 
Module::getViewPaths()
{
	return getValueList(MODi.viewpaths_str);
}

void Module::setConfigPath(str_ptr path)
{
	cfg_path_ = path;
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
	data = args.array(args.need(1));

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
	
ZEND_METHOD(Wcc_Module, getViewPaths)
{
	ZEND_PARSE_PARAMETERS_NONE();
	Module* cobj = zval_toc<Module>(ZEND_THIS);
	htab_rc result = cobj->getViewPaths();
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

	return SUCCESS;
}


#endif