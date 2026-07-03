#ifndef WCC_DISPATCH_CPP
#define WCC_DISPATCH_CPP

#ifndef DISPATCH_ARGINFO_H
extern "C" {
	#include "stub/dispatch_arginfo.h"
}
#endif

#ifndef DISPATCH_WCC_H
#include "dispatch.h"
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

#ifndef WCC_LOADER_H
#include "loader.h"
#endif

#ifndef ROUTE_MATCH_H
#include "route_match.h"
#endif

#ifndef WCC_ROUTESET_H
#include "route_set.h"
#endif

#ifndef WCC_CONFIG_H
#include "config.h"
#endif

#ifndef GLOBAL_RESPONSE_H
#include "response.h"
#endif

#ifndef MODULE_WCC_H
#include "module.h"
#endif

namespace wcc {

using namespace zpp;

base_obj_mgr<Dispatch> Dispatch::omg;

Disp_init DSPi;

void 
Disp_init::init()
{
	finder_str = "finder";
	default_str = "default";
	route_match_str = "route_match";

	config_str = "config";
	config_dir = "config_dir";
	activate_fn = "activate";
	cache_routes_str = "cache_routes";

	getassets_fn = "getassets";
	getviewpaths_fn = "getviewpaths";
	engine_str = "engine";
	getfinder_fn = "getfinder";
	addpaths_fn = "addpaths";

	viewdata_str = "view_data";
	sharewithall_fn = "sharewithall";
	redirect_str = "redirect";
	response_str = "response";

	classname_str = "classname";
	php_extn = ".php";
	route_parser = "route_parser";
	parseraw_str = "parseraw";

	beforecall_str = "beforecall";
	set_str = "set";
	modcfg_str = "modcfg";
	services_str = "services";

	roles_str = "roles";
	active_str = "active";
	modules_str = "modules";
}


void Dispatch::debug_info(htab_rw hw)
{

	hw.set(DSPi.route_match_str, route_match_);
	hw.set(DSPi.finder_str, finder_);
	hw.set(DSPi.roles_str, roles_);
	hw.set(DSPi.active_str, active_);
	hw.set(DSPi.services_str, services_);
	hw.set(DSPi.config_str, config_);
	hw.set(DSPi.modules_str, modules_);
	hw.set(DSPi.modcfg_str, modcfg_);
}

Services* 
Dispatch::svc_ptr()
{
	return zobj_toc<Services>(services_);
}

Config*
Dispatch::config_ptr()
{
	return zobj_toc<Config>(config_);
}

RouteMatch*
Dispatch::rm_ptr()
{
	return zobj_toc<RouteMatch>(route_match_);
}

void
Dispatch::construct()
{
	services_ = Services::instance();

	Services* svc = svc_ptr();
	val_return ftest = svc->get(DSPi.finder_str);
	if (!ftest.throw_errors())
	{
		finder_ = std::move(ftest.value_);
	}
	ftest = svc->get(DSPi.config_str);
	if (!ftest.throw_errors())
	{
		config_ = std::move(ftest.value_);
	}
}


void
Dispatch::action(htab_ptr to)
{
	// try and make a default 4-tuple list target array based on array properties
	str_rc module;
	str_rc nspace;
	str_rc object;
	str_rc classname;
	str_rc method;

	htab_rc args;

	htab_rc target4;
	//  Origianal PHP code is pedanticly followed, is ancient hacky code
	//  once related to 'FAT FREE' framework routing array

	if (to.is_list() && (to.size() >= 4))
	{
		
		nspace = str_ptr::empty_str();

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

		nspace = to.get(route_data.NSP_S);

		str_buf buf;


		if (nspace.size())
		{
			buf << nspace << "\\";
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
	target.push_back(nspace);
	target.push_back(classname);
	target.push_back(method);
	target.push_back(args);

	setModule(module);

	obj_rc route_match = getRouteMatch();

	RouteMatch* rm = zobj_toc<RouteMatch>(route_match);
	val_rc temp(args);

	rm->setCallInfo(classname, method, temp);
	rm->setModuleName(module);

	val_return content = obcall(rm);
	if (!content.throw_errors(__FUNCTION__))
	{
		respond(content.value_);
	}
	else {
		//TODO: Catch and return respond with error messages
	}
}

htab_rc   
Dispatch::module_viewpaths(obj_ptr module)
{
	fn_call afn(DSPi.getviewpaths_fn, module);
	fn_noparams ret(afn);
	return ret.array();
}


void 
Dispatch::call_module_activate(obj_ptr module)
{
	fn_call afn(DSPi.activate_fn, module);
	fn_params<1>    afn_result(afn);
	ZVAL_OBJ(&afn_result.params[0], finder_);
	afn_result.call_fn(); // void result
}

obj_rc
Dispatch::createModule(str_ptr name, htab_ptr mcfg)
{
	obj_rc result;
	str_rc cname = mcfg.get(DSPi.classname_str);
	if (!cname.ok())
	{
		cname = Module::omg.class_name();
	}
	htab_rc args;
	htab_rw margs(args);

	margs.push_back(name);
	margs.push_back(mcfg);

	result = ReflectCache::staticInstanceArgs(cname, margs);

	return result;
}

// assumes not already loaded
obj_return
Dispatch::addModule(str_ptr name, val_ptr modspec)
{
	obj_return result;

	obj_rc modo;
	Module* m = nullptr;

	str_rc  dir;

	val_rc  mcfg = modspec;
	

	if (mcfg.isString())
	{

		dir = mcfg.zstr();
		str_buf path;

		path << dir << "/module.php";

		val_return test = Loader::cpp_global()->readPHP(path.zstr());

		if (test.has_errors())
		{
			result = test.move_error();
			return result;
		}


		mcfg = test.value_;
	}
	else {
		dir = config_.str_property(DSPi.config_dir);
	}

	if (mcfg.isArray())
	{
		htab_ptr data = mcfg.zarray();
		modo = createModule(name, data);
		

		m = zobj_toc<Module>(modo);
		m->setConfigPath(dir);

		htab_rw mlist(modules_);
		mlist.set(name, modo);

		call_module_activate(modo);
	}

	if (m)
	{
		result.value_ = modo;
		htab_rc reqlist = m->getRequires();

		htab_walk wk;

		auto value = wk.value();
		for(wk.start(reqlist); wk.ok(); wk.next())
		{
			if (value.isString())
			{
				obj_return test = setModule(value.zstr());
				if (test.has_errors())
				{
					 result.error() << test.get_errors();
				}
			}
		}
	}
	else {
		result.error() << "Failed to create module " << name;
	}

	return result;
}


bool
Dispatch::clearRouteCache(str_ptr file, str_ptr cache_name)
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

	cache_obj = getRoutesCache(cache_name);

	if (cache_obj.ok())
	{
		ICache* ic = zobj_toc<ICache>(cache_obj);
		ic->deleteKey(path);
	}

	return !file_exists(path);
}


error_return
Dispatch::dispatch(obj_ptr rmatch)
{
	route_match_ = rmatch;

	error_return result;
	val_rc       temparg;

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
	

	obj_return module_err = setModule(mod_name);

	if (module_err.has_errors())
	{
		result = module_err.move_error();
		return result;
	}

	Services* svc = svc_ptr();

	obj_rc engine;

	val_return etest = svc->get(DSPi.engine_str);
	if (etest.has_errors())
	{
		result = etest.move_error();
		return result;
	}
	else {
		engine = etest.value_.zobject();
	}

	if (engine.ok())
	{
		htab_rc paths = module_viewpaths(active_);

		if (zs_cmp_ci(mod_name,DSPi.default_str)!=0)
		{
			obj_rc defmod = getModule(DSPi.default_str);
			htab_rc defpaths = module_viewpaths(defmod);
			htab_rw allpaths(paths);
			allpaths.merge(defpaths);
		}

		if (paths.size())
		{	
			val_rc search = engine.call(DSPi.getfinder_fn);
			if (search.isObject())
			{
				obj_ptr sop = search.zobject();
				val_rc temparg(paths);
				sop.call(DSPi.addpaths_fn, temparg);
			}
		}
		val_rc view_data;
		val_return vtest = svc->get(DSPi.viewdata_str);
		if (vtest.has_errors())
		{
			result = vtest.move_error();
			return result;
		}
		else {
			view_data = vtest.value_;
		}
		if (view_data.isArray())
		{
			engine.call(DSPi.sharewithall_fn, view_data);
		}
	}
	val_return content = obcall(route_match_);

	if (!content.has_errors())
	{
		result = respond(content.value_);
	}
	else {
		result = content.move_error();
	}
	
	return result;
}


void
Dispatch::dispatchError(obj_ptr rmatch, str_ptr err_class, str_ptr method, htab_ptr errArgs)
{
	route_match_ = rmatch;
	val_return content = obcallEx(err_class, method, errArgs);
	if (!content.has_errors())
	{
		respond(content.value_);
	}
	else {
		//GIVE UP!
	}
}


void
Dispatch::forward( val_ptr fto)
{
	if (fto.isArray())
	{
		action(fto.zarray());
	}
	else if(fto.isString())
	{
		obj_rc response;

		val_return test = svc_ptr()->get(DSPi.response_str);

		if (test.throw_errors())
		{
			return;
		}
		response = test.value_.zobject();
		Response* robj = zobj_toc<Response>(response);

		robj->redirect(fto.zstr());
	}
}


obj_rc
Dispatch::getActiveModule()
{
	return active_;
}


htab_ptr
Dispatch::getArgs()
{
	return rm_ptr()->getObjArgs();
}


obj_rc
Dispatch::getRoutesCache(str_ptr cache_name)
{
	obj_rc result;

	Config* cf = config_ptr();

	val_rc temparg;

	temparg.set_bool(true);
	val_rc rstr = cf->getOrNot(DSPi.cache_routes_str, temparg);
	if (rstr.isTrue())
	{
		val_return rtest = svc_ptr()->get(cache_name);
		if (!rtest.throw_errors())
		{
			result = std::move(rtest.value_); 
		}
	}
	return result;
}


obj_return
Dispatch::getDefaultModule()
{
	obj_return result;

	obj_rc mdef = modules_.get(MODi.DEFAULT_MOD);

	if (mdef.ok())
	{
	    result.value_ = mdef;
		return result;
	}
	val_rc defaults = modcfg_[MODi.DEFAULT_MOD];

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
Dispatch::getMethodName()
{
	str_rc result;

	if (route_match_.ok())
	{
		RouteMatch* rm = zobj_toc<RouteMatch>(route_match_);
		result = rm->getObjMethod();
	}
	return result;
}


obj_rc
Dispatch::getModule(str_ptr name)
{
	obj_rc result = modules_.get(name);
	return result;
}


str_rc
Dispatch::getModuleName()
{
	str_rc result;

	if (active_.ok())
	{
		Module* modo = zobj_toc<Module>(active_);
		result = modo->getName();
	}
	return result;
}


htab_return
Dispatch::loadRoutes(str_ptr file, str_ptr cache_name)
{
	htab_return result;

	str_rc rfile;
	str_rc extn = file_extension(file);
	if (!extn.size())
	{
		rfile = file + DSPi.php_extn;
	}
	else {
		rfile = file;
	}

	if (!file_exists(rfile))
	{
		result.error() << "Routes file " << rfile << " not found";
		return result;
	}

	obj_rc cache_obj = getRoutesCache(cache_name);

	long modified_time = filemtime(rfile);

	val_rc rdata;

	if (cache_obj.ok())
	{
		ICache* cobj = zobj_toc<ICache>(cache_obj);

		obj_rc cdata = cobj->getCached(rfile);

		if (cdata.ok())
		{
			ICacheData* data = zobj_toc<ICacheData>(cdata);
			if (data->getStored() < modified_time) {
				cobj->deleteKey(rfile);
			}
			else {
				rdata = data->getData();
			}
		}
	}
	if (!rdata.isNull())
	{
		loadedFile_ = rfile;
		result.value_ = rdata;
		return result;
	}

	val_return raw = Loader::readPHP(rfile);

	if (raw.has_errors())
	{
		result = raw.move_error();
		return result;
	}
	val_ptr test(raw.value_);
	//showmem("readPHP returned", test);

	if (test.isObject())
	{
		obj_rc rawobj = test.zobject();
		if (!rawobj.instanceof(RouteSet::omg.classEntry()))
		{
			result.error() << rawobj.className() << " not expected from " << rfile;
			return result;
		}

		htab_rw wr(rdata);
		wr.set(DSPi.set_str, rawobj);
		result.value_ = rdata;
		//showarray("return rdata array", result.value_);
	}
	else if (test.isArray())
	{
		result = parseRaw(test.zarray());
	}
	if (!result.has_errors())
	{
		loadedFile_ = rfile;
	}
	return result;
}


htab_rc
Dispatch::getRoles()
{
	return roles_;
}


obj_rc
Dispatch::getRoute()
{
	obj_rc result;

	obj_rc rmobj = getRouteMatch();
	if (rmobj.ok())
	{
		RouteMatch* rm = zobj_toc<RouteMatch>(rmobj);
		result = rm->getMatch();
	}
	return result;
}


obj_rc
Dispatch::getRouteMatch()
{
	obj_rc result;

	if (!route_match_.ok())
	{
		val_return rtest = svc_ptr()->get(DSPi.route_match_str);
		if (!rtest.throw_errors())
		{
			route_match_ = std::move(rtest.value_);
		}
	}
	return route_match_;
}


str_rc
Dispatch::getUri()
{
	str_rc result;

	obj_rc rmobj = getRouteMatch();
	if (rmobj.ok())
	{
		RouteMatch* rm = zobj_toc<RouteMatch>(rmobj);
		result = rm->getUri();
	}
	return result;
}

val_return
Dispatch::obcall(obj_ptr rmatch)
{
	val_return   result;

	RouteMatch* rm = zobj_toc<RouteMatch>(rmatch);
	obj_rc robj = rm->getMatch();
	val_rc target;


	if (robj.ok())
	{
		Route* r = zobj_toc<Route>(robj);
		target = r->getTarget();
	}
	else {
		result.error() << "RouteMatch has no route object";
		return result;
	}
	
	if (target.isArray())
	{
		htab_ptr tac(target);
		roles_ = tac.get(route_data.ROLE_S);
		if (roles_.isNull())
		{
			roles_ = htab_ptr::empty_array();
		}
	}

	htab_rc disval;
	htab_rw arg(disval);

	arg.push_back(self_);

	obj_rc pbefore = Pair::omg.new_zobj();
	Pair* p = zobj_toc<Pair>(pbefore);

	val_rc arg1(DSPi.beforecall_str);
	val_rc arg2(disval);

	p->construct(arg1,arg2);


	htab_rc extra_args;
	obj_rc  after_obj;

	result = rm->call(extra_args, pbefore, after_obj);
	return result;

}

val_return 
Dispatch::obcallEx(str_ptr obclass, str_ptr obmethod, htab_ptr obargs)
{
	RouteMatch* rm = zobj_toc<RouteMatch>(route_match_);
	val_rc temparg(obargs);
	rm->setCallInfo(obclass, obmethod, temparg);
	return obcall(rm);
}

htab_return 
Dispatch::parseRaw(htab_ptr input)
{
	htab_return result;

	obj_rc parser;

	val_return ptest = Services::service(DSPi.route_parser);
	if (ptest.has_errors())
	{
		ptest.error() << " No route parser object";
		result = ptest.move_error();
		return result;
	}
	parser = std::move(ptest.value_);
	if (!parser.ok())
	{
		result.error() << "Service " << DSPi.route_parser << " not found";
		return result;
	}
	val_rc temparg(input);
	result.value_ = parser.call(DSPi.parseraw_str, temparg);
	return result;
}

error_return 
Dispatch::respond(val_ptr content)
{
	error_return result;
	obj_rc rpobj;
	val_return rtest = Services::service(DSPi.response_str);
	if (rtest.has_errors())
	{
		result = rtest.move_error();
		return result;
	}
	rpobj = std::move(rtest.value_);
	if (!rpobj.ok())
	{
		result.error() = "No response service";
		return result;
	}

	Response* rp = zobj_toc<Response>(rpobj);
	if (rp->isSent())
	{
		return result;
	}

	if (content.isNull())
	{
		if (rp->hasContent() || rp->getStatusCode())
		{
			showobj("rp", rpobj);
			rp->send();
			return result;
		}
		result.error() << "No response status";
		return result; 
	}

	if (content.isObject())
	{
		obj_rc ct_obj = content.zobject();
		if (ct_obj.instanceof(Response::omg.classEntry()))
		{
			Response* cobj = zobj_toc<Response>(ct_obj);
			cobj->send();
			return result;
		}
	}

	if (content.isString())
	{
		str_ptr s = content.zstr();
		if (s.size())
		{
			rp->setContentType(RSPD.text_html,RSPD.utf8);
			rp->setContent(s);
			rp->send();
		}
	}
	return result;

}


void 
Dispatch::setLog(bool val)
{
	doLog_ = val;
	if (val)
	{
		//logging service assigned here?
	}
}


obj_return 
Dispatch::setModule(str_ptr name)
{

	obj_return result;
	obj_rc modo = modules_.get(name);

	if (modo.ok())
	{
		active_ = modo;
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

	val_rc mdata = modcfg_.get(name);

	if (mdata.isString() && (zs_cmp(name, MODi.DEFAULT_MOD)!=0))
	{
		 result = addModule(name, mdata);
		 return result;
	}
	if (!mdata.isArray())
	{
		active_ = mdef;
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

	active_ = modo;
	return result;
}

void
Dispatch::setModuleCfg(htab_ptr cfg)
{
	modcfg_ = cfg;
}

}//end wcc


using namespace wcc;
using namespace zpp;


ZEND_METHOD(Wcc_Dispatch, __construct)
{
	ZEND_PARSE_PARAMETERS_NONE();
	Dispatch* cobj = zval_toc<Dispatch>(ZEND_THIS);
	cobj->construct();
}

ZEND_METHOD(Wcc_Dispatch, action)
{
	zarg_rd args(execute_data);
	htab_ptr data = args.htab(args.need(0));
	if (!args.throw_errors(__FUNCTION__))
	{
		Dispatch* cobj = zval_toc<Dispatch>(ZEND_THIS);
		cobj->action(data);
	}
}

ZEND_METHOD(Wcc_Dispatch, addModule)
{
	zarg_rd args(execute_data);
	str_ptr name = args.str(args.need(0));
	val_ptr mspec = args.string_or_array(args.need(1));

	if (!args.throw_errors(__FUNCTION__))
	{
		Dispatch* cobj = zval_toc<Dispatch>(ZEND_THIS);
		obj_return result = cobj->addModule(name, mspec);
		if (!result.throw_errors(__FUNCTION__))
		{
			result.value_.move_zv(return_value);
		}
	}
}

ZEND_METHOD(Wcc_Dispatch, clearRouteCache)
{
	zarg_rd args(execute_data);
	str_ptr file = args.str(args.need(0));
	str_ptr cache_name = args.str(args.need(1));
	if (!args.throw_errors(__FUNCTION__))
	{
		Dispatch* cobj = zval_toc<Dispatch>(ZEND_THIS);
		bool result = cobj->clearRouteCache(file,cache_name);
		RETURN_BOOL(result);
	}
}

ZEND_METHOD(Wcc_Dispatch, dispatch)
{
	zarg_rd args(execute_data);
	obj_ptr rm = args.obj_class(args.need(0), RouteMatch::omg.classEntry());
	if (!args.throw_errors(__FUNCTION__))
	{
		Dispatch* cobj = zval_toc<Dispatch>(ZEND_THIS);
		error_return result = cobj->dispatch(rm);
		result.throw_errors(__FUNCTION__);
	}	
}

ZEND_METHOD(Wcc_Dispatch, dispatchError)
{
	zarg_rd args(execute_data);
	htab_ptr data;
	obj_ptr rm = args.obj_class(args.need(0), RouteMatch::omg.classEntry());
	str_ptr cname = args.str(args.need(1));
	str_ptr method = args.str(args.need(2));
	
	args.zarray_null(data, args.option(3));

	if (!args.throw_errors(__FUNCTION__))
	{
		Dispatch* cobj = zval_toc<Dispatch>(ZEND_THIS);
		cobj->dispatchError(rm, cname, method, data);
	}
}
ZEND_METHOD(Wcc_Dispatch, forward)
{
	zarg_rd args(execute_data);
	val_ptr data = args.string_or_array(args.need(0));
	if (!args.throw_errors(__FUNCTION__))
	{
		Dispatch* cobj = zval_toc<Dispatch>(ZEND_THIS);
		cobj->forward(data);
	}
}

ZEND_METHOD(Wcc_Dispatch, getActiveModule)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Dispatch* cobj = zval_toc<Dispatch>(ZEND_THIS);
	obj_rc result = cobj->getActiveModule();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Dispatch, getArgs)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Dispatch* cobj = zval_toc<Dispatch>(ZEND_THIS);
	htab_ptr result = cobj->getArgs();
	result.copy_zv(return_value);
}

ZEND_METHOD(Wcc_Dispatch, getRoutesCache)
{
	zarg_rd args(execute_data);
	str_ptr name = args.str(args.need(0));
	if (!args.throw_errors(__FUNCTION__))
	{
		Dispatch* cobj = zval_toc<Dispatch>(ZEND_THIS);
		obj_rc result = cobj->getRoutesCache(name);
		result.move_zv(return_value);
	}
}

ZEND_METHOD(Wcc_Dispatch, getDefaultModule)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Dispatch* cobj = zval_toc<Dispatch>(ZEND_THIS);
	obj_return result = cobj->getDefaultModule();
	if (!result.throw_errors(__FUNCTION__))
	{
		result.value_.move_zv(return_value);
	}
}

ZEND_METHOD(Wcc_Dispatch, getMethodName)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Dispatch* cobj = zval_toc<Dispatch>(ZEND_THIS);
	str_rc result = cobj->getMethodName();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Dispatch, getModule)
{
	zarg_rd args(execute_data);
	str_ptr name = args.str(args.need(0));
	if (!args.throw_errors(__FUNCTION__))
	{
		Dispatch* cobj = zval_toc<Dispatch>(ZEND_THIS);
		obj_rc result = cobj->getModule(name);
		result.move_zv(return_value);
	}
}

