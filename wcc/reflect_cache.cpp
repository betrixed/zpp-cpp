#ifndef REFLECT_CACHE_CPP
#define REFLECT_CACHE_CPP

#ifndef REFLECT_CACHE_H
#include "reflect_cache.h"
#endif

#ifndef FN_CALL_H
#include "zpp/fn_call.h"
#endif

#ifndef REFLECT_CACHE_ARGINFO
#define REFLECT_CACHE_ARGINFO
extern "C" {
	#include "stub/reflectcache_arginfo.h"	
};
#endif

namespace wcc 
{
	using namespace zpp;

	base_obj_mgr<ReflectCache> ReflectCache::omg;

	thread_local obj_rc    g_reflect_cache;
	
class ReflectCache_data : public state_init {
public:
	str_intern cache_key;
	str_intern construct_key;
	str_intern reflection_class;
	str_intern new_instance;
	str_intern new_instance_args;
	class_data  rfc_cdata;

	
	ReflectCache_data() 
	{

	}

	virtual void init()
	{
		cache_key = "cache";
		construct_key = "__construct";
		reflection_class = "reflectionclass";
		new_instance = "newinstance";
		new_instance_args = "newinstanceargs";
		// presume reflectionclass is configured.
		//rfc_cdata.set(reflection_class); // this will segfault here
		//zend_printf("ReflectCache_data::init\n");
	}
	virtual void init_req()
	{
		g_reflect_cache = ReflectCache::omg.new_zobj();
	}

	virtual void end_req()
	{
		//zend_printf("ReflectCache_data  ");
		//showobj("end_req", g_reflect_cache);
		g_reflect_cache.init();

	}
};

ReflectCache_data RFC_data;

ReflectCache::ReflectCache()
{
}

void
ReflectCache::clear()
{
	cache_.reset();
}

obj_rc 
ReflectCache::getReflectClass(str_ptr class_name)
{
	obj_rc result;

	//showstr("getReflectClass", class_name);

	htab_rw cache(cache_);

	val_ptr test(cache.get(class_name));

	result = test.zobject();

	if (result.ok())
	{
		return result;
	}

	if (!RFC_data.rfc_cdata.ok())
	{
		RFC_data.rfc_cdata.set(RFC_data.reflection_class);
	}

	if (RFC_data.rfc_cdata.new_object(result))
	{
		//showobj("new_object",result);
		fn_call_args<1> fn;
		ZVAL_STR(fn.argsptr(), class_name);
		fn.set_fci(result, RFC_data.construct_key);
		
		val_rc crc = fn.call_fn();
		val_ptr temp(crc);

		if (temp.isObject())
		{
			cache.set(class_name, crc);
			result = temp.zobject();
		}
	}
	else {
		zend_throw_error(zend_ce_error,"new object failed for %s", class_name.data());
	}
	//showobj("getReflectClass",result);
	return result;

}

obj_rc 
ReflectCache::newInstance(str_ptr class_name)
{
	obj_rc result;

	obj_rc rfc_obj = getReflectClass(class_name);

	if (rfc_obj.ok())
	{
		fn_call fn(RFC_data.new_instance, rfc_obj);
		fn_result newi(fn);
		result = newi.obj();
	}
	return result;
}

obj_rc //static
ReflectCache::staticInstance(str_ptr class_name)
{
	auto rcobj =  ReflectCache::cpp();
	return rcobj->newInstance(class_name);
}

obj_rc //static
ReflectCache::staticInstanceArgs(str_ptr class_name, htab_ptr args)
{
	auto rcobj =  ReflectCache::cpp();
	return rcobj->newInstanceArgs(class_name, args);
}

obj_rc 
ReflectCache::newInstanceArgs(str_ptr class_name, htab_ptr args)
{
	obj_rc result;

	obj_rc rfc = getReflectClass(class_name);
	if (rfc.ok())
	{
		//showobj("RFC", rfc);

		fn_call_args<1> fn;
		//ZVAL_ARR(fn.argsptr(), args);
		val_ptr::array_bind(fn.argsptr(), args);
		
		//showstr("fn name", RFC_data.new_instance_args);
		
		fn.set_fci(rfc, RFC_data.new_instance_args);
		val_rc recall = fn.call_fn();
		val_ptr test(recall);

		if (test.isObject())
		{
			result = test.zobject();
			//showmem("test", test);
		}
		else {
			//showmem("recall", recall);
			zend_throw_error(zend_ce_error, "newInstanceArgs fail for %s", class_name.data());
		}
	}
	else {
		zend_throw_error(zend_ce_error, "No reflection class for %s", class_name.data());
	}
	return result;
}


obj_ptr
ReflectCache::instance()
{
	return g_reflect_cache;
	/*
	Global gme = GLOBALS[ReflectCache::omg.class_name()];

	obj_rc result;

	if (gme.value().isNull()) {

		gme = ReflectCache::omg.new_zobj();
	}

	result = gme.value().zobject();

	return result;
	*/
}
	
ReflectCache* ReflectCache::cpp()
{
	return zobj_toc<ReflectCache>(g_reflect_cache);
}

void
ReflectCache::debug_info(htab_rw hw)
{	
	hw.set(RFC_data.cache_key, cache_);
}


};//namespace wcc

