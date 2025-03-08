#ifndef WCC_SERVICES_CPP
#define WCC_SERVICES_CPP

#ifndef WCC_SERVICES_H
#include "services.h"
#endif

#ifndef FN_CALL_H
#include "zpp/fn_call.h"
#endif

#ifndef GLOBALS_H
#include "globals.h"
#endif

#ifndef REFLECT_CACHE_H
#include "reflect_cache.h"
#endif

namespace wcc 
{
	using namespace zpp;
	

	base_obj_mgr<Services> Services::omg;


	class Services_data : public state_init {
	public:
		zstr_intern active;
		zstr_intern defer;
		zstr_intern instances;
		zstr_intern throw_fail;
		zstr_intern defer_ct;

		zobj_mgr	g_services;

		Services_data() : state_init() {}

		virtual void init() 
		{
			
			 active = zstr_intern("active");
			
			 defer = zstr_intern("defer");
			
			 instances = zstr_intern("instances");
			
			 throw_fail = zstr_intern("throw_fail");
			
			 defer_ct = zstr_intern("defer_ct");
		}

		virtual void init_req()
		{
			g_services = Services::omg.new_zobj();
		}

		virtual void end_req()
		{
			#ifdef DEBUG_EXTRA
				zend_printf("end_req services");
			#endif
			g_services.init();
		}
	};

