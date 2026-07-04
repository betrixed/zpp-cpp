#ifndef WCC_SERVICES_CPP
#define WCC_SERVICES_CPP

#ifndef WCC_SERVICES_H
#include "services.h"
#endif

#ifndef FN_CALL_H
#include "zpp/fn_call.h"
#endif

#ifndef REFLECT_CACHE_H
#include "reflect_cache.h"
#endif

//#define DBG_SERVICES

#ifdef DBG_SERVICES
#ifndef WCC_DEBUGLOG_H
#include "debuglog.h"
#endif
#endif

#ifndef WCC_SERVICES_ARGINFO
#define WCC_SERVICES_ARGINFO
extern "C" {
	#include "stub/services_arginfo.h"
};
#endif

namespace wcc 
{
	using namespace zpp;
	

	base_obj_mgr<Services> Services::omg;

	thread_local obj_rc	g_services;

	class Services_data : public state_init {
	public:
		str_intern active;
		str_intern defer;
		str_intern instances;
		str_intern throw_fail;
		str_intern defer_ct;
		str_intern call_str;

	 	void init() override 
		{
			
			 active = "active";
			
			 defer = "defer";
			
			 instances = "instances";
			
			 throw_fail = "throw_fail";
			
			 defer_ct = "defer_ct";

			 call_str = "call";
		}

		void init_req() override
		{
			g_services = Services::omg.new_zobj();
			//showobj("new g_services", g_services);
		}

		void end_req() override
		{
			//zend_printf("end_req Services_data ");
			//showobj("g_services", g_services);
			g_services.init();
		}
	};

