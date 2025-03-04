#ifndef ICACHE_DATA_CPP
#define ICACHE_DATA_CPP

#ifndef ICACHE_DATA_H
#include "icachedata.h"
#endif

#ifndef ICACHE_ARGINFO_H
#define ICACHE_ARGINFO_H

extern "C" {
	#include "stub/icache_arginfo.h"
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
	expiry_key = zstr_intern("expiry");
	expire_key = zstr_intern("expire");
	prefix_key = zstr_intern("prefix");
	key_key = zstr_intern("key");
	data_key = zstr_intern("data");
	ttl_key = zstr_intern("ttl");
	stored_key = zstr_intern("stored");
	saved = zstr_intern("saved");
	class_key = zstr_intern("class");
	service_key = zstr_intern("service");

	cached = zstr_intern("cached");
	svc_cache = zstr_intern("svc_cache");
	options = zstr_intern("options");
	services = zstr_intern("services");
	ttl = zstr_intern("ttl");
}

void 
ICacheData::setStored()
{
	stored_ = time(nullptr);
	saved_ = true;
}

zobj_mgr 
ICacheData::new_ICacheData(zstr_user key, 
	zval_user value, zend_long ttl)
{

	ICacheData* cobj = ICacheData::omg.make_new();
	cobj->construct(key, value, ttl);

	zobj_mgr result;
	result.adopt(cobj);
	return result;
}

void 
ICacheData::construct(zstr_user key, 
					zval_user data, zend_long ttl)
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
ICacheData::member_info(htab_write s, bool store)
{
	// make expiry at front of json
	s.set(IC_STR.expiry_key,  ttl_ + stored_);
	s.set(IC_STR.key_key, key_);
	s.set(IC_STR.data_key, data_);
	s.set(IC_STR.ttl_key, ttl_);
	s.set(IC_STR.stored_key, stored_);
	if (!store) {
		s.set(IC_STR.saved, saved_);
	}
}

htab_mgr 
ICacheData::serialize()
{
	htab_init s;

	member_info(s, false);

	return s;
}

void ICacheData::debug_info(htab_write hw)
{
	member_info(hw, true);
}

void 
ICacheData::unserialize(htab_read s)
{

	//s.show_data("unserialize");

	key_ = s.get(IC_STR.key_key);
	data_ = s.get(IC_STR.data_key);
	ttl_ = zval_user(s.get(IC_STR.ttl_key)).zlong();
	stored_ = zval_user(s.get(IC_STR.stored_key)).zlong();
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
	zval_mgr zdata(data);

	cobj->construct(skey, zdata, ttl);
}


ZEND_METHOD(Wcc_ICacheData, __serialize)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();	

	ICacheData* cobj = zval_toc<ICacheData>(ZEND_THIS);

	htab_mgr data = cobj->serialize();
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
	zval_user result = cobj->getData();
	result.return_zv(return_value);
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
	zstr_user result = cobj->getKey();
	result.return_zv(return_value);
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
	zval_mgr dw(data);
	
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