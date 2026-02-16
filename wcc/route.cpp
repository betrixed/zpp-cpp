#ifndef WCC_ROUTE_CPP
#define WCC_ROUTE_CPP

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "route.h"

#include "zend_smart_str.h"

#ifndef STATE_INIT_H
#include "state_init.h"
#endif

#ifndef WCC_ROUTE_ARGINFO
#define WCC_ROUTE_ARGINFO
extern "C" {
	#include "stub/route_arginfo.h"	
};
#endif

namespace wcc {
 	
Route::RouteMgr Route::omg;

Route_init route_data;

/* This helps for a clean valgrind report */
void freehtmemory(HashTable* ht)
{
	bool persistent = GC_FLAGS(ht) & IS_ARRAY_PERSISTENT;
	void* ptr = HT_GET_DATA_ADDR(ht);
	//zend_printf("Free HashTable Data %lx, persistent=%d\n", ptr, persistent);

	pefree(ptr, persistent);
}

void Route_init::end() 
{

	freehtmemory(&verb_names);
	freehtmemory(&route_verbs);
	

	//showarray("route_verbs", &route_verbs);
	//showarray("verb_names", &verb_names);

	/*
	showarray("route_verbs", route_verbs);
	route_verbs.lose();
	showarray("route_verbs", route_verbs);

	showarray("route_verbs", verb_names);
	verb_names.lose();
	showarray("route_verbs", verb_names);
	*/
}

void Route_init::nodestroy(zval* val)
{
	// Allow zend to clean up str_intern
}
void Route_init::init() 
{

	//10 items, 16 slots is enough?

	zend_hash_init(&route_verbs, 16, NULL, nodestroy,1);
	zend_hash_init(&verb_names, 16, NULL, nodestroy,1);
	GET_S = "GET";
	POST_S = "POST";
	PUT_S = "PUT";
	PATCH_S = "PATCH";
	OPTIONS_S = "OPTIONS";
	DELETE_S = "DELETE";
	HEAD_S = "HEAD";
	CONNECT_S = "CONNECT";
	TRACE_S = "TRACE";
	PURGE_S = "PURGE";

	cc_verbs   = "verbs";
	cc_ajax = "ajax";
	cc_params = "params";
	cc_compiled = "compiled";
	cc_pattern  = "pattern";
	cc_target = "target";
	cc_id  = "id";

	ARG_S  = "_arg";
	FUN_S  = "_fun";
	FUNX_S = "_fnx";
	HITS_S = "_hit";
	MOD_S  = "_mod";
	NSP_S  = "_nsp";
	OBJ_S  = "_obj";
	OBJX_S = "_obx";
	ROLE_S = "_rol";


	htab_rw rv(&route_verbs);

	rv.set(GET_S, (int)html::V_GET);
	rv.set(POST_S, (int)html::V_POST);
	rv.set(PUT_S, (int)html::V_PUT);
	rv.set(PATCH_S, (int)html::V_PATCH);
	rv.set(OPTIONS_S, (int)html::V_OPTIONS);
	rv.set(DELETE_S, (int)html::V_DELETE);
	rv.set(HEAD_S, (int)html::V_HEAD);
	rv.set(CONNECT_S, (int)html::V_CONNECT);
	rv.set(TRACE_S, (int)html::V_TRACE);
	rv.set(PURGE_S, (int)html::V_PURGE);

	//zend_printf("route_verbs table size %d, used %d\n", route_verbs.nTableSize, route_verbs.nNumUsed);
	//verb_names.reset();

	htab_rw p2(&verb_names);

	// Push in ascending order (powers of 2)
	p2.push_back(GET_S); 
	p2.push_back(POST_S);
	p2.push_back(PUT_S);
	p2.push_back(PATCH_S);
	p2.push_back(OPTIONS_S);
	p2.push_back(DELETE_S);
	p2.push_back(HEAD_S);
	p2.push_back(CONNECT_S);
	p2.push_back(TRACE_S);
	p2.push_back(PURGE_S);	
	
	//showarray("verbs",rv);
	//showarray("names", p2);
}

void Route_init::init_ce(zend_class_entry* ce)
{
	//zend_printf("Init Route Class\n");
	class_data fn(ce);
	
// match the php script constant values
	fn.add_constant("GET_S",GET_S);
	fn.add_constant("POST_S",POST_S);
	fn.add_constant("PUT_S", PUT_S);
	fn.add_constant("PATCH_S", PATCH_S);
	fn.add_constant("OPTIONS_S", OPTIONS_S);
	fn.add_constant("DELETE_S", DELETE_S);
	fn.add_constant("HEAD_S", HEAD_S);
	fn.add_constant("CONNECT_S", CONNECT_S);
	fn.add_constant("TRACE_S", TRACE_S);
	fn.add_constant("PURGE_S", PURGE_S);

	fn.add_constant("GET_I", html::V_GET);
	fn.add_constant("POST_I",html::V_POST);
	fn.add_constant("PUT_I", html::V_PUT);
	fn.add_constant("PATCH_I", html::V_PATCH);
	fn.add_constant("OPTIONS_I", html::V_OPTIONS);
	fn.add_constant("DELETE_I", html::V_DELETE);
	fn.add_constant("HEAD_I", html::V_HEAD);
	fn.add_constant("CONNECT_I", html::V_CONNECT);
	fn.add_constant("TRACE_I", html::V_TRACE);
	fn.add_constant("PURGE_I", html::V_PURGE);


	fn.add_constant("AJAX_NONE", ajax::AJ_NONE);
	fn.add_constant("AJAX_ONLY", ajax::AJ_ONLY);
	fn.add_constant("AJAX_ALSO", ajax::AJ_ALSO);
}


void Route::RouteMgr::init_class_fn()
{
	base_obj_mgr::init_class_fn();
	route_data.init_ce(class_entry_);
}


Route::Route() : base_d(), verbs_(html::V_GET), ajax_(ajax::AJ_NONE)
{
	id_ = str_empty();
	//showstr("Route name", id_);
	compiled_ = str_empty();
	pattern_ = str_empty();
	params_ = htab_empty();
}

Route::~Route()
{
	//showmem("~Wcc_Route", pattern_);
}

void
Route::debug_info(htab_rw hw)
{
	hw.set(route_data.cc_verbs, (int)verbs_);

	hw.set(route_data.cc_ajax,  (int)ajax_);

	hw.set(route_data.cc_pattern,  pattern_);

	hw.set(route_data.cc_compiled,  compiled_);

	hw.set(route_data.cc_target,  target_);

	hw.set(route_data.cc_params,  params_);

	hw.set(route_data.cc_id,  id_);
}

htab_rc 
Route::__serialize()
{

	htab_rc result;

	htab_rw hw(result);
	hw.set(route_data.cc_id, id_);

	hw.set(route_data.cc_verbs,  (int)verbs_);
	hw.set(route_data.cc_ajax,  (int)ajax_);
	hw.set(route_data.cc_pattern,  pattern_);
	hw.set(route_data.cc_compiled,  compiled_);
	hw.set(route_data.cc_target,  target_);
	hw.set(route_data.cc_params,  params_);

	return result;
}

void
Route::__unserialize(htab_ptr htab)
{
	val_ptr temp;

	params_ = htab.get(route_data.cc_params);

	if (htab.try_fetch(route_data.cc_compiled, temp))
	{	
		 compiled_ = temp.zstr();
		 //temp.set_null();
	}

	if (htab.try_fetch(route_data.cc_pattern, temp))
	{
		 pattern_ = temp.zstr();
		 //temp.set_null();
	}

	//showmem("target_ before  ", target_);
	target_ = htab.get(route_data.cc_target);
	//dump_info::msg_dump("target_ unserialize  ", target_);
	if (htab.try_fetch(route_data.cc_verbs, temp))
	{
		if (temp.isLong())
		{
			 verbs_ = temp.zlong();
			 //temp.set_null();
		}
	}

	if (htab.try_fetch(route_data.cc_ajax, temp))
	{
		if (temp.isLong())
		{
			 ajax_ = temp.zlong();
			 //temp.set_null();
		}
	}
	if (htab.try_fetch(route_data.cc_id, temp))
	{
		if (temp.isString())
		{
			 id_ = temp.zstr();
		}
	}	
	//htab_own showme;
	//debug_info(showme);
	//showme.show_data("unserialized");
}

void 
Route::construct(int verbs, str_ptr pattern, val_ptr target)
{
	verbs_ = verbs;
	pattern_ = pattern;
	target_ = target;
}

obj_rc //static
Route::get(str_ptr pattern, val_ptr target, int ajax)
{
	obj_rc result;

	result = Route::omg.new_zobj();
	//showobj("new Route zobj", result);
	Route* cobj = zobj_toc<Route>(result);

	cobj->construct(html::V_GET, pattern, target);
	cobj->ajax_ = ajax;

	return result;
}

obj_rc  //static
Route::post(str_ptr pattern, val_ptr target, int ajax)
{
	obj_rc result;

	result =  Route::omg.new_zobj();
	Route* cobj = zobj_toc<Route>(result);

	cobj->construct(html::V_POST, pattern, target);
	cobj->ajax_ = ajax;

	return result;	
}

obj_rc  //static
Route::methods(int verbs, str_ptr pattern, val_ptr target, int ajax)
{
	obj_rc result;

	result =  Route::omg.new_zobj();
	Route* cobj = zobj_toc<Route>(result);

	cobj->construct(verbs, pattern, target);
	cobj->ajax_ = ajax;

	return result;	
}
	

zend_long //static
Route::getVerbInt(str_ptr sverb)
{
	str_rc verbstr(sverb);
	verbstr.uppercase();
	
	htab_ptr hr(&route_data.route_verbs);

	val_ptr test;

	if (hr.try_fetch(verbstr,test))
	{
		return test.zlong();
	}
	return 0;
}

void 
Route::name(str_ptr name)
{
	//showstr("Route::name", name);
	id_ = name;
}

str_ptr 
Route::getName()
{
	return id_;
}

htab_ptr 
Route::getParams()
{
	return params_;
}

void 
Route::setParams(htab_ptr params)
{
	params_ = params;
}

class param_replace : public preg_callback {
public:
	htab_rc params_;

