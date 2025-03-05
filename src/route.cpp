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

namespace wcc {
 	
base_obj_mgr<Route> Route::omg;

Route_init route_data;


void Route_init::end() 
{
	zend_hash_clean(&route_verbs);
	zend_hash_clean(&verb_names);
	/*
	showarray("route_verbs", route_verbs);
	route_verbs.lose();
	showarray("route_verbs", route_verbs);

	showarray("route_verbs", verb_names);
	verb_names.lose();
	showarray("route_verbs", verb_names);
	*/
}

void Route_init::init() 
{

	//route_verbs.reset();
	zend_hash_init(&route_verbs, 10, NULL, ZVAL_PTR_DTOR,1);
	zend_hash_init(&verb_names, 10, NULL, ZVAL_PTR_DTOR,1);
	GET_S = zstr_intern("GET");
	POST_S = zstr_intern("POST");
	PUT_S = zstr_intern("PUT");
	PATCH_S = zstr_intern("PATCH");
	OPTIONS_S = zstr_intern("OPTIONS");
	DELETE_S = zstr_intern("DELETE");
	HEAD_S = zstr_intern("HEAD");
	CONNECT_S = zstr_intern("CONNECT");
	TRACE_S = zstr_intern("TRACE");
	PURGE_S = zstr_intern("PURGE");

	cc_verbs   = zstr_intern("verbs");
	cc_ajax = zstr_intern("ajax");
	cc_params = zstr_intern("params");
	cc_compiled = zstr_intern("compiled");
	cc_pattern  = zstr_intern("pattern");
	cc_target = zstr_intern("target");
	cc_id  = zstr_intern("id");

	cc_fixed =  zstr_intern("fixed");
	cc_vary = zstr_intern("vary");
	cc_file = zstr_intern("file");

	htab_write rv(&route_verbs);

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

	//verb_names.reset();

	htab_write p2(&verb_names);

	// Order of enum values. (power of 2)
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


void RouteMgr::init_class_fn()
{
	base_obj_mgr::init_class_fn();
	route_data.init_ce(class_entry_);
}


// class has base_obj_mgr zend_class_entry
int isRouteObject(zend_object* obj) {
	return (Route::omg.classEntry() == obj->ce) ? 1 : 0;
}


Route::Route() : base_d(), verbs_(0), ajax_(0)
{
	compiled_ = zstr_empty();
	pattern_ = zstr_empty();
	params_ = htab_empty();
}

Route::~Route()
{
	//showmem("~Wcc_Route", pattern_);
}

void
Route::debug_info(htab_write hw)
{
	hw.set(route_data.cc_verbs, (int)verbs_);

	hw.set(route_data.cc_ajax,  (int)ajax_);

	hw.set(route_data.cc_pattern,  pattern_);

	hw.set(route_data.cc_compiled,  compiled_);

	hw.set(route_data.cc_target,  target_);

	hw.set(route_data.cc_params,  params_);

	hw.set(route_data.cc_id,  id_);
}

htab_mgr 
Route::__serialize()
{

	htab_mgr result;

	htab_write hw(result);
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
Route::__unserialize(htab_read htab)
{
	zval_user temp;

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
Route::construct(int verbs, zstr_user pattern, zval_user target)
{
	verbs_ = verbs;
	pattern_ = pattern;
	target_ = target;
}

zobj_mgr //static
Route::get(zstr_user pattern, zval_user target, int ajax)
{
	zobj_mgr result;

	result = Route::omg.new_zobj();
	Route* cobj = zobj_toc<Route>(result);

	cobj->construct(html::V_GET, pattern, target);
	cobj->ajax_ = ajax;

	result.adopt(cobj);
	return result;
}

zobj_mgr  //static
Route::post(zstr_user pattern, zval_user target, int ajax)
{
	zobj_mgr result;

	result =  Route::omg.new_zobj();
	Route* cobj = zobj_toc<Route>(result);

	cobj->construct(html::V_POST, pattern, target);
	cobj->ajax_ = ajax;

	return result;	
}

zobj_mgr  //static
Route::methods(int verbs, zstr_user pattern, zval_user target, int ajax)
{
	zobj_mgr result;

	result =  Route::omg.new_zobj();
	Route* cobj = zobj_toc<Route>(result);

	cobj->construct(verbs, pattern, target);
	cobj->ajax_ = ajax;

	return result;	
}
	

zend_long 
Route::getVerbInt(zstr_user sverb)
{
	zstr_mgr verbstr = sverb.to_upper();
	htab_read hr(&route_data.route_verbs);
	zval_user test;

	if (hr.try_fetch(verbstr,test))
	{
		return test.zlong();
	}
	return 0;
}

void 
Route::name(zstr_user name)
{
	id_ = name;
}

zstr_user 
Route::getName()
{
	return id_;
}

htab_read 
Route::getParams()
{
	return params_;
}

void 
Route::setParams(htab_read params)
{
	params_ = params;
}

class param_replace : public preg_callback {
public:
	htab_mgr params_;

	param_replace(htab_read plist) : params_(plist)
	{			
	}

	virtual bool get_replace(htab_read captures)
	{
		htab_read plist(params_);
		if (call_count_ < plist.size())
		{
			zstr_user key = captures.get(int(1));
			zstr_mgr value = plist.get(call_count_);
			if (value.isNull())
			{
				value = plist.get(key);
			}
			if (!value.isNull())
			{
				replace_ = value;
				return true;
			}
			else {
				return false;
			}
		}
		return false;
	}
};

zstr_mgr
Route::routeUrl(htab_read pvalues)
{
	zstr_mgr result;


	if (htab_read(this->params_).size() == 0)
	{
		result = pattern_;
	}
	else {
		preg url_params("#{([a-zA-Z][\\w\\d]*)}#");

		param_replace replace(pvalues);

		result = url_params.replace_callback(replace, pattern_);
	}
	return result;

}



static const char* start_rex = "#^";
static const char* end_rex = "$#";



//static 
htab_mgr 
Route::getVerbNames( zend_long flags )
{
	htab_mgr result;

	htab_write rval(result);

	htab_read names(&route_data.verb_names);

	size_t nct = names.size();
	for( size_t mix = 0; mix < nct; mix++) {
		size_t mask = 1 << mix;
		if ((flags & mask) != 0) {
			zval_user vname = names.get(mix);
			if (vname.isString()) {
				rval.push_back(vname);
			}
		}
	}
	return result;
}


// static method returns string
// for first verb bit encountered.

zstr_mgr 
Route::getVerb(zend_long verb)
{
	htab_read names(&route_data.verb_names);
	zstr_mgr rval;

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

	zobj_mgr obj = Route::get(pattern, target, ajax);
	obj.move_zv(return_value);
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

	zobj_mgr obj = Route::post(pattern, target, ajax);
	obj.move_zv(return_value);
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

	zobj_mgr obj = Route::methods(verbs, pattern, target, ajax);
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
	zstr_user s = cobj->getCompiled();
	s.return_zv(return_value);
}

PHP_METHOD(Wcc_Route, getPattern) 
{

	ZEND_PARSE_PARAMETERS_NONE();

	Route* cobj = zval_toc<Route>(ZEND_THIS);
	zstr_user s = cobj->getPattern();
	s.return_zv(return_value);
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
	htab_read hr = cobj->getParams();
	hr.return_zv(return_value);
}




// static method, no this. return value of zero means not found.
PHP_METHOD(Wcc_Route, GetVerbInt) 
{
	zend_string*	verb;
	long vlen;
	zend_string* key;
	zval  *zv_find = NULL;
	zval_mgr test;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(verb)
	ZEND_PARSE_PARAMETERS_END();

	vlen = ZSTR_LEN(verb);

	if (vlen > 0) {
		RETURN_LONG(Route::getVerbInt(verb));
	}
	RETURN_LONG(0);
}

//static
PHP_METHOD(Wcc_Route, GetVerbNames) 
{

	zend_long           verbs;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_LONG(verbs)
	ZEND_PARSE_PARAMETERS_END();

	htab_mgr rval = Route::getVerbNames(verbs);

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

	zstr_mgr rval = Route::getVerb(verb);

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
	zval_user ret = cobj->getTarget();

	ret.return_zv(return_value);

}

PHP_METHOD(Wcc_Route, name)
{
	
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(name);
	ZEND_PARSE_PARAMETERS_END();

	Route* cobj = zval_toc<Route>(ZEND_THIS);
	cobj->name(name);

	zobj_mgr result(cobj->zobj());
	result.move_zv(return_value);
}

PHP_METHOD(Wcc_Route, routeUrl)
{
	
	zval* params = nullptr;

	ZEND_PARSE_PARAMETERS_START(0, 1)
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY(params);
	ZEND_PARSE_PARAMETERS_END();

	htab_read plist;

	if (params)
	{
		plist = params;
	}
	Route* cobj = zval_toc<Route>(ZEND_THIS);
	zval_mgr result = cobj->routeUrl(plist);

	result.move_zv(return_value);
}

PHP_METHOD(Wcc_Route,getName)
{
	ZEND_PARSE_PARAMETERS_NONE();
	
	Route* cobj = zval_toc<Route>(ZEND_THIS);

	zstr_user name = cobj->getName();
	name.return_zv(return_value);

}

PHP_METHOD(Wcc_Route, __serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();
	
	Route* cobj = zval_toc<Route>(ZEND_THIS);

	htab_mgr ret = cobj->__serialize();

	ret.move_zv(return_value);
}

PHP_METHOD(Wcc_Route, __unserialize)
{
	zval* data;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ARRAY(data)
	ZEND_PARSE_PARAMETERS_END();

	htab_read hr(data);

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
	
	return SUCCESS;

}
#endif