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

class Disp_init : public static_init {
public:
	str_intern  finder_str;
	str_intern  default_str;
	str_intern  route_match_str;
	str_intern  run_str;

	str_intern  config_str;
	str_intern  config_dir;
	str_intern  activate_fn;
	str_intern  cache_routes_str;

	str_intern  getassets_fn;
	str_intern  getviewpaths_fn;
	str_intern  engine_str;
	str_intern  getfinder_fn;
	str_intern  addpaths_fn;

	str_intern  viewdata_str;
	str_intern  sharewithall_fn;
	str_intern  redirect_str;
	str_intern  response_str;

	virtual void init()
	{
		finder_str = "finder";
		default_str = "default";
		route_match_str = "route_match";
		run_str = "run";

		config_str = "config";
		config_dir = "config_dir";
		activate_fn = "activate";
		cache_routes_str = "cache_routes";

		getassets_fn = "getassets";
		getviewpaths_fn = "getviewpaths";
		engine_str = "engine";
		getfinder_fn = "getfinder";
		addpaths_fn == "addpaths";

		viewdata_str = "view_data";
		sharewithall_fn = "sharewithall";
		redirect_str = "redirect";
		response_str = "response";
	}
}

Disp_init DSPi;

Services* 
Module::svc_ptr()
{
	return zobj_toc<Services>(services_);
}

Run*
Module::run_ptr()
{
	return zobj_toc<Run>(run_);
}

RouteMatch*
Module::rm_ptr()
{
	return zobj_toc<RouteMatch>(route_match_);
}

void
Module::construct()
{
	services_ = Services::instance();

	Services* svc = svc_ptr();
	finder_ = svc->get(DSPi.finder_str);
	run_ = svc->get(DSPi.run_str);

}


void
Module::action(htab_ptr to)
{
	// try and make a default 4-tuple list target array based on array properties
	str_rc module;
	str_rc namespace;
	str_rc object;
	str_rc classname;
	str_rc method;

	htab_rc args;

	htab_rc target4;
	//  Origianal PHP code is pedanticly followed, is ancient hacky code
	//  once related to 'FAT FREE' framework routing array

	if (to.is_list() && (to.size() >= 4))
	{
		
		namespace = str_ptr::empty_str();

		module = to.get((int) 0);
		object = to.get((int) 1); // full classname
		classname = object;
		method = to.get((int) 2);	
		args = to.get((int)  3);
	}
	else {
		// try for RouteSet::XXX_S constants, defined in route_data

		str_rc objx, metx;

		module = to.get(route_data.MOD_S);
		object = to.get(route_data.OBJ_S);
		method = to.get(route_data.FUN_S);
		objx = to.get(route_data.OBJX_S);
		metx = to.get(route_data.FUNX_S);

		args = to.get(route_data.ARG_S);

		namespace = to.get(route_data.NSP_S);

		str_buf buf;


		if (namespace.size())
		{
			buf << namespace << "\\";
		}

		buf << object << objx;

		classname = buf.zstr();

		buf << method << metx;

		method = buf.zstr();
	}

	if (!module.size())
	{
		module = DSPi.default_str;
	}
	if (!args.size())
	{
		args = htab_ptr::empty_array();
	}

	// make a common quad-tuple list
	htab_rw target(target4);

	//:TODO may be check for exception right here!
	target.push_back(namespace);
	target.push_back(classname);
	target.push_back(method);
	target.push_back(args);

	setModule(module);

	obj_rc route_match = getRouteMatch();

	RouteMatch* rm = zobj_toc<RouteMatch>(route_match);
	rm->setCallInfo(classname, method, args);
	rm->setModuleName(module);

	val_rc content = obcall(rm);

	respond(content);

}

htab_rc   
Module::module_viewpaths(obj_ptr module)
{
	fn_call afn(DSPi.getviewpaths_fn, module);
	fn_result ret(afn);
	return ret.array();
}


void 
Module::call_module_activate(obj_ptr module)
{
	fn_call afn(DSPi.activate_fn, module);
	fn_params<1>    afn_result(afn);
	ZVAL_OBJ(&afn_result.params[0], finder_);
	afn_result.call_fn(); // void result
}


// assumes not already loaded
obj_rc
Module::addModule(str_ptr name, val_ptr modspec)
{
	obj_rc result;
	Module* modo = nullptr;

	str_rc  dir;

	val_rc  mcfg = modspec;
	

	if (mcfg.isString())
	{

		dir = mcfg.zstr();
		str_buf path;

		path << dir << "/module.php";

		mcfg = Loader::cpp_global()->require(path.zstr());
	}
	else {
		dir = run_ptr()->property(DSPi.config_dir);
	}

	if (mcfg.isArray())
	{
		result = createModule(name, mcfg);

		modo = zobj_toc<Module>(result);

		modo->setConfigPath(dir);

		htab_rw mlist(modules_);
		mlist.set(name, result);

		call_module_activate(result);
	}

	if (modo)
	{
		htab_rc reqlist = call__module_requires(result);

		htab_walk wk;

		auto value = wk.value();
		for(wk.start(reqlist); wk.ok(); wk.next())
		{
			if (value.isString())
			{
				setModule(value.zstr());
			}
		}
	}

	return result;
}


