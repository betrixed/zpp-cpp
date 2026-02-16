#ifndef ROUTE_MATCH_CPP
#define ROUTE_MATCH_CPP

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif


extern "C" {
#include "ext/standard/url.h"
#include <Zend/zend_closures.h>
};


#ifndef ROUTE_MATCH_H
#include "route_match.h"
#endif


#ifndef WCC_ROUTE_H
#include "route.h"
#endif

#ifndef ROUTE_SET_H
#include "route_set.h"
#endif

#ifndef WCC_TARGET_H
#include "target.h"
#endif


#ifndef REQUEST_GLOBALS_H
#include "request_globals.h"
#endif

#ifndef REFLECT_CACHE_H
#include "reflect_cache.h"
#endif

#ifndef ROUTE_MATCH_ARGINFO
#define ROUTE_MATCH_ARGINFO
extern "C" {
	#include "stub/routematch_arginfo.h"	
};
#endif

#include <string.h>


#define INFO_HLEN 12

namespace wcc 
{
	base_obj_mgr<RouteMatch> RouteMatch::omg;

class RouteMatch_init : public state_init {
public:
	str_intern cc_route_obj;
	str_intern cc_match_args;
	str_intern cc_roles;
	str_intern cc_ob_args;
	str_intern cc_errors;

	str_intern cc_uri;
	str_intern cc_module_name;
	str_intern cc_ob_class;
	str_intern cc_ob_method;

	str_intern cc_verb_flag;
	str_intern cc_ajax_flag;
	str_intern cc_route_id;
	str_intern request_obj;