	param_replace(const char* expr, htab_ptr plist)
	   : preg_callback(expr), params_(plist)
	{			
	}

	bool callback() override
	{
		int ix = call_ct_;
		//showdata("\ncaptures_",captures_);
		if ((int)captures_.size() > ix)
		{
			htab_ptr key_expr = captures_.get(int(ix));
			str_ptr  key = key_expr.get(int(0));
			//showstr("key ", key);
			if (key.ok())
			{
				val_ptr vp = params_.get(key);
				replace_ = vp.to_zstr();
				return true;
			}
		}
		replace_.init();
		return false;
	}
};

str_rc
Route::routeUrl(htab_ptr pvalues)
{
	str_rc result;


	if (htab_ptr(this->params_).size() == 0)
	{
		result = pattern_;
	}
	else {
		
		param_replace pcb(R"x(#\{([a-zA-Z][\w\d]*)\}#)x", pvalues);

		result = pcb.replace(pattern_);
	}
	return result;

}



//static const char* start_rex = "#^";
//static const char* end_rex = "$#";



//static 
htab_rc 
Route::getVerbNames( zend_long flags )
{
	htab_rc result;

	htab_rw rval(result);

	htab_ptr names(&route_data.verb_names);

	size_t nct = names.size();
	for( size_t mix = 0; mix < nct; mix++) {
		size_t mask = 1 << mix;
		if ((flags & mask) != 0) {
			val_ptr vname = names.get(mix);
			if (vname.isString()) {
				rval.push_back(vname);
			}
		}
	}
	return result;
}


// static method returns string
// for first verb bit encountered.

str_rc 
Route::getVerb(zend_long verb)
{
	htab_ptr names(&route_data.verb_names);
	str_rc rval;

	for( zend_long mix = 0; mix < 9; mix++) {
		zend_long mask = 1 << mix;
		if ((verb & mask) != 0) {
			rval = names.get(mix);
			if (rval.ok()) {
				break;
			}
		}
	}
	return rval;
}

}; // end namespace wcc -@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@



