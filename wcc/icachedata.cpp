#ifndef ICACHE_DATA_CPP
#define ICACHE_DATA_CPP

#ifndef ICACHE_DATA_H
#include "icachedata.h"
#endif

#ifndef ICACHE_DATA_ARGINFO
#define ICACHE_DATA_ARGINFO
extern "C" {
	#include "stub/icachedata_arginfo.h"
};
#endif

namespace wcc {

using namespace zpp;

base_obj_mgr<ICacheData>  ICacheData::omg;

/**
 * Serialize keys
 */

icache_str IC_STR;

void
icache_str::init()
{
	expiry_key = "expiry";
	expire_key = "expire";
	prefix_key = "prefix";
	key_key = "key";
	data_key = "data";
	ttl_key = "ttl";
	stored_key = "stored";
	saved = "saved";
	class_key = "class";
	service_key = "service";

	cached = "cached";
	svc_cache = "svc_cache";
	options = "options";
	services = "services";
	ttl = "ttl";
}

void 
ICacheData::setStored()
{
	stored_ = time(nullptr);
	saved_ = true;
}

obj_rc 
ICacheData::new_ICacheData(str_ptr key, 
	val_ptr value, zend_long ttl)
{
	obj_rc result;

	result = ICacheData::omg.new_zobj();

	ICacheData* cobj = zobj_toc<ICacheData>(result);

	cobj->construct(key, value, ttl);

	return result;
}

void 
ICacheData::construct(str_ptr key, 
					val_ptr data, zend_long ttl)
{
	//showstr("key", key);
	//showmem("construct", data);
	//zend_printf("ttl %ld", ttl);

	key_ = key;
	data_ = data;
	ttl_ = ttl;
	stored_ = time(nullptr);
	saved_ = false;
}

void 
ICacheData::member_info(htab_rw s, bool dinfo)
{
	// make expiry at front of json
	s.set(IC_STR.expiry_key,  ttl_ + stored_);
	s.set(IC_STR.key_key, key_);
	s.set(IC_STR.data_key, data_);
	s.set(IC_STR.ttl_key, ttl_);
	s.set(IC_STR.stored_key, stored_);
	if (dinfo) {
		s.set(IC_STR.saved, saved_);
	}
}

htab_rc 
ICacheData::serialize()
{
	htab_rc s;

	member_info(s, false);

	return s;
}

void ICacheData::debug_info(htab_rw hw)
{
	member_info(hw, true);
}

void 
ICacheData::unserialize(htab_ptr s)
{

	//s.show_data("unserialize");

	key_ = s.get(IC_STR.key_key);
	data_ = s.get(IC_STR.data_key);
	ttl_ = val_ptr(s.get(IC_STR.ttl_key)).zlong();
	stored_ = val_ptr(s.get(IC_STR.stored_key)).zlong();
	saved_ = true;
}


}; // namespace wcc

ZEND_METHOD(Wcc_ICacheData, __construct)
{
	zend_string* skey;
	zval*        data;
	zend_long    ttl = 0;

	
	ZEND_PARSE_PARAMETERS_START(2, 3)
	Z_PARAM_STR(skey)
	Z_PARAM_ZVAL(data)
	Z_PARAM_OPTIONAL
	Z_PARAM_LONG(ttl)
	ZEND_PARSE_PARAMETERS_END();	

	ICacheData* cobj = zval_toc<ICacheData>(ZEND_THIS);
	val_rc zdata(data);

	cobj->construct(skey, zdata, ttl);
}


ZEND_METHOD(Wcc_ICacheData, __serialize)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();	

	ICacheData* cobj = zval_toc<ICacheData>(ZEND_THIS);

	htab_rc data = cobj->serialize();
	data.move_zv(return_value);

}

ZEND_METHOD(Wcc_ICacheData, __unserialize)
{
	zval* data;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ARRAY(data)
	ZEND_PARSE_PARAMETERS_END();	

	ICacheData* cobj = zval_toc<ICacheData>(ZEND_THIS);

	cobj->unserialize(data);
}

ZEND_METHOD(Wcc_ICacheData, getData)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();	

	ICacheData* cobj = zval_toc<ICacheData>(ZEND_THIS);
	val_ptr result = cobj->getData();
	result.copy_zv(return_value);
}

ZEND_METHOD(Wcc_ICacheData, getExpiry)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();	

	ICacheData* cobj = zval_toc<ICacheData>(ZEND_THIS);
	zend_long expiry = cobj->getExpiry();

	RETURN_LONG(expiry);
}

ZEND_METHOD(Wcc_ICacheData, getKey)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();	

	ICacheData* cobj = zval_toc<ICacheData>(ZEND_THIS);
	str_ptr result = cobj->getKey();
	result.copy_zv(return_value);
}

ZEND_METHOD(Wcc_ICacheData, getStored)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();	

	ICacheData* cobj = zval_toc<ICacheData>(ZEND_THIS);
	zend_long stored = cobj->getStored();

	RETURN_LONG(stored);
}

ZEND_METHOD(Wcc_ICacheData, getTTL)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();	

	ICacheData* cobj = zval_toc<ICacheData>(ZEND_THIS);
	zend_long ttl = cobj->getTTL();

	RETURN_LONG(ttl);
}

ZEND_METHOD(Wcc_ICacheData, isSaved)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();	

	ICacheData* cobj = zval_toc<ICacheData>(ZEND_THIS);
	bool b = cobj->isSaved();

	RETURN_BOOL(b);
}

ZEND_METHOD(Wcc_ICacheData, setStored)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();	

	ICacheData* cobj = zval_toc<ICacheData>(ZEND_THIS);
	cobj->setStored();
}

ZEND_METHOD(Wcc_ICacheData, update)
{
	zval* data;
	zend_long ttl = 0;

	ZEND_PARSE_PARAMETERS_START(2, 2)
	Z_PARAM_ZVAL(data)
	Z_PARAM_LONG(ttl)
	ZEND_PARSE_PARAMETERS_END();	

	ICacheData* cobj = zval_toc<ICacheData>(ZEND_THIS);
	val_rc dw(data);
	
	cobj->update(dw, ttl);
}

PHP_MINIT_FUNCTION(Wcc_ICacheData_reg)
{
	auto ce = register_class_Wcc_ICacheData();

	ICacheData::omg.classEntry(ce);

	return SUCCESS;
}

//icachedata.cpp
#endif