	void init() override
	{
		cc_route_obj = "route";
		cc_match_args = "match_args";
		cc_roles = "roles";
		cc_ob_args = "ob_args";
		cc_errors = "errors";

		cc_uri = "uri";
		cc_module_name = "module_name";
		cc_ob_class = "ob_class";
		cc_ob_method = "ob_method";

		cc_verb_flag = "verb";
		cc_ajax_flag = "ajax";
		cc_route_id = "route_id";
		request_obj = "request";
	}
};

RouteMatch_init RM_data;



void
RouteMatch::debug_info(htab_rw hw)
{
	hw.set(RM_data.cc_route_obj, route_);
	hw.set(RM_data.cc_match_args, match_args_);
	hw.set(RM_data.cc_roles, roles_);
	hw.set(RM_data.cc_ob_args, ob_args_);
	hw.set(RM_data.cc_errors, errors_);
	hw.set(RM_data.cc_uri, uri_);
	hw.set(RM_data.cc_module_name, module_name_);
	hw.set(RM_data.cc_ob_class, ob_class_);
	hw.set(RM_data.cc_ob_method, ob_method_);
	hw.set(RM_data.cc_verb_flag, (int)verb_flag_);
	hw.set(RM_data.cc_ajax_flag, (int)ajax_flag_);
}

obj_ptr   
RouteMatch::testRoute(obj_ptr ro)
{
	//showobj("testRoute ro", ro);
	obj_ptr result;

	if (!Route::omg.myType(ro)) 
	{
		str_ptr classname(zend_std_get_class_name(ro));
		zend_throw_error(zend_ce_exception, "testRoute Error: Not route object %s\n",  classname.data());
		return result;
	}

	Route* robj =  zobj_toc<Route>(ro);

	if ( ((verb_flag_ & robj->verbs_) != 0) && ((ajax_flag_ & robj->ajax_) != 0) ) 
	{
		//zend_printf("matched\n");
		result = std::move(ro);
	}
	//zend_printf("discarded v %ld a %ld\n", robj->verbs_, robj->ajax_);
	return result;
}

obj_ptr
RouteMatch::firstMatch(val_ptr wrap)
{
	obj_ptr result;

	if (wrap.isObject())
	{
		result = testRoute(wrap.zobject());
	}
	else if (wrap.isArray())
	{
		htab_walk list;
		auto obj = list.value();
		for(list.start(wrap.zarray()); list.ok(); list.next()) 
		{
			result = testRoute(obj.zobject());
			if (result.ok()) {
				return result;
			}
		}
	}
	return result;
}
/** what was this doing ?
#define PRE_EXP	"/\\G"
#define POST_EXP "/u"
*/


bool 
RouteMatch::find_route(RouteSet* routeset)
{
	val_ptr	match;

	obj_ptr 	robj;

	val_rc	mreturn;

	htab_ptr 	list(routeset->fixed_);

	route_.init();
	if (list.try_fetch(uri_, match)) 
	{
		//showmem("fetched", match);

		robj = firstMatch(match);

		if (robj.ok()) 
		{
			route_ = robj;
			match_args_.reset();
			return true;
		}
	}

	htab_walk walk;

	auto key = walk.key();
	auto rzval = walk.value();

	for( walk.start(routeset->vary_); walk.ok(); walk.next())
	{
		str_ptr pattern(key);
		preg rexpmatch(pattern);

		//showmem("key", key);

		if (rexpmatch.matches(uri_) > 0)
		{
			robj = firstMatch(rzval);
			if (robj.ok()) {
				route_ = robj;

				match_args_ = rexpmatch.results();
				//match_args_.dec_ref();
				//showmem("match_args_", match_args_);
				return true;
			}
			else {
				zend_printf("Route pattern match but flags failed: %s\n", pattern.data());
			}
		}
	}
	return false;

}


val_rc //static
RouteMatch::call_method(obj_ptr obj, str_ptr method, htab_ptr args)
{
	if (args.size())
	{
		return obj.call(method, args);
	}
	else {
		return obj.call(method);
	}
}

val_rc 
RouteMatch::call(htab_ptr extra, obj_ptr before, obj_ptr after)
{
	val_rc  result;
	val_rc  zobj;
	obj_ptr obj;

	if (!ob_class_.size() || !ob_method_.size()) 
	{

		this->prepare_call();
	}

	if (ob_class_.size())
	{
		// only works for objects with zero arguments constructor
		zobj = ReflectCache::staticInstance(ob_class_);
	}
	else
	{
		zobj = target_;
	}
	

	if (extra.size())
	{
		htab_ptr hr(ob_args_);

		if (hr.size())
		{
			htab_rw(ob_args_).merge(extra);
		}
		else {
			ob_args_ = extra;
		}
	}

	val_ptr test(zobj);
	if (!test.isObject())
	{
		zend_throw_exception(zend_ce_error, "RouteMatch::Invoke with no object",0);
		return result;
	}
	obj = test.zobject();
	if (obj.instanceof(zend_ce_closure))
	{
		// doesn't actually have methods.
		if (!call_spread_fn(result, zobj, ob_args_))
		{
			return false;
		}
		return result;
	}
	else {
		str_ptr method_name;

		if (before.ok())
		{
			Pair* ppair = zobj_toc<Pair>(before);

			val_ptr first(ppair->first());
			val_ptr second(ppair->second());

			//zend_printf("Before ");
			//showmem(" second", second);

			method_name = first.zstr();
			
			if (method_name.size() && obj.method_exists(method_name))
			{
				result = this->call_method(obj, method_name, second);
				test = result;
				if (test.isFalse()  || test.isObject())
				{
					return result;
				}
			}
		}

		if (obj.method_exists(ob_method_))
		{
			// store this here in route_match object,
			// in case an after hook needs and wants to change final returned value
			result = this->call_method(obj, ob_method_, ob_args_);
			result_ = result;
		}
		
		if (after.ok())
		{
			Pair* ppair = zobj_toc<Pair>(after);

			val_ptr first(ppair->first());
			val_ptr second(ppair->second());

			method_name = second.zstr();
			if (method_name.size() && obj.method_exists(method_name))
			{
				result = this->call_method(obj, method_name, second);
				test = result;
				if (!test.isNull())
				{
					result_ = result;
				}
			}	
		}	
	}

	return result;
}

str_rc
call_url_decode(str_ptr in_str)
{
	str_rc result;

	zend_string  *out_str;
	// new string buffer
	out_str = zend_string_init(in_str.data(),in_str.size(), 0);
	// Relies on decoded string being shorter or equal length.
	ZSTR_LEN(out_str) = php_url_decode(ZSTR_VAL(out_str), ZSTR_LEN(out_str));
	result.adopt(out_str);
	return result;
}

void RouteMatch::set_tuple12( htab_ptr tg)
{
	val_ptr mobj(tg.get(route_data.OBJ_S));

	//get_ht_cc(target, OBJ_S);

	if (mobj.isString())
	{
		ob_class_ = mobj.zstr();
		mobj = tg.get(route_data.FUN_S);
		if (mobj.isString()) {
			ob_method_ = mobj.zstr();
		}
	}
	else {
		mobj = tg.get(zend_long(0));
		if (mobj.isString()) {
			ob_class_ = mobj.zstr();
		}
		mobj = tg.get(1);
		if (mobj.isString()) {
			ob_method_ = mobj.zstr();
		}
	}
}



void RouteMatch::set_tuple14(htab_ptr tg)
{
	val_ptr mobj;

	if (!module_name_.size() ==0 ) {
		mobj = tg.get(zend_long(0));
		if (mobj.isString()) {
			module_name_ = mobj.zstr();
		}
	}
	mobj = tg.get(1);
	if (mobj.isString()) 
	{
		ob_class_ = mobj.zstr();
	}
	mobj = tg.get(2);
 	if (mobj.isString()) 
	{
		ob_method_ = mobj.zstr();
	}
	mobj = tg.get(3);
	if (mobj.isArray()) {
		
		htab_rw targs(mobj);
		htab_ptr  obargs(ob_args_);

		if (obargs.size() > 0) 
		{
			targs.merge(obargs);
		}
		ob_args_ = targs;
	}
}


void RouteMatch::error_context(Route* r)
{
	htab_rw errors_ht(errors_);

	errors_ht.push_back((zend_string*)uri_);
	errors_ht.push_back((zend_string*)r->compiled_);
	str_rc margs = htab_ptr(match_args_).print_kv("margs");

	errors_ht.push_back(margs);

	str_rc params = htab_ptr(r->params_).print_kv("params");
	errors_ht.push_back(params);
}


htab_rc RouteMatch::fetchArgs()
{
	htab_rc result;

	Route* route = zobj_toc<Route>(route_);

	htab_ptr params_ht(route->params_);
	htab_ptr margs_ht(match_args_);

	

	val_ptr test;
	
	str_rc error_msg;

	
	auto ct = params_ht.size();

	if (ct > 0)
	{
		ct += 1;

		// params are not empty
		long mct = margs_ht.size();
		if (mct != ct) {
			error_msg.adopt(strpprintf(0, "Route matches count should be %u", ct));
			htab_rw(errors_).push_back(error_msg);
			error_context(route);
		}

		htab_walk walk;

		auto name = walk.key();
		auto val = walk.value();
		
		for( walk.start(params_ht); walk.ok(); walk.next())
		{	
			if (!margs_ht.try_fetch(val,test))
			{
				str_rc svalue(val.to_zstr());
				error_msg.adopt(strpprintf(0, "Null value arg# %s", svalue.data()));
				htab_rw(errors_).push_back(error_msg);
				error_context(route);
			}
			else 
			{
				htab_rw args(result);
				if (test.isString()) {
					str_rc decoded = call_url_decode(test);
					args.set(name,decoded);
				}
				else {
					args.set(name,test);
				}
			}
		}
	}
	else {
		htab_ptr tg(route->target_);

		if ( tg.ok() && tg.try_fetch(route_data.ARG_S, test) && test.isArray())
		{
			result = test.zarray();
		}
	}
	return result;
}

bool RouteMatch::prepare_call()
{
	val_ptr test;

	errors_.init();
	//zend_printf("prepare_call 1\n");

	Route* route = zobj_toc<Route>(route_);

	val_ptr route_target(route->target_);

	obj_rc tg_obj(route_target.zobject());
	//zend_printf("prepare_call 2\n");
	if (tg_obj.ok())
	{
		this->target_ = tg_obj;

		if( tg_obj.instanceof(Target::omg.classEntry())) 
		{	
			//zend_printf("prepare_call 3\n");
			Target* cobj = zobj_toc<Target>(tg_obj);
			module_name_ = cobj->getModule();
		 	ob_class_ = cobj->getClass();
		 	ob_method_ = cobj->getFunc();
		 	ob_args_ = this->fetchArgs();

		 	htab_ptr defaults = cobj->getParams();

		 	if (defaults.size())
		 	{
		 		//showdata("defaults", defaults);
		 		
		 		obj_rc request = Services::service(RM_data.request_obj);
		 		RequestGlobals* rg = zobj_toc<RequestGlobals>(request);
		 		obj_ptr  qry = rg->query();

		 		Hmap*    hmap = zobj_toc<Hmap>(qry);

		 		for_key_value   wk;
		 		for(wk.start(defaults); wk.ok(); wk.next())
		 		{
		 			str_rc key = wk.key();
		 			if (!hmap->has(key)) {
		 				hmap->set(key, wk.value());
		 			}
		 		}
		 	}
		 	return true;
		}
		else if (route_target.isCallable())
		{
			//zend_printf("prepare_call 4\n");
			return true;
		}
	}

	htab_ptr tg(route_target.zarray());

	if (!tg)
	{
		return false;
	}

	test = tg.get(route_data.ROLE_S);
	if (test.isArray())
	{
		roles_ = test.zarray();
	}
	else {
		roles_.init();
	}

	// first clean up cobj->errors

	ob_args_.init();

	ob_class_.init();
	ob_method_.init();

	zend_string* temp;
	if (tg.try_fetch(route_data.MOD_S, test) && test.getStringData(&temp))
	{
		module_name_ = temp;
	}
	else {
		module_name_.init();
	}

	ob_args_ = this->fetchArgs();

	size_t ct = tg.size();
	if ( (ct >= 3) && (tg.has_index(2))) 
	{
		set_tuple14(tg);
	}
	else {
		set_tuple12(tg);
	}
	
	size_t clen = ob_class_.size();
	size_t mlen = ob_method_.size();

	if (!clen || !mlen)
	{
		htab_rw etab(errors_);
		str_rc error_msg;

		if (!clen)
		{
			error_msg.adopt(strpprintf(0, "Missing class name"));
			etab.push_back(error_msg);
		}
		if (!mlen) 
		{
			error_msg.adopt(strpprintf(0, "Missing object method"));
			etab.push_back(error_msg);
		}

	}
	return (errors_.isEmpty());
}


void RouteMatch::construct(str_ptr uri, int verbs, int ajax)
{
	uri_ = uri;
	verb_flag_ = verbs;
	ajax_flag_ = ajax;
}

void 
RouteMatch::setCallInfo(str_ptr obclass, str_ptr obmethod, val_ptr args)
{
	ob_class_ = obclass;
	ob_method_ = obmethod;
	ob_args_ = args.zarray();
}

}// wcc namespace

