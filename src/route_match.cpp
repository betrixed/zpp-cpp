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

#ifndef REFLECT_CACHE_H
#include "reflect_cache.h"
#endif

#include <string.h>


#define INFO_HLEN 12

namespace wcc 
{
	base_obj_mgr<RouteMatch> RouteMatch::omg;

};

using namespace wcc;

class RouteMatch_init : public state_init {
public:
	RouteMatch_init() : state_init() {}

	zstr_intern cc_route_obj;
	zstr_intern cc_match_args;
	zstr_intern cc_roles;
	zstr_intern cc_ob_args;
	zstr_intern cc_errors;

	zstr_intern cc_uri;
	zstr_intern cc_module_name;
	zstr_intern cc_ob_class;
	zstr_intern cc_ob_method;

	zstr_intern cc_verb_flag;
	zstr_intern cc_ajax_flag;
	zstr_intern cc_route_id;

	virtual void init()
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
	}
};

RouteMatch_init RM_data;



void
RouteMatch::debug_info(htab_write hw)
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

zobj_user   
RouteMatch::testRoute(zobj_user ro)
{
	//showobj("testRoute ro", ro);
	zobj_user result;

	if (!Route::omg.myType(ro)) 
	{
		zstr_user classname(zend_std_get_class_name(ro));
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

zobj_user
RouteMatch::firstMatch(zval_user wrap)
{
	zobj_user result;

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
	zval_user	match;

	zobj_user 	robj;

	zval_mgr	mreturn;

	htab_read 	list(routeset->fixed_);

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
		preg rexpmatch(key.zstr());

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
		}
	}
	return false;

}


zval_mgr //static
RouteMatch::call_method(zobj_user obj, zstr_user method, htab_read args)
{
	if (args.size())
	{
		return obj.call(method, args);
	}
	else {
		return obj.call(method);
	}
}

zval_mgr 
RouteMatch::call(htab_read extra, zobj_user before, zobj_user after)
{
	zval_mgr  result;
	zval_mgr  zobj;
	zobj_user obj;

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
		htab_read hr(ob_args_);

		if (hr.size())
		{
			htab_write(ob_args_).merge(extra);
		}
		else {
			ob_args_ = extra;
		}
	}

	zval_user test(zobj);
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
		zstr_user method_name;

		if (before.ok())
		{
			Pair* ppair = zobj_toc<Pair>(before);

			zval_user first(ppair->first());
			zval_user second(ppair->second());

			//zend_printf("Before ");
			//showmem(" second", second);

			method_name = first.zstr();
			
			if (method_name.size() && obj.method_exists(method_name))
			{
				result = this->call_method(obj, method_name, second);
				test = result;
				if (test.isFalse())
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

			zval_user first(ppair->first());
			zval_user second(ppair->second());

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

zstr_mgr
call_url_decode(zstr_user in_str)
{
	zstr_mgr result;

	zend_string  *out_str;
	// new string buffer
	out_str = zend_string_init(in_str.data(),in_str.size(), 0);
	// Relies on decoded string being shorter or equal length.
	ZSTR_LEN(out_str) = php_url_decode(ZSTR_VAL(out_str), ZSTR_LEN(out_str));
	result.adopt(out_str);
	return result;
}

void RouteMatch::set_tuple12( htab_read tg)
{
	zval_user mobj(tg.get(route_data.OBJ_S));

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



void RouteMatch::set_tuple14(htab_read tg)
{
	zval_user mobj;
	zend_ulong ct;

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
		
		htab_write targs(mobj);
		htab_read  obargs(ob_args_);

		if (obargs.size() > 0) 
		{
			targs.merge(obargs);
		}
		ob_args_ = targs;
	}
}


void RouteMatch::error_context(Route* r)
{
	htab_write errors_ht(errors_);

	errors_ht.push_back((zend_string*)uri_);
	errors_ht.push_back((zend_string*)r->compiled_);
	zstr_mgr margs = htab_read(match_args_).print_kv("margs");

	errors_ht.push_back(margs);

	zstr_mgr params = htab_read(r->params_).print_kv("params");
	errors_ht.push_back(params);
}


htab_mgr RouteMatch::fetchArgs()
{
	htab_mgr result;

	Route* route = zobj_toc<Route>(route_);

	htab_read params_ht(route->params_);
	htab_read margs_ht(match_args_);

	

	zval_user test;
	
	zstr_mgr error_msg;

	
	auto ct = params_ht.size();

	if (ct > 0)
	{
		ct += 1;

		// params are not empty
		long mct = margs_ht.size();
		if (mct != ct) {
			error_msg.adopt(strpprintf(0, "Route matches count should be %ld", ct));
			htab_write(errors_).push_back(error_msg);
			error_context(route);
		}

		htab_walk walk;

		auto name = walk.key();
		auto val = walk.value();
		
		for( walk.start(params_ht); walk.ok(); walk.next())
		{	
			if (!margs_ht.try_fetch(val,test))
			{
				zstr_mgr svalue(val.to_zstr());
				error_msg.adopt(strpprintf(0, "Null value arg# %s", svalue.data()));
				htab_write(errors_).push_back(error_msg);
				error_context(route);
			}
			else 
			{
				htab_write args(result);
				if (test.isString()) {
					zstr_mgr decoded = call_url_decode(test);
					args.set(name,decoded);
				}
				else {
					args.set(name,test);
				}
			}
		}
	}
	else {
		htab_read tg(route->target_);

		if ( tg.ok() && tg.try_fetch(route_data.ARG_S, test) && test.isArray())
		{
			result = test.zarray();
		}
	}
	return result;
}

bool RouteMatch::prepare_call()
{
	zval_user test;

	errors_.init();
	//zend_printf("prepare_call 1\n");

	Route* route = zobj_toc<Route>(route_);

	zval_user route_target(route->target_);

	zobj_user tg_obj(route_target.zobject());
	//zend_printf("prepare_call 2\n");
	if (!tg_obj.isNull())
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
		 	return true;
		}
		else if (route_target.isCallable())
		{
			//zend_printf("prepare_call 4\n");
			return true;
		}
	}

	htab_read tg(route_target.zarray());

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
		htab_write etab(errors_);
		zstr_mgr error_msg;

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


void RouteMatch::construct(zstr_user uri, int verbs, int ajax)
{
	uri_ = uri;
	verb_flag_ = verbs;
	ajax_flag_ = ajax;
}

void 
RouteMatch::setCallInfo(zstr_user obclass, zstr_user obmethod, zval_user args)
{
	ob_class_ = obclass;
	ob_method_ = obmethod;
	ob_args_ = args.zarray();
}

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
	

	route_verbflag_name(cobj->verb_flag_, return_value);
}