ZEND_METHOD(Wcc_Dispatch, getModuleName)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Dispatch* cobj = zval_toc<Dispatch>(ZEND_THIS);
	str_rc result = cobj->getModuleName();
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_Dispatch, loadRoutes)
{
	zarg_rd args(execute_data);
	str_ptr file = args.str(args.need(0));
	str_ptr cache_name = args.str(args.need(1));
	if (!args.throw_errors(__FUNCTION__))
	{
		Dispatch* cobj = zval_toc<Dispatch>(ZEND_THIS);
		htab_return result = cobj->loadRoutes(file, cache_name);
		if (!result.throw_errors(__FUNCTION__))
		{
			result.value_.move_zv(return_value);
		}
	}
}

ZEND_METHOD(Wcc_Dispatch, getRoles)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Dispatch* cobj = zval_toc<Dispatch>(ZEND_THIS);
	htab_rc result = cobj->getRoles();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Dispatch, getRoute)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Dispatch* cobj = zval_toc<Dispatch>(ZEND_THIS);
	obj_rc result = cobj->getRoute();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Dispatch, getRouteMatch)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Dispatch* cobj = zval_toc<Dispatch>(ZEND_THIS);
	obj_rc result = cobj->getRouteMatch();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Dispatch, getUri)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Dispatch* cobj = zval_toc<Dispatch>(ZEND_THIS);
	str_rc result = cobj->getUri();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Dispatch, obcall)
{
	zarg_rd args(execute_data);
	obj_ptr rm = args.obj_class(args.need(0), RouteMatch::omg.classEntry());
	if (!args.throw_errors(__FUNCTION__))
	{
		Dispatch* cobj = zval_toc<Dispatch>(ZEND_THIS);
		val_return result = cobj->obcall(rm);
		if (!result.throw_errors(__FUNCTION__))
		{
			result.value_.move_zv(return_value);
		}
	}
}

