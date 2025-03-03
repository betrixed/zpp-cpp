#ifndef REFLECT_CACHE_CPP
#define REFLECT_CACHE_CPP

#ifndef REFLECT_CACHE_H
#include "reflect_cache.h"
#endif

#ifndef FN_CALL_H
#include "zpp/fn_call.h"
#endif

namespace wcc 
{
	using namespace zpp;

	base_obj_mgr<ReflectCache> ReflectCache::omg;


class ReflectCache_data : public state_init {
public:
	zstr_intern cache_key;
	zstr_intern construct_key;
	zstr_intern reflection_class;
	zstr_intern new_instance;
	zstr_intern new_instance_args;
	class_data  rfc_cdata;

	ReflectCache_data() 
	{

	}

	void init()
	{
		cache_key = zstr_intern("cache");
		construct_key = zstr_intern("__construct");
		reflection_class = zstr_intern("reflectionclass");
		new_instance = zstr_intern("newinstance");
		new_instance_args = zstr_intern("newinstanceargs");
		// presume reflectionclass is configured.
		//rfc_cdata.set(reflection_class); // this will segfault here
		//zend_printf("ReflectCache_data::init\n");
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

zobj_mgr 
ReflectCache::getReflectClass(zstr_user class_name)
{
	zobj_mgr result;

	//showstr("getReflectClass", class_name);

	htab_write cache(cache_);

	zval_user test(cache.get(class_name));

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
		
		zval_mgr crc = fn.call_fn();
		zval_user temp(crc);

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

zobj_mgr 
ReflectCache::newInstance(zstr_user class_name)
{
	zobj_mgr result;

	zobj_mgr rfc_obj = getReflectClass(class_name);

	if (rfc_obj.ok())
	{
		fn_call fn;

		fn.set_fci(rfc_obj, RFC_data.new_instance);

		zval_mgr nobj = fn.call_fn();
		zval_user temp(nobj);

		if (temp.isObject())
		{
			result = temp.zobject();
		}
	}
	return result;
}

zobj_mgr //static
ReflectCache::staticInstance(zstr_user class_name)
{
	auto rcobj =  ReflectCache::cpp();
	return rcobj->newInstance(class_name);
}

zobj_mgr //static
ReflectCache::staticInstanceArgs(zstr_user class_name, zval_user args)
{
	auto rcobj =  ReflectCache::cpp();
	return rcobj->newInstanceArgs(class_name, args);
}

zobj_mgr 
ReflectCache::newInstanceArgs(zstr_user class_name, zval_user args)
{
	zobj_mgr result;

	zobj_mgr rfc = getReflectClass(class_name);
	if (rfc.ok())
	{
		fn_call_args<1> fn;
		ZVAL_COPY_VALUE(fn.argsptr(), args);

		fn.set_fci(rfc, RFC_data.new_instance_args);
		zval_mgr recall(fn.call_fn());
		zval_user test(recall);

		if (test.isObject())
		{
			result = test.zobject();
		}
	}
	return result;
}


zobj_mgr 
ReflectCache::instance()
{
	Global gme = GLOBALS[ReflectCache::omg.class_name()];

	zobj_mgr result;

	if (gme.value().isNull()) {

		gme = ReflectCache::omg.new_zobj();
	}

	result = gme.value().zobject();

	return result;
}
	
ReflectCache* ReflectCache::cpp()
{
	zobj_user result(ReflectCache::instance());
	return zobj_toc<ReflectCache>(result);
}

void
ReflectCache::debug_info(htab_write hw)
{	
	hw.set(RFC_data.cache_key, cache_);
}


};//namespace wcc

using namespace wcc;

PHP_METHOD(Wcc_ReflectCache, instance)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	zobj_user result(ReflectCache::instance());
	result.return_zv(return_value);
}


PHP_METHOD(Wcc_ReflectCache, getReflect)
{
	zend_string* cname;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(cname)
	ZEND_PARSE_PARAMETERS_END();	

	auto rcobj = zval_toc<ReflectCache>(ZEND_THIS);

	zobj_mgr result = rcobj->getReflectClass(cname);
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

	zobj_mgr result = rcobj->newInstance(cname);
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

	zobj_mgr result = rcobj->newInstanceArgs(cname, args);
	result.move_zv(return_value);

}

PHP_METHOD(Wcc_ReflectCache, staticInstance)
{
	zend_string* cname;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(cname)
	ZEND_PARSE_PARAMETERS_END();

	zobj_mgr result = ReflectCache::staticInstance(cname);

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

	zobj_mgr result = ReflectCache::staticInstanceArgs(cname, args);
	result.move_zv(return_value);
}

PHP_METHOD(Wcc_ReflectCache, getrc)
{
	zend_string* cname;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(cname)
	ZEND_PARSE_PARAMETERS_END();	

	auto rcobj =  ReflectCache::cpp();
	zobj_mgr result = rcobj->getReflectClass(cname);
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

	return SUCCESS;
}


#endif
//reflect_cache.cpp