	Services_data SVC_data;

zval_mgr 
Services::call_value(zobj_user callme)
{
	zval_mgr self(this->zobj());
	// 1 argument
	//showmem("self svc", self);
	return callme.callable(self);
}

Services::~Services()
{
#ifdef DEBUG_EXTRA
	showarray("instances_", instances_);
	showarray("defer_", defer_);
	showarray("active_", active_);
#endif
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

zval_mgr  
Services::activate(zstr_user key)
{

	zval_mgr result;
	//zend_printf("activate %s\n", ZSTR_VAL(key));

	htab_read defer(defer_);

	zval_user test;
	if (!defer.try_fetch(key, test))
	{
		if (throw_fail_)
		{
			zend_throw_error(zend_ce_error, "Services activate key not found: %s", key.data());
		}
		return result;
	}

	if (defer_ct_ > 3)
	{
		defer_ct_ = 0;
		zend_throw_error(zend_ce_error, "Services recursion limit hit: %s", key.data());
		return result;
	}

	defer_ct_++;

	if (test.isCallable())
	{	
		zobj_user callme = test.zobject();

		zval_mgr result2 = call_value(callme);

		htab_write(active_).set(key, result2);

		result = get(key);
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
	zobj_user sv = SVC_data.g_services;

	//showmem("instance", sv);
	return zobj_toc<Services>(sv);
}

zobj_user
Services::instance()
{
	return SVC_data.g_services;
	/*
	zobj_user result;

	Global gme = GLOBALS[Services::omg.class_name()];
	
	zval_user val = gme.value(); // good for while gme is around
	showmem("read instance", val);

	if (val.isNull()) {
		zobj_mgr obj = Services::omg.new_zobj();
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
zobj_user 
Services::setOne(zstr_user key, zobj_user obj)
{
	Services* self = Services::cpp_global();
	return self->setObject(obj,key);
}

/* static */
zobj_user  
Services::getOne(zstr_user key)
{

	Services* self = Services::cpp_global();

	zobj_user result = self->getObject(key);

	if (result.ok())
	{
		return result;
	}
	return self->newInstance(key);
}

// static
zval_mgr  
Services::service(zstr_user key)
{
	Services* self = Services::cpp_global();
	return self->get(key);
}

// static
zval_mgr  
Services::service(const std::string_view& key)
{
	zval_mgr result;

	auto slen = key.size();
	if (slen)
	{
		zstr_temp skey(key.data(), slen);
		result = Services::service(skey);
	}
	return result;
}

zobj_user
Services::newInstance(zstr_user name_class)
{

	ReflectCache* rc = ReflectCache::cpp();

	//showstr("newInstance of ", name_class);

	zobj_mgr obj = rc->newInstance(name_class);
	
	if (obj.ok())
	{
		//showarray("instances", instances_);
		htab_write(instances_).set(name_class, obj);
	}
	else {
		zend_throw_error(zend_ce_error, "newInstance failed for %s", name_class.data());
	}

	return obj;
}

zobj_user
Services::getObject(zstr_user key)
{
	zobj_user result;

	zval_user test;

	if (htab_write(instances_).try_fetch(key,test))
	{
		result = test.zobject();
	}

	return result;
}

zobj_user 
Services::setObject(zobj_user obj, zstr_user key)
{
	if (key.isNull()) {
		// get class name of object
		key = obj.className();
	}

	htab_write(instances_).set(key, obj);
	return obj;
}


bool Services::isActive(zstr_user name)
{

	return htab_read(active_).has_key(name);
}

bool Services::has(zstr_user name)
{

	return (htab_read(active_).has_key(name) || htab_read(defer_).has_key(name));
}


void Services::setDefer(zstr_user name, zval_user value)
{
	htab_write(defer_).set(name, value);
}

void  Services::set(zstr_user name, zval_user value)
{
	htab_write(active_).set(name, value);
}

zval_mgr  
Services::get(zstr_user name)
{
	zval_mgr result;
	
	zval_user value;

	//showstr("services::get", name);

	if (!htab_read(active_).try_fetch(name, value))
	{
		result = activate(name);
		return  result;
	}
	//showmem("got active ", value);
	int ztype = value.ztype();

	if (value.isCallable())
	{
		zobj_user callme = value.zobject();
		//showobj("callable ", callme);
		result = call_value(callme);
	}
	else {
		result = value;
	}
	return result;
}


void  
Services::unset(zstr_user name)
{
	htab_write(active_).unset(name);
}


void  
Services::setThrowFail(bool value)
{
	throw_fail_ = value;
}

void Services::debug_info(htab_write info)
{
	//showarray("debug_info-0", info);

	//base_d::debug_info(info);

	//showarray("debug_info-1", info);
	//showarray("key active_", active_);
	info.set(SVC_data.active, active_);
	info.set(SVC_data.defer, defer_);
	info.set(SVC_data.instances, instances_);
	info.set(SVC_data.throw_fail, throw_fail_);
	info.set(SVC_data.defer_ct,   defer_ct_);
	//showarray("debug_info", info);
}

}; // namespace wcc

using namespace wcc;

ZEND_METHOD(Wcc_Services, getOne)
{
	zend_string* skey;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(skey)
	ZEND_PARSE_PARAMETERS_END();

	//showstr("getOne call", skey);

	zobj_user result = Services::getOne(skey);
	result.return_zv(return_value);
}

ZEND_METHOD(Wcc_Services, instance)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	zobj_user result = Services::instance();
	result.return_zv(return_value);
}

ZEND_METHOD(Wcc_Services, service)
{
	zend_string* skey;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(skey)
	ZEND_PARSE_PARAMETERS_END();

	zval_mgr result = Services::service(skey);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Services, setOne)
{
	zend_string* skey;
	zval* 		 obj;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(skey)
		Z_PARAM_OBJECT(obj)
	ZEND_PARSE_PARAMETERS_END();

	Services* svc = zval_toc<Services>(ZEND_THIS);

	zval_user test(obj);
	zobj_user result = svc->setOne(skey, test.zobject());

	result.return_zv(return_value);
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
	zend_string* skey;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(skey)
	ZEND_PARSE_PARAMETERS_END();

	Services* svc = zval_toc<Services>(ZEND_THIS);

	zval_mgr result = svc->get(skey);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_Services, getObject)
{
	zend_string* skey;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(skey)
	ZEND_PARSE_PARAMETERS_END();

	Services* svc = zval_toc<Services>(ZEND_THIS);
	zobj_user result = svc->getObject(skey);
	result.return_zv(return_value);
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
	zend_string* skey;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(skey)
	ZEND_PARSE_PARAMETERS_END();

	Services* svc = zval_toc<Services>(ZEND_THIS);
	zobj_user result = svc->newInstance(skey);
	result.return_zv(return_value);
}


ZEND_METHOD(Wcc_Services, set)
{
	zend_string* skey;
	zval*        svalue;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR(skey)
		Z_PARAM_ZVAL(svalue)
	ZEND_PARSE_PARAMETERS_END();

	Services* svc = zval_toc<Services>(ZEND_THIS);
	svc->set(skey, svalue);

}

ZEND_METHOD(Wcc_Services, setObject)
{
	zval* 		 obj;
	zend_string* skey = nullptr;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_OBJECT(obj)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(skey)
	ZEND_PARSE_PARAMETERS_END();

	Services* svc = zval_toc<Services>(ZEND_THIS);

	zval_user test(obj);
	zobj_user result = svc->setObject(test.zobject(), skey);
	result.return_zv(return_value);
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

	return SUCCESS;
}

#endif
//wc_services.cpp