bool
Module::clearRouteCache(str_ptr file, str_ptr cache_name)
{
	str_rc ext = file_extension(file);
	str_rc path;

	if (ext.size()==0)
	{
		str_buf buf;
		buf << file << ".php";
		path = buf.zstr();
	}
	else {
		path = file;
	}
	obj_rc cache_obj;

	cache_obj = getCache(cache_name);

	if (cache_obj.ok())
	{
		ICache* ic = zobj_toc<ICache>(cache_obj);
		ic->deleteKey(path);
	}

	return file_exists(path);
}


error_return
Module::dispatch(obj_ptr rmatch)
{
	route_match_ = rmatch;

	error_return result;

	RouteMatch* rm = rm_ptr();

	if (!rm->prepare_call())
	{
		result.error() << "RouteMatch prepare_call failed";
		return result;
	}

	str_rc mod_name = rm->getModuleName();
	if (!mod_name.size())
	{
		result.error() << "Need a module name";
		return result;
	}

	obj_rc module = setModule(mod_name);

	htab_rc paths = module_viewpaths(module);

	if (zs_cmp_ci(mod_name,DSPi.default_str)!=0)
	{
		obj_rc defmod = getModule(DSPi.default_str);
		htab_rc defpaths = module_viewpaths(defmod);
		paths.merge(defpaths);
	}

	Services* svc = svc_ptr();

	obj_rc engine = svc->get(DSPi.engine_str);
	if (paths.size())
	{	
		obj_rc search = engine.call(DSPi.getfinder_fn);
		search.call(DSPi.addpaths_fn, paths);
	}

	htab_rc view_data = svc->get(DSPi.viewdata_str);
	if (view_data.size())
	{
		engine.call(DSPi.sharewithall_fn, view_data);
	}

	val_rc content = obcall(route_match_);

	svc->unset(DSPi.engine_str);

	respond(content);
}


void
Module::dispatchError(obj_ptr rmatch, str_ptr err_class, str_ptr method, htab_ptr errArgs)
{
	route_match_ = rmatch;
	val_rc content = obcallEx(err_class, method, errArgs);
	respond(content);
}


void
Module::forward( val_ptr fto)
{
	if (fto.isArray())
	{
		action(fto);
	}
	else if(fto.isString())
	{
		obj_rc response = svc_ptr()->get(DSPi.response_str);
		response.call(DSPi.redirect_str, fto);
	}
}


obj_rc
Module::getActiveModule()
{
	return active_;
}


htab_rc
Module::getArgs()
{
	return rm_ptr()->getObjArgs();
}


obj_rc
Module::getCache(str_ptr cache_name)
{
	obj_rc result;

	obj_rc cfg = svc_ptr()->get(DSPi.config_str);
	Config* cf = zobj_toc<Config>(cfg);

	val_rc rstr = cf->getOrNot(DSPi.cache_routes_str, true);
	if (rstr.isTrue())
	{
		result = svc_ptr()->get(cache_name);
	}
	return result;
}


obj_rc
Module::getDefaultModule()
{
	obj_rc mdef = modules_.get()
}


str_rc
Module::getMethodName()
{
	
}


obj_rc
Module::getModule(str_ptr name)
{
	
}


str_rc
Module::getModuleName()
{
	
}


htab_rc
Module::loadRoutes(str_ptr file, str_ptr cache_name)
{
	
}


htab_rc
Module::getRoles()
{
	
}


obj_rc
Module::getRoute()
{
	
}


obj_rc
Module::getRouteMatch()
{
	if (!route_match_.ok())
	{
		route_match_ = svc_ptr()->get(route_match_str);
	}
	return route_match_;
}


str_rc
Module::getUri()
{
	
}


htab_rc
Module::loadRoutes(str_ptr file, str_ptr cache_name)
{
	
}

val_rc
Module::obcall(obj_ptr rmatch)
{
	
}

val_rc 
Module::obcallEx(str_ptr obclass, str_ptr obmethod, htab_ptr obargs)
{
	
}

htab_rc 
Module::parseRaw(htab_ptr input)
{
	
}

void 
Module::respond(val_ptr content)
{
	
}


void 
Module::setLog(bool val)
{
	
}


obj_rc 
Module::setModule(str_ptr name)
{
	
}



}//end wcc


using namespace wcc;
using namespace zpp;

#endif