ZEND_METHOD(Wcc_Dispatch, obcallEx)
{
	zarg_rd args(execute_data);
	
	str_ptr obclass = args.str(args.need(0));
	str_ptr obmethod = args.str(args.need(1));
	htab_ptr obargs = args.htab(args.option(2));

	if (!args.throw_errors(__FUNCTION__))
	{
		Dispatch* cobj = zval_toc<Dispatch>(ZEND_THIS);
		val_return result = cobj->obcallEx(obclass, obmethod, obargs);
		if (!result.throw_errors(__FUNCTION__))
		{
			result.value_.move_zv(return_value);
		}
	}
}

ZEND_METHOD(Wcc_Dispatch, parseRaw)
{
	zarg_rd args(execute_data);
	htab_ptr input = args.htab(args.need(0));

	if (!args.throw_errors(__FUNCTION__))
	{
		Dispatch* cobj = zval_toc<Dispatch>(ZEND_THIS);
		htab_return result = cobj->parseRaw(input);
		if (!result.throw_errors(__FUNCTION__))
		{
			result.value_.move_zv(return_value);
		}
	}
}

ZEND_METHOD(Wcc_Dispatch, respond)
{
	zarg_rd args(execute_data);
	val_ptr input = args.need(0);

	if (!args.throw_errors(__FUNCTION__))
	{
		Dispatch* cobj = zval_toc<Dispatch>(ZEND_THIS);
		error_return result = cobj->respond(input);
		result.throw_errors(__FUNCTION__);
	}
}