using namespace wcc;

PHP_METHOD(Wcc_RouteMatch, __construct)
{
	zend_string* uri;
	zend_long    verbs;
	zend_long    ajax;

	RouteMatch* cobj = zval_toc<RouteMatch>(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_START(3, 3)
	Z_PARAM_STR(uri)
	Z_PARAM_LONG(verbs)
	Z_PARAM_LONG(ajax)
	ZEND_PARSE_PARAMETERS_END();

	cobj->construct(uri, verbs, ajax);

	//zend_printf("new %d %d\n", sizeof(zval), (char*)&cobj->match_args - (char*)&cobj->route);

}

PHP_METHOD(Wcc_RouteMatch, setCallInfo)
{
	zend_string* obclass;
	zend_string* obmethod;
	zval*        args;

	RouteMatch* cobj = zval_toc<RouteMatch>(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_START(3, 3)
	Z_PARAM_STR(obclass)
	Z_PARAM_STR(obmethod)
	Z_PARAM_ARRAY(args)
	ZEND_PARSE_PARAMETERS_END();

	cobj->setCallInfo(obclass, obmethod, args);
}

PHP_METHOD(Wcc_RouteMatch, getVerbName)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RouteMatch* cobj = zval_toc<RouteMatch>(ZEND_THIS);
	
	str_rc result = Route::getVerb(cobj->verb_flag_);

	result.move_zv(return_value);
}

PHP_METHOD(Wcc_RouteMatch, getErrors)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RouteMatch* cobj = zval_toc<RouteMatch>(ZEND_THIS);

	htab_ptr errors = cobj->getErrors();
	errors.copy_zv(return_value);
}