	Services_data SVC_data;

val_rc 
Services::call_value(obj_ptr callme)
{
	//"call" method on closure obj
	val_rc arg(self_);

	return callme.callable(arg);
}

Services::~Services()
{
}


void	
Services::clearActive()
{
	active_.reset();
}

void       
Services::clearDefer()
{
	defer_.reset();
}

void       
Services::clearObjects()
{
	instances_.reset();
}

val_return  
Services::activate(str_ptr key)
{
#ifdef DBG_SERVICES
	DebugLog* log = DebugLog::cpp_global();
#endif

	val_return result;
	//zend_printf("Services::activate(\"%s\")\n", key.data());

	val_ptr test;

	test = defer_.get(key);

	if (test.is_nullptr())
	{
		result.error() << "Service " << key << " does not exist";
		return result;
	}

	if (defer_ct_ > 3)
	{
		defer_ct_ = 0;
		result.error() << "Service request '" << key << "' recursed more than 3 times";
		return result;
	}

	defer_ct_++;

	if (test.isCallable())
	{	
#ifdef DBG_SERVICES
		log->dump("activate callable", test);
#endif

		obj_ptr callme = test.zobject();

		//zend_printf("Callable object \n");

		val_rc result2 = call_value(callme);

		//showmem("callme return", result2);

		htab_rw(active_).set(key, result2);

		result = get(key);
	}
	else {
#ifdef DBG_SERVICES
		log->dump("activate value", test);
#endif
		result.value_ = test;
	}
	defer_ct_--;
	return result;
}

Services::Services() 
{
	throw_fail_ = false;
	defer_ct_ = 0;
}

Services* 
Services::cpp_global()
{
	//obj_ptr sv = g_services;

	///DebugLog* log = DebugLog::cpp_global();

	///log->dump("cpp_global g_services", sv);

	return zobj_toc<Services>(g_services);
}

obj_rc
Services::instance()
{
	return g_services;
	/*
	obj_ptr result;

	Global gme = GLOBALS[Services::omg.class_name()];
	
	val_ptr val = gme.value(); // good for while gme is around
	showmem("read instance", val);

	if (val.isNull()) {
		obj_rc obj = Services::omg.new_zobj();
		showobj("new return", obj);
		gme = obj;
		result = (zend_object*) obj;
		showobj("as global", result);
	}
	else
	{
		result = val;
	}
	return result;
	*/

}

/* static */
obj_rc 
Services::setOne(str_ptr key, obj_ptr obj)
{
	Services* self = Services::cpp_global();
	return self->setObject(obj,key);
}

/* static */
obj_return  
Services::makeOne(str_ptr key, htab_ptr arglist)
{
	Services* me = Services::cpp_global();
	return me->newInstance(key, arglist);
}

/* static */
obj_return  
Services::getOne(str_ptr key, htab_ptr arglist)
{
	obj_return  result;
	obj_rc services = Services::instance();
#ifdef DBG_SERVICES
	DebugLog* log = DebugLog::cpp_global();
	if (log)
	{   
		log->dump("key", key);
	}
#endif

	Services* svc = zobj_toc<Services>(services);

	obj_rc single = svc->getObject(key);

	if (single.ok())
	{
		result.value_ = std::move(single);
		return result;	
	}

	if (class_exists(key, false))
	{
		result = svc->newInstance(key, arglist);
	}
	else {
		result.error() << "Services::getOne" 
			" cannot make object with key " << key;
	}
	return result;

}

// static
val_return  
Services::service(str_ptr key)
{
	Services* self = Services::cpp_global();

	return self->get(key);
}

// static
val_return  
Services::service(const std::string_view& key)
{
	val_return result;

	auto slen = key.size();
	if (slen)
	{
		str_rc skey(key.data(), slen);
		result = Services::service(skey);
	}
	return result;
}

obj_return
Services::newInstance(str_ptr name_class, htab_ptr arglist)
{
	obj_return result;

	ReflectCache* rc = ReflectCache::cpp();

	//showstr("newInstance of ", name_class);
	obj_rc obj;
	if (arglist.size())
	{
#ifdef DBG_SERVICES
		DebugLog* log = DebugLog::cpp_global();
		log->dump("newInstanceArgs", arglist);
#endif
		obj = rc->newInstanceArgs(name_class, arglist);
	}
	else {
		obj = rc->newInstance(name_class);
	}
	

	if (obj.ok())
	{
		//showarray("instances", instances_);
		htab_rw(instances_).set(name_class, obj);
		result.value_ = std::move(obj);
	}
	else {
		 result.error() << "Failed to make " << name_class;
	}
	return result;
}

obj_rc
Services::getObject(str_ptr key)
{
	obj_rc result;
#ifdef DBG_SERVICES
	DebugLog* log = DebugLog::cpp_global();
	if (log)
	{
		log->dump("Instances", instances_);
	}
#endif

	result = instances_.get(key);
#ifdef DBG_SERVICES
	if (log)
	{
		log->dump("getObject result", result);
	}
#endif
	return result;
}

obj_rc
Services::setObject(obj_ptr obj, str_ptr key)
{
	if (key.isNull()) {
		// get class name of object
		key = obj.className();
	}

	htab_rw(instances_).set(key, obj);
	return obj;
}


bool Services::isActive(str_ptr name)
{

	return htab_ptr(active_).has_key(name);
}

bool Services::has(str_ptr name)
{
	return (htab_ptr(active_).has_key(name) || htab_ptr(defer_).has_key(name));
}


void Services::setDefer(str_ptr name, val_ptr value)
{
	htab_rw(defer_).set(name, value);
}

void  Services::set(str_ptr name, val_ptr value)
{
	zval* data = (zval*) value;
	zend_string* key = (zend_string*) name;

	htab_rw temp(active_);

	temp.set(key, data);
}

void  
Services::set(str_ptr name, obj_ptr obj)
{
	htab_rw temp(active_);

	temp.set(name, obj);
}

void
Services::set(str_ptr name, val_rc& val)
{
	set(name, val_ptr(val));
}

val_return  
Services::get(str_ptr name)
{
	val_return result;
#ifdef DBG_SERVICES
	DebugLog* log = DebugLog::cpp_global();
	log->dump("svc get", name);
#endif
	if (!name.size())
	{
		return result;
	}

	val_ptr test = active_.get(name);

	if (test.is_nullptr())
	{
		//check defered 
		result = activate(name);
		return  result;
	}

	//auto ztype = test.ref_type();


	if (test.isCallable())
	{	
#ifdef DBG_SERVICES
		log->dump("get isCallable", test);
#endif
		obj_ptr callme = test.zobject();
		result.value_ = call_value(callme);
	}
	else {
#ifdef DBG_SERVICES
		log->dump("get Value", test);
#endif
		result.value_ = test;
	}
	return result;
}


void  
Services::unset(str_ptr name)
{
	htab_rw(active_).unset(name);
}


void  
Services::setThrowFail(bool value)
{
	throw_fail_ = value;
}

void Services::debug_info(htab_rw info)
{
	base_d::debug_info(info);

	info.set(SVC_data.active, active_);
	info.set(SVC_data.defer, defer_);
	info.set(SVC_data.instances, instances_);
	info.set(SVC_data.throw_fail, throw_fail_);
	info.set(SVC_data.defer_ct,   defer_ct_);

}

}; // namespace wcc

using namespace wcc;

ZEND_METHOD(Wcc_Services, getOne)
{
	zarg_rd args(execute_data);

	str_ptr skey = args.str(args.need(0));
	htab_ptr arglist = args.htab(args.option(1));

	if (!args.throw_errors())
	{
		obj_return result = Services::getOne(skey, arglist);
		if (!result.throw_errors(__FUNCTION__))
		{
			result.value_.move_zv(return_value);
		}
	}
}

ZEND_METHOD(Wcc_Services, makeOne)
{
	zarg_rd args(execute_data);

	str_ptr skey = args.str(args.need(0));
	htab_ptr arglist = args.htab(args.option(1));

	if (!args.throw_errors())
	{
		obj_return result = Services::makeOne(skey, arglist);
		if (!result.throw_errors(__FUNCTION__))
		{
			result.value_.move_zv(return_value);
		}
	}
}