PHP_METHOD(Wcc_Route, __construct)
{
	zend_string    *pattern;
	zval           *target;
	zend_long      verbs;
	

	ZEND_PARSE_PARAMETERS_START(3, 3)
	Z_PARAM_LONG(verbs)
	Z_PARAM_STR(pattern)
	Z_PARAM_ZVAL(target)
	ZEND_PARSE_PARAMETERS_END();

	Route* cobj = zval_toc<Route>(ZEND_THIS);

	cobj->construct(verbs, pattern, target);
}

//static
PHP_METHOD(Wcc_Route, get)
{
	zend_string    *pattern;
	zval           *target;
	zend_long      ajax = ajax::AJ_NONE;
	

	ZEND_PARSE_PARAMETERS_START(2, 3)
	Z_PARAM_STR(pattern)
	Z_PARAM_ZVAL(target)
	Z_PARAM_OPTIONAL
	Z_PARAM_LONG(ajax)
	ZEND_PARSE_PARAMETERS_END();

	obj_rc obj = Route::get(pattern, target, ajax);
	
	obj.move_zv(return_value);
	//Static call doesn't need a reference boost?
	//showmem("static::get", return_value);
	//val_rc::try_addref(return_value);
	

}

//static
PHP_METHOD(Wcc_Route, post)
{
	zend_string    *pattern;
	zval           *target;
	zend_long      ajax = ajax::AJ_NONE;
	

	ZEND_PARSE_PARAMETERS_START(2, 3)
	Z_PARAM_STR(pattern)
	Z_PARAM_ZVAL(target)
	Z_PARAM_OPTIONAL
	Z_PARAM_LONG(ajax)
	ZEND_PARSE_PARAMETERS_END();

	obj_rc obj = Route::post(pattern, target, ajax);
	obj.move_zv(return_value);
	//needs a reference boost
	//val_rc::try_addref(return_value);
}