PHP_METHOD(Wcc_RouteMatch, getErrors)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RouteMatch* cobj = zval_toc<RouteMatch>(ZEND_THIS);

	htab_read errors = cobj->getErrors();
	errors.return_zv(return_value);
}

PHP_METHOD(Wcc_RouteMatch, getUri)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RouteMatch* cobj = zval_toc<RouteMatch>(ZEND_THIS);
	zstr_user ret = cobj->getUri();

	ret.return_zv(return_value);
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

	htab_read roles = cobj->getRoles();
	roles.return_zv(return_value);
}

PHP_METHOD(Wcc_RouteMatch, getModuleName)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RouteMatch* cobj = zval_toc<RouteMatch>(ZEND_THIS);
	zstr_user name = cobj->getModuleName();

	name.return_zv(return_value);
}

PHP_METHOD(Wcc_RouteMatch, getObjClass)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RouteMatch* cobj = zval_toc<RouteMatch>(ZEND_THIS);
	zstr_user name = cobj->getObjClass();
	name.return_zv(return_value);
}

PHP_METHOD(Wcc_RouteMatch, getObjMethod)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RouteMatch* cobj = zval_toc<RouteMatch>(ZEND_THIS);
	zstr_user name = cobj->getObjMethod();
	name.return_zv(return_value);
}

PHP_METHOD(Wcc_RouteMatch, getObjArgs)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RouteMatch* cobj = zval_toc<RouteMatch>(ZEND_THIS);
	htab_read args = cobj->getObjArgs();
	args.return_zv(return_value);
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
	zobj_user match = cobj->getMatch();

	return match.return_zv(return_value);
}

PHP_METHOD(Wcc_RouteMatch, findRoute)
{
	zend_string  *uri;
	zend_long    verb;
	zend_long    ajax;
	HashTable	*result;

	zval*         route_set;

	zval         routeval;
	zend_string  *regexp;

	pcre_cache_entry  *pce;

	zval         mreturn;

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
	zval* extra_args;
	zval* before_pair;
	zval* after_pair;

	zend_class_entry* pair_ce = Pair::omg.classEntry();

	ZEND_PARSE_PARAMETERS_START(0, 3)
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY_OR_NULL(extra_args)
	Z_PARAM_OBJECT_OF_CLASS_OR_NULL(before_pair, pair_ce)
	Z_PARAM_OBJECT_OF_CLASS_OR_NULL(after_pair, pair_ce)
	ZEND_PARSE_PARAMETERS_END();

	RouteMatch* prm = zval_toc<RouteMatch>(ZEND_THIS);
	zval_mgr result = prm->call(extra_args, before_pair, after_pair);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RouteMatch, callMethod)
{
	zval* obj;
	zend_string* method;
	zval* args;

	ZEND_PARSE_PARAMETERS_START(2,3)
	Z_PARAM_OBJECT(obj)
	Z_PARAM_STR(method)
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY_OR_NULL(args)
	ZEND_PARSE_PARAMETERS_END();

	RouteMatch* prm = zval_toc<RouteMatch>(ZEND_THIS);
	zval_mgr result = prm->call_method(obj,method,args);
	result.move_zv(return_value);

}


PHP_MINIT_FUNCTION(route_match_d)
{
	//WcR_ce = wcc_class_reg("Wc\\Route", class_Wcc_Route_methods);
	auto ce = register_class_Wcc_RouteMatch();

	RouteMatch::omg.classEntry(ce);

	return SUCCESS;
}

#endif