ZEND_METHOD(Wcc_Dispatch, setLog)
{
	zarg_rd args(execute_data);
	bool    value;

	args.zbool(value, args.need(0));

	if (!args.throw_errors(__FUNCTION__))
	{
		Dispatch* cobj = zval_toc<Dispatch>(ZEND_THIS);
		cobj->setLog(value);
	}
}

ZEND_METHOD(Wcc_Dispatch, setModule)
{
	zarg_rd args(execute_data);
	str_ptr    value  = args.need(0);

	if (!args.throw_errors(__FUNCTION__))
	{
		Dispatch* cobj = zval_toc<Dispatch>(ZEND_THIS);
		obj_return result = cobj->setModule(value);
		result.value_.move_zv(return_value);
	}
}

ZEND_METHOD(Wcc_Dispatch, setModuleCfg)
{
	zarg_rd args(execute_data);
	htab_ptr    value  = args.htab(args.need(0));

	if (!args.throw_errors(__FUNCTION__))
	{
		Dispatch* cobj = zval_toc<Dispatch>(ZEND_THIS);
		cobj->setModuleCfg(value);
	}
}

PHP_MINIT_FUNCTION(Wcc_Dispatch_reg)
{
	auto ce = register_class_Wcc_Dispatch();
	Dispatch::omg.classEntry(ce);

	STATE_INIT_ADD(DSPi)

	return SUCCESS;
}

#endif