//static
PHP_METHOD(Wcc_Route, methods)
{
	zend_long      verbs;
	zend_string    *pattern;
	zval           *target;
	zend_long      ajax = ajax::AJ_NONE;


	ZEND_PARSE_PARAMETERS_START(3, 4)
	Z_PARAM_LONG(verbs)
	Z_PARAM_STR(pattern)
	Z_PARAM_ZVAL(target)
	Z_PARAM_OPTIONAL
	Z_PARAM_LONG(ajax)
	ZEND_PARSE_PARAMETERS_END();

	obj_rc obj = Route::methods(verbs, pattern, target, ajax);
	obj.move_zv(return_value);
}

PHP_METHOD(Wcc_Route, hasVerb) 
{
	zend_long      verb;


	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_LONG(verb)
	ZEND_PARSE_PARAMETERS_END();


	Route* cobj = zval_toc<Route>(ZEND_THIS);

	if ( (verb & cobj->verbs_) != 0 ) {
		RETURN_TRUE;
	}
	else {
		RETURN_FALSE;
	}
}

PHP_METHOD(Wcc_Route, allow) 
{
	zend_long      verb;
	zend_long      ajax;

	ZEND_PARSE_PARAMETERS_START(2, 2)
	Z_PARAM_LONG(verb)
	Z_PARAM_LONG(ajax)
	ZEND_PARSE_PARAMETERS_END();


	Route* cobj = zval_toc<Route>(ZEND_THIS);

	if ( ((verb & cobj->verbs_) != 0) && ((ajax & cobj->ajax_) != 0) ) {
		RETURN_TRUE;
	}
	else {
		RETURN_FALSE;
	}
}

PHP_METHOD(Wcc_Route, getCompiled) 
{

	ZEND_PARSE_PARAMETERS_NONE();

	Route* cobj = zval_toc<Route>(ZEND_THIS);
	str_ptr s = cobj->getCompiled();
	s.copy_zv(return_value);
}

PHP_METHOD(Wcc_Route, getPattern) 
{

	ZEND_PARSE_PARAMETERS_NONE();

	Route* cobj = zval_toc<Route>(ZEND_THIS);
	str_ptr s = cobj->getPattern();
	s.copy_zv(return_value);
}

PHP_METHOD(Wcc_Route, getVerbs) 
{
	ZEND_PARSE_PARAMETERS_NONE();

	Route* cobj = zval_toc<Route>(ZEND_THIS);

	ZVAL_LONG(return_value, cobj->verbs_);
}

PHP_METHOD(Wcc_Route, getParams) 
{
	ZEND_PARSE_PARAMETERS_NONE();

	Route* cobj = zval_toc<Route>(ZEND_THIS);
	htab_ptr hr = cobj->getParams();
	hr.copy_zv(return_value);
}




// static method, no this. return value of zero means not found.
PHP_METHOD(Wcc_Route, GetVerbInt) 
{
	zend_string*	verb;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(verb)
	ZEND_PARSE_PARAMETERS_END();

	zend_long result = Route::getVerbInt(verb);

	RETURN_LONG(result);
}

//static
PHP_METHOD(Wcc_Route, GetVerbNames) 
{

	zend_long           verbs;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_LONG(verbs)
	ZEND_PARSE_PARAMETERS_END();

	htab_rc rval = Route::getVerbNames(verbs);

	rval.move_zv(return_value);
}


PHP_METHOD(Wcc_Route, setCompiled) 
{
	zend_string* cpat;
	 
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(cpat)
	ZEND_PARSE_PARAMETERS_END();

	Route* cobj = zval_toc<Route>(ZEND_THIS);
	cobj->setCompiled(cpat);
	return;
}