ZEND_METHOD(Wcc_Services, instance)
{
	ZEND_PARSE_PARAMETERS_NONE();

	obj_rc result = Services::instance();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Services, service)
{
	zarg_rd args(execute_data);

	str_ptr skey = args.str(args.need(0));

	if (!args.throw_errors())
	{
		val_return result = Services::service(skey);
		result.value_.move_zv(return_value);
	}
}

ZEND_METHOD(Wcc_Services, setOne)
{
	zarg_rd args(execute_data);

	str_ptr skey = args.str(args.need(0));
	obj_ptr obj = args.obj(args.need(1));

	if (!args.throw_errors())
	{
		Services* svc = zval_toc<Services>(ZEND_THIS);
		obj_rc result = svc->setOne(skey, obj);
		result.move_zv(return_value);
	}
}


ZEND_METHOD(Wcc_Services, unset)
{
	zend_string* skey;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(skey)
	ZEND_PARSE_PARAMETERS_END();

	Services* svc = zval_toc<Services>(ZEND_THIS);
	svc->unset(skey);
}

ZEND_METHOD(Wcc_Services, get)
{
	zarg_rd args(execute_data);
	str_ptr name = args.str(args.need(0));

	if (!args.throw_errors())
	{
		Services* svc = zval_toc<Services>(ZEND_THIS);
		val_return result = svc->get(name);
		if (!result.throw_errors()){
			result.value_.move_zv(return_value);
		}
	}
}

ZEND_METHOD(Wcc_Services, getObject)
{
	zend_string* skey;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(skey)
	ZEND_PARSE_PARAMETERS_END();

	Services* svc = zval_toc<Services>(ZEND_THIS);
	obj_rc result = svc->getObject(skey);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Services, has)
{
	zend_string* skey;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(skey)
	ZEND_PARSE_PARAMETERS_END();

	Services* svc = zval_toc<Services>(ZEND_THIS);
	RETURN_BOOL(svc->has(skey));

}

ZEND_METHOD(Wcc_Services, isActive)
{
	zend_string* skey;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(skey)
	ZEND_PARSE_PARAMETERS_END();

	Services* svc = zval_toc<Services>(ZEND_THIS);

	RETURN_BOOL(svc->isActive(skey));

}

ZEND_METHOD(Wcc_Services, newInstance)
{
	zarg_rd args(execute_data);

	str_ptr cname =  args.str(args.need(0));
	htab_ptr arglist = args.htab_or_null(args.option(1));

	if (!args.throw_errors())
	{
		Services* svc = zval_toc<Services>(ZEND_THIS);
		obj_return result = svc->newInstance(cname, arglist);
		if (!result.throw_errors())
		{
			result.value_.move_zv(return_value);
		}
	}
}


ZEND_METHOD(Wcc_Services, set)
{
	zarg_rd args(execute_data);

	str_ptr skey;
	val_ptr pvalue;

	args.zstring(skey, args.need(0));
	
	pvalue = args.need(1);

	if (!args.throw_errors(__FUNCTION__))
	{
		Services* svc = zval_toc<Services>(ZEND_THIS);
		svc->set(skey, pvalue);
	}

}

ZEND_METHOD(Wcc_Services, setObject)
{
	obj_ptr obj;
	str_ptr key;
	

	zarg_rd args(execute_data);

	args.obj(obj, args.need(0));
	args.zstring_null(key, args.option(1));

	if (!args.throw_errors(__FUNCTION__))
	{
		Services* svc = zval_toc<Services>(ZEND_THIS);
		// result is now referenced in services
		
		obj_rc result = svc->setObject(obj, key);
		result.move_zv(return_value);
	}
	// handles potential null

}

ZEND_METHOD(Wcc_Services, setDefer)
{
	zend_string* skey;
	zval*        svalue;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR(skey)
		Z_PARAM_ZVAL(svalue)
	ZEND_PARSE_PARAMETERS_END();

	Services* svc = zval_toc<Services>(ZEND_THIS);

	svc->setDefer(skey, svalue);
}

ZEND_METHOD(Wcc_Services, setThrowFail)
{

	bool        bvalue;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_BOOL(bvalue)
	ZEND_PARSE_PARAMETERS_END();

	Services* svc = zval_toc<Services>(ZEND_THIS);
	svc->setThrowFail(bvalue);	
}



ZEND_METHOD(Wcc_Services, clearActive)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	Services* svc = zval_toc<Services>(ZEND_THIS);
	svc->clearActive();
}

ZEND_METHOD(Wcc_Services, clearDefer)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	Services* svc = zval_toc<Services>(ZEND_THIS);
	svc->clearDefer();
}

ZEND_METHOD(Wcc_Services, clearObjects)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	Services* svc = zval_toc<Services>(ZEND_THIS);
	svc->clearObjects();
}


PHP_MINIT_FUNCTION(wc_services_md)
{
	auto ce = register_class_Wcc_Services();

	Services::omg.classEntry(ce);

	STATE_INIT_ADD(SVC_data);
	
	return SUCCESS;
}

#endif
//wc_services.cpp