using namespace wcc;

PHP_METHOD(Wcc_ReflectCache, instance)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	obj_ptr result(ReflectCache::instance());
	result.copy_zv(return_value);
}


PHP_METHOD(Wcc_ReflectCache, getReflect)
{
	zend_string* cname;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(cname)
	ZEND_PARSE_PARAMETERS_END();	

	auto rcobj = zval_toc<ReflectCache>(ZEND_THIS);

	obj_rc result = rcobj->getReflectClass(cname);
	result.move_zv(return_value);

	//showmem("getReflect", return_value);
}

PHP_METHOD(Wcc_ReflectCache, newInstance)
{
	zend_string* cname;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(cname)
	ZEND_PARSE_PARAMETERS_END();	

	auto rcobj = zval_toc<ReflectCache>(ZEND_THIS);

	obj_rc result = rcobj->newInstance(cname);
	result.move_zv(return_value);
}

PHP_METHOD(Wcc_ReflectCache, newInstanceArgs)
{
	zend_string* cname;
	zval*        args;

	ZEND_PARSE_PARAMETERS_START(2, 2)
	Z_PARAM_STR(cname)
	Z_PARAM_ARRAY(args)
	ZEND_PARSE_PARAMETERS_END();	

	auto rcobj = zval_toc<ReflectCache>(ZEND_THIS);

	obj_rc result = rcobj->newInstanceArgs(cname, args);
	result.move_zv(return_value);

}

PHP_METHOD(Wcc_ReflectCache, staticInstance)
{
	zend_string* cname;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(cname)
	ZEND_PARSE_PARAMETERS_END();

	obj_rc result = ReflectCache::staticInstance(cname);

	result.move_zv(return_value);

}

PHP_METHOD(Wcc_ReflectCache, staticInstanceArgs)
{
	zend_string* cname;
	zval*        args;
	
	ZEND_PARSE_PARAMETERS_START(2, 2)
	Z_PARAM_STR(cname)
	Z_PARAM_ARRAY(args)
	ZEND_PARSE_PARAMETERS_END();	

	obj_rc result = ReflectCache::staticInstanceArgs(cname, args);
	result.move_zv(return_value);
}

PHP_METHOD(Wcc_ReflectCache, getrc)
{
	zend_string* cname;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(cname)
	ZEND_PARSE_PARAMETERS_END();	

	auto rcobj =  ReflectCache::cpp();
	obj_rc result = rcobj->getReflectClass(cname);
	result.move_zv(return_value);

}


PHP_METHOD(Wcc_ReflectCache, clear)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();	

	auto rcobj =  ReflectCache::cpp();
	rcobj->clear();
}


PHP_MINIT_FUNCTION(Wcc_ReflectCache)
{
	auto ce = register_class_Wcc_ReflectCache();

	ReflectCache::omg.classEntry(ce);

	STATE_INIT_ADD(RFC_data)
	
	return SUCCESS;
}


#endif
//reflect_cache.cpp