PHP_METHOD(Wcc_Route, setPattern)
{
	zend_string* pattern;
	 
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(pattern)
	ZEND_PARSE_PARAMETERS_END();

	Route* cobj = zval_toc<Route>(ZEND_THIS);
	cobj->setPattern(pattern);
}

PHP_METHOD(Wcc_Route, setAjax)
{
	Route* cobj = zval_toc<Route>(ZEND_THIS);
	 
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_LONG(cobj->ajax_);
	ZEND_PARSE_PARAMETERS_END();

}

PHP_METHOD(Wcc_Route, getAjax)
{
	ZEND_PARSE_PARAMETERS_NONE();
	
	Route* cobj = zval_toc<Route>(ZEND_THIS);

	ZVAL_LONG(return_value, cobj->ajax_);
}

PHP_METHOD(Wcc_Route, setParams)
{
	zval* data;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ARRAY(data)
	ZEND_PARSE_PARAMETERS_END();

	Route* cobj = zval_toc<Route>(ZEND_THIS);
	cobj->setParams(data);
	return;

}

PHP_METHOD(Wcc_Route, setTarget)
{
	zval* target;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(target)
	ZEND_PARSE_PARAMETERS_END();

	Route* cobj = zval_toc<Route>(ZEND_THIS);

	cobj->target_ = target;

	return;

}


//static
PHP_METHOD(Wcc_Route, GetVerb)
{
	zend_long verb;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_LONG(verb)
	ZEND_PARSE_PARAMETERS_END();

	str_rc rval = Route::getVerb(verb);

	rval.move_zv(return_value);
}

PHP_METHOD(Wcc_Route, hasParams)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Route* cobj = zval_toc<Route>(ZEND_THIS);

	if (cobj->hasParams())
	{
		RETURN_TRUE;
	}
	else {
		RETURN_FALSE;
	}
}

PHP_METHOD(Wcc_Route, getTarget)
{
	ZEND_PARSE_PARAMETERS_NONE();
	
	Route* cobj = zval_toc<Route>(ZEND_THIS);
	val_ptr ret = cobj->getTarget();

	ret.copy_zv(return_value);

}

PHP_METHOD(Wcc_Route, name)
{
	
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(name);
	ZEND_PARSE_PARAMETERS_END();

	Route* cobj = zval_toc<Route>(ZEND_THIS);
	cobj->name(name);

	//Non-static (method) call
	// and PHP needs reference count boost (for some reason)
	ZVAL_COPY(return_value, ZEND_THIS);
	//showmem("ZEND_THIS", ZEND_THIS);
	//showmem("return_value", return_value);

}

PHP_METHOD(Wcc_Route, routeUrl)
{
	
	zval* params = nullptr;

	ZEND_PARSE_PARAMETERS_START(0, 1)
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY_OR_NULL(params);
	ZEND_PARSE_PARAMETERS_END();

	htab_ptr plist;

	if (params)
	{
		plist = params;
	}
	Route* cobj = zval_toc<Route>(ZEND_THIS);
	val_rc result = cobj->routeUrl(plist);

	result.move_zv(return_value);
}

PHP_METHOD(Wcc_Route,getName)
{
	ZEND_PARSE_PARAMETERS_NONE();
	
	Route* cobj = zval_toc<Route>(ZEND_THIS);

	str_ptr name = cobj->getName();
	name.copy_zv(return_value);

}

PHP_METHOD(Wcc_Route, __serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();
	
	Route* cobj = zval_toc<Route>(ZEND_THIS);

	htab_rc ret = cobj->__serialize();

	ret.move_zv(return_value);
}

PHP_METHOD(Wcc_Route, __unserialize)
{
	zval* data;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ARRAY(data)
	ZEND_PARSE_PARAMETERS_END();

	htab_ptr hr(data);

	Route* cobj = zval_toc<Route>(ZEND_THIS);

	cobj->__unserialize(hr);

}
PHP_MSHUTDOWN_FUNCTION(wcc_route_d)
{
	zend_printf("wcc_route_d shutown\n");
	//Route::clean_static();

	return SUCCESS;
}

PHP_MINIT_FUNCTION(wcc_route_d)
{
	//WcR_ce = wcc_class_reg("Wc\\Route", class_Wcc_Route_methods);
	zend_class_entry *ce = register_class_Wcc_Route();
	Route::omg.classEntry(ce);
	
	STATE_INIT_ADD(route_data)
	
	return SUCCESS;

}
#endif