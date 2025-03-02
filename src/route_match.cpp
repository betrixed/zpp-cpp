#ifndef ROUTE_MATCH_CPP
#define ROUTE_MATCH_CPP

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif


extern "C" {
#include "ext/standard/url.h"
};



#include "route_match.h"

#ifndef WCC_ROUTE_H
#include "route.h"
#endif

#include "route_set.h"

#include <string.h>


#define INFO_HLEN 12

namespace wcc 
{
	base_obj_mgr<RouteMatch> RouteMatch::omg;

};

using namespace wcc;



static const char* const cc_route_obj = "route";
static const char* const cc_match_args = "match_args";
static const char* const cc_roles = "roles";
static const char* const cc_ob_args = "ob_args";
static const char* const cc_errors = "errors";

static const char* const cc_uri = "uri";
static const char* const cc_module_name = "module_name";
static const char* const cc_ob_class = "ob_class";
static const char* const cc_ob_method = "ob_method";

static const char* const cc_verb_flag = "verb";
static const char* const cc_ajax_flag = "ajax";
static const char* const cc_route_id = "route_id";


void
RouteMatch::debug_info(HashTable *ht)
{
	htab_ptr  hw(ht);

	hw.set(cc_route_obj, route_);
	hw.set(cc_match_args, match_args_);
	hw.set(cc_roles, roles_);
	hw.set(cc_ob_args, ob_args_);
	hw.set(cc_errors, errors_);
	hw.set(cc_uri, uri_);
	hw.set(cc_module_name, module_name_);
	hw.set(cc_ob_class, ob_class_);
	hw.set(cc_ob_method, ob_method_);
	hw.set(cc_verb_flag, verb_flag_);
	hw.set(cc_ajax_flag, ajax_flag_);
}

Route*    
RouteMatch::testRoute(zend_object* zobj)
{
	if (!(zobj  && route_mgr .myType(zobj))) {
		if (zobj) {
			zend_string* p = zend_std_get_class_name(zobj);
			zstr_ptr classname(p);
			zend_throw_error(zend_ce_exception, "testRoute Error: Not route object %s\n",  classname.cstr());
		}
		else {
			zend_throw_error(zend_ce_exception, "testRoute Error: Null object\n");
		}
		return nullptr;
	}

	Route* robj =  zobj_toc<Route>(zobj);

	if ( ((verb_flag_ & robj->verbs_) != 0) && ((ajax_flag_ & robj->ajax_) != 0) ) 
	{
		return robj;
	}

	return nullptr;
}

Route*  
RouteMatch::firstMatch(zval_own& wrap)
{
	Route* robj = nullptr;

	if (wrap.isObject())
	{
		return testRoute(wrap.zobject());
	}
	else if (wrap.isArray())
	{

		htab_walk list;
		auto& obj = list.value();
		for(list.start(wrap.zarray()); list.ok(); list.next()) 
		{
			robj = testRoute(obj.zobject());
			if (robj) {
				return robj;
			}
		}
	}
	return nullptr;
}
/** what was this doing ?
#define PRE_EXP	"/\\G"
#define POST_EXP "/u"
*/


bool 
RouteMatch::find_route(RouteSet* routeset)
{
	zval_own		  match;
	Route*        robj;

	zval_own         mreturn;

	htab_ptr list(routeset->fixed_);

	if (list.try_fetch(uri_, match)) 
	{
		robj = firstMatch(match);

		if (robj) 
		{
			route_.setobj(robj->zobj());
			match_args_.reset();
			return true;
		}
	}

	htab_walk walk;

	auto& key = walk.key();
	auto& rzval = walk.value();

	for( walk.start(routeset->vary_) ; walk.ok(); walk.next())
	{
		preg rexpmatch(key.zstr());

		//showmem("key", key);

		if (rexpmatch.matches(uri_) > 0)
		{
			robj = firstMatch(rzval);
			if (robj) {
				route_.setobj(robj->zobj());
				match_args_ = rexpmatch.results();
				//match_args_.dec_ref();
				//showmem("match_args_", match_args_);
				return true;
			}
		}
	}
	return false;

}

/** replace array values in ato, with afrom */ 
void gazump_array(zval* ato, zval *afrom)
{
	zend_ulong   hidx;
 

	htab_ptr hto(Z_ARR_P(ato));
	htab_ptr hfrom(Z_ARR_P(afrom));

	htab_walk walk;
	auto& key = walk.key();
	auto& val = walk.value();
	
	for(walk.start(hfrom) ; walk.ok(); walk.next())
	{
		if (key.isString()) 
		{
			hto.set(key.zstr(), val);
		}
		else {
			hto.set(key.zlong(), val);
		}
	}
}

zval_own //static
RouteMatch::call_method(zobj_ptr obj, zstr_ptr method, htab_ptr args)
{
	if (args.size())
	{
		return obj.call(method, args);
	}
	else {
		return obj.call(method);
	}
}

zval_own 
RouteMatch::invoke(htab_ptr extra, zobj_ptr before, zobj_ptr after)
{
	zval_own result;
	zval_own zobj;
	zobj_own obj;


	if (this->prepare_call())
	{
		if (ob_class_.size())
		{
			// only works for objects with zero arguments constructor
			zobj = Wcc_ReflectCache::staticInstance(ob_class_);
		}
		else
		{
			zobj = target_;
		}
		

		if (extra.size())
		{
			if (ob_args_.size())
			{
				ob_args_.merge(extra);
			}
			else {
				ob_args_ = extra;
			}
		}

		obj = zobj.zobject();
		if (obj.instanceof(zend_ce_closure))
		{
			// doesn't actually have methods.
			if (!call_spread_fn(result, zobj, ob_args_))
			{
				return false;
			}
			return result;
		}
		else if (obj.isObject()) {
			zstr_ptr method_name;

			if (before.isObject())
			{
				Pair* ppair = zobj_toc<Pair>(before);

				zval_ptr first(ppair->first());
				zval_ptr second(ppair->second());

				//zend_printf("Before ");
				//showmem(" second", second);

				method_name = first.zstr();
				
				if (method_name.size() && obj.method_exists(method_name))
				{

					htab_ptr args(second);

					result = this->call_method(obj, method_name, args);
					if (result.isFalse())
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
			
			if (after.isObject())
			{
				Pair* ppair = zobj_toc<Pair>(after);

				zval_ptr first(ppair->first());
				zval_ptr second(ppair->second());

				method_name = second.zstr();
				if (method_name.size() && obj.method_exists(method_name))
				{
					htab_ptr args(second);
					result = this->call_method(obj, method_name, args);
					if (!result.isNull())
					{
						result_ = result;
					}
				}	
			}	
		}
	}
	return result;
}

/** temporary string value argument to be replaced */
zend_string* call_url_decode(zend_string *in_str)
{
	zend_string  *out_str;

	out_str = zend_string_init(ZSTR_VAL(in_str), ZSTR_LEN(in_str), 0);
	ZSTR_LEN(out_str) = php_url_decode(ZSTR_VAL(out_str), ZSTR_LEN(out_str));

	return out_str;
}

void RouteMatch::set_tuple12( htab_ptr tg)
{
	zval_ptr mobj(tg.get(OBJ_S));

	//get_ht_cc(target, OBJ_S);

	if (mobj.isString())
	{
		ob_class_ = mobj;
		mobj = tg.get(FUN_S);
		if (mobj.isString()) {
			ob_method_ = mobj;
		}
	}
	else {
		mobj = tg.get(zend_long(0));
		if (mobj.isString()) {
			ob_class_ = mobj;
		}
		mobj = tg.get(1);
		if (mobj.isString()) {
			ob_method_ = mobj;
		}
	}
}



void RouteMatch::set_tuple14(htab_ptr tg)
{
	zval_ptr mobj;
	zend_ulong ct;

	if (!module_name_.size() ==0 ) {
		mobj = tg.get(zend_long(0));
		if (mobj.isString()) {
			module_name_ = mobj;
		}
	}
	mobj = tg.get(1);
	if (mobj.isString()) 
	{
		ob_class_ = mobj;
	}
	mobj = tg.get(2);
 	if (mobj.isString()) 
	{
		ob_method_ = mobj;
	}
	mobj = tg.get(3);
	if (mobj.isArray()) {
		
		htab_own targs(mobj);

		if (ob_args_.size() > 0) 
		{
			targs.merge(ob_args_);
		}
		ob_args_ = targs;
	}
}


void RouteMatch::error_context(Route* r)
{
	htab_ptr errors_ht(errors_);

	errors_ht.push_back(uri_);
	errors_ht.push_back(r->compiled_);
	errors_ht.push_back(htab_ptr(match_args_).print_all("margs"));
	errors_ht.push_back(htab_ptr(r->params_).print_all("params"));
}

htab_own RouteMatch::fetchArgs()
{
	Route* route = zobj_toc<Route>(route_.ptr());

	htab_ptr params_ht(route->params_);
	htab_ptr margs_ht(match_args_);


	zval_own test;
	htab_own args_ht;
	
	auto ct = params_ht.size();

	if (ct > 0)
	{
		ct += 1;

		// params are not empty
		long mct = margs_ht.size();
		if (mct != ct) {
			zstr_ptr msg(strpprintf(0, "Route matches count should be %ld", ct));
			errors_.push_back(msg);
			error_context(route);
		}

		htab_walk walk;

		zval_own& name = walk.key();
		zval_own& val = walk.value();
		
		for( walk.start(params_ht); walk.ok(); walk.next())
		{	

			if (!margs_ht.try_fetch(val,test))
			{
				zstr_ptr svalue(val.to_zstr());
				zstr_ptr msg(strpprintf(0, "Null value arg# %s", svalue.data()));
				errors_.push_back(msg);
				error_context(route);
			}
			else 
			{
				if (test.isString()) {
					zend_string* decoded = call_url_decode(test.zstr());
					//showstr("call_url_decode",decoded);
					test.set(decoded);
				}
				args_ht.set(name,test);
			}
		}
	}
	else {
		htab_ptr tg(route->target_.zarray());

		if (!tg.isNull() && tg.try_fetch(ARG_S, test) && test.isArray())
		{
			args_ht = test;
		}
	}
	return args_ht;
}

bool RouteMatch::prepare_call()
{
	zval_own test;

	errors_.clear();

	Route* route = zobj_toc<Route>(route_.ptr());

	zval_ptr route_target(route->target_);

	zobj_ptr tg_obj(route_target.zobject());

	if (!tg_obj.isNull())
	{
		this->target_ = tg_obj;

		if( tg_obj.instanceof(Target::omg.classEntry())) 
		{
			Target* cobj = zobj_toc<Target>(tg_obj);
			module_name_ = cobj->getModule();
		 	ob_class_ = cobj->getClass();
		 	ob_method_ = cobj->getFunc();
		 	ob_args_ = this->fetchArgs();
		 	return true;
		}
		else if (route_target.isCallable())
		{
			return true;
		}
	}

	htab_ptr tg(route_target.zarray());

	if (tg.isNull())
	{
		return false;
	}

	if (tg.try_fetch(ROLE_S, test) && test.isArray())
	{
		roles_ = test.zarray();
	}
	else {
		roles_.clear();
	}

	// first clean up cobj->errors

	ob_args_.clear();

	ob_class_.lose();
	ob_method_.lose();

	zend_string* temp;
	if (tg.try_fetch(MOD_S, test) && test.getStringData(&temp))
	{
		module_name_ = temp;
	}
	else {
		module_name_.clear();
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

	if (ob_class_.size()==0) 
	{
		zstr_ptr msg(strpprintf(0, "Missing class name"));
		errors_.push_back(msg);
	}
	if (ob_method_.size()==0) 
	{
		zstr_ptr msg(strpprintf(0, "Missing object method"));
		errors_.push_back(msg);
	}
	return (errors_.size() == 0);
}





PHP_METHOD(Wcc_RouteMatch, __construct)
{
	zend_string* uri;

	RouteMatch* cobj = zval_toc<RouteMatch>(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_START(3, 3)
	Z_PARAM_STR(uri)
	Z_PARAM_LONG(cobj->verb_flag_)
	Z_PARAM_LONG(cobj->ajax_flag_)
	ZEND_PARSE_PARAMETERS_END();

	cobj->uri_.set(uri);

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

	cobj->ob_class_ = obclass;
	cobj->ob_method_ = obmethod;
	cobj->ob_args_ = zval_ptr(args);

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
	cobj->errors_.return_zv(return_value);
}

PHP_METHOD(Wcc_RouteMatch, getUri)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RouteMatch* cobj = zval_toc<RouteMatch>(ZEND_THIS);
	cobj->uri_.return_zv(return_value);
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
	cobj->roles_.return_zv(return_value);
}

PHP_METHOD(Wcc_RouteMatch, getModuleName)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RouteMatch* cobj = zval_toc<RouteMatch>(ZEND_THIS);
	cobj->module_name_.return_zv(return_value);
}

PHP_METHOD(Wcc_RouteMatch, getObjClass)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RouteMatch* cobj = zval_toc<RouteMatch>(ZEND_THIS);
	cobj->ob_class_.return_zv(return_value);
}

PHP_METHOD(Wcc_RouteMatch, getObjMethod)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RouteMatch* cobj = zval_toc<RouteMatch>(ZEND_THIS);
	cobj->ob_method_.return_zv(return_value);
}

PHP_METHOD(Wcc_RouteMatch, getObjArgs)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RouteMatch* cobj = zval_toc<RouteMatch>(ZEND_THIS);
	cobj->ob_args_.return_zv(return_value);
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
	const zobj_own& match = cobj->getMatch();

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

ZEND_METHOD(Wcc_RouteMatch, __invoke)
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
	zval_own result = prm->invoke(extra_args,before_pair,after_pair);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_RouteMatch, call_method)
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
	zval_own result = prm->call_method(obj,method,args);
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