PHP_METHOD(Wcc_RouteMatch, getUri)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RouteMatch* cobj = zval_toc<RouteMatch>(ZEND_THIS);
	str_ptr ret = cobj->getUri();

	ret.copy_zv(return_value);
}

PHP_METHOD(Wcc_RouteMatch, getVerb)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RouteMatch* cobj = zval_toc<RouteMatch>(ZEND_THIS);
	RETURN_LONG(cobj->verb_flag_);
}

PHP_METHOD(Wcc_RouteMatch, getAjax)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RouteMatch* cobj = zval_toc<RouteMatch>(ZEND_THIS);
	RETURN_LONG(cobj->ajax_flag_);
}

PHP_METHOD(Wcc_RouteMatch, getRoles)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RouteMatch* cobj = zval_toc<RouteMatch>(ZEND_THIS);

	htab_ptr roles = cobj->getRoles();
	roles.copy_zv(return_value);
}

PHP_METHOD(Wcc_RouteMatch, getModuleName)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RouteMatch* cobj = zval_toc<RouteMatch>(ZEND_THIS);
	str_ptr name = cobj->getModuleName();

	name.copy_zv(return_value);
}

PHP_METHOD(Wcc_RouteMatch, getObjClass)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RouteMatch* cobj = zval_toc<RouteMatch>(ZEND_THIS);
	str_ptr name = cobj->getObjClass();
	name.copy_zv(return_value);
}

PHP_METHOD(Wcc_RouteMatch, getObjMethod)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RouteMatch* cobj = zval_toc<RouteMatch>(ZEND_THIS);
	str_ptr name = cobj->getObjMethod();
	name.copy_zv(return_value);
}

PHP_METHOD(Wcc_RouteMatch, getObjArgs)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RouteMatch* cobj = zval_toc<RouteMatch>(ZEND_THIS);
	htab_ptr args = cobj->getObjArgs();
	args.copy_zv(return_value);
}




PHP_METHOD(Wcc_RouteMatch, prepare_call)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RouteMatch* cobj = zval_toc<RouteMatch>(ZEND_THIS);

	bool found = cobj->prepare_call();
	ZVAL_BOOL(return_value,found);
	return;
}


PHP_METHOD(Wcc_RouteMatch, setModuleName)
{
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	RouteMatch* cobj = zval_toc<RouteMatch>(ZEND_THIS);
	cobj->setModuleName(name);
	
}

PHP_METHOD(Wcc_RouteMatch, getMatch)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RouteMatch* cobj = zval_toc<RouteMatch>(ZEND_THIS);
	obj_ptr match = cobj->getMatch();

	return match.copy_zv(return_value);
}

PHP_METHOD(Wcc_RouteMatch, findRoute)
{

	zval*         route_set;

	zend_class_entry* rs_entry = RouteSet::omg.classEntry();

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_OBJECT_OF_CLASS(route_set, rs_entry);
	ZEND_PARSE_PARAMETERS_END();

	RouteMatch* prm = zval_toc<RouteMatch>(ZEND_THIS);
	RouteSet*	   rset = zval_toc<RouteSet>(route_set);

	bool found = prm->find_route(rset);

	ZVAL_BOOL(return_value,found);
	return;

}

ZEND_METHOD(Wcc_RouteMatch, call)
{
	zval* extra_args = nullptr;
	zval* before_pair = nullptr;
	zval* after_pair = nullptr;

	zend_class_entry* pair_ce = Pair::omg.classEntry();

	ZEND_PARSE_PARAMETERS_START(0, 3)
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY_OR_NULL(extra_args)
	Z_PARAM_OBJECT_OF_CLASS_OR_NULL(before_pair, pair_ce)
	Z_PARAM_OBJECT_OF_CLASS_OR_NULL(after_pair, pair_ce)
	ZEND_PARSE_PARAMETERS_END();

	RouteMatch* prm = zval_toc<RouteMatch>(ZEND_THIS);
	val_rc result = prm->call(extra_args, before_pair, after_pair);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RouteMatch, callMethod)
{
	zval* obj;
	zend_string* method;
	zval* args = nullptr;

	ZEND_PARSE_PARAMETERS_START(2,3)
	Z_PARAM_OBJECT(obj)
	Z_PARAM_STR(method)
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY_OR_NULL(args)
	ZEND_PARSE_PARAMETERS_END();

	RouteMatch* prm = zval_toc<RouteMatch>(ZEND_THIS);
	val_rc result = prm->call_method(obj,method,args);
	result.move_zv(return_value);

}


PHP_MINIT_FUNCTION(route_match_d)
{
	//WcR_ce = wcc_class_reg("Wc\\Route", class_Wcc_Route_methods);
	auto ce = register_class_Wcc_RouteMatch();

	RouteMatch::omg.classEntry(ce);

	STATE_INIT_ADD(RM_data)
	
	return SUCCESS;
}

#endif