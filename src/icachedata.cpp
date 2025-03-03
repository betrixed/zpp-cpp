#ifndef ICACHE_DATA_CPP
#define ICACHE_DATA_CPP

#ifndef ICACHE_DATA_H
#include "icachedata.h"
#endif

#ifndef ICACHE_ARGINFO_H
#define ICACHE_ARGINFO_H

extern "C" {
	#include "icache_arginfo.h"
};
#endif

namespace wcc {

using namespace zpp;

	Wcc_ICacheData_Mgr icachedata_mgr;

	const char* Wcc_ICacheData::class_name = "Wcc\\ICacheData";


/**
 * Serialize keys
 */

class icache_str : public state_init {
public:
	zstr_perm expiry_key;
	zstr_perm key_key;
	zstr_perm data_key;
	zstr_perm ttl_key;
	zstr_perm stored_key;
    
    virtual void init();
};


icache_str IC_STR;

void
icache_str::init()
{
	expiry_key = zstr_perm("expiry");
	key_key = zstr_perm("key");
	data_key = zstr_perm("data");
	ttl_key = zstr_perm("ttl");
	stored_key = zstr_perm("stored");
}

void 
Wcc_ICacheData::setStored()
{
	stored_ = time(nullptr);
	saved_ = true;
}

zval_own 
Wcc_ICacheData::new_ICacheData(zend_string* key, 
	zval_own& value, zend_long ttl)
{

	Wcc_ICacheData* cobj = icachedata_mgr.make_new();
	cobj->construct(key, value, ttl);

	zval_own result;
	result.set(cobj->zobj(), false);
	return result;
}

void 
Wcc_ICacheData::construct(zend_string* key, 
					zval_own& data, zend_long ttl)
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
Wcc_ICacheData::member_info(htab_ptr& s, bool store)
{
	// make expiry at front of json
	s.set(IC_STR.expiry_key,  ttl_ + stored_);
	s.set(IC_STR.key_key, key_);
	s.set(IC_STR.data_key, data_);
	s.set(IC_STR.ttl_key, ttl_);
	s.set(IC_STR.stored_key, stored_);
	if (!store) {
		s.set("saved", saved_);
	}
}

zval_own 
Wcc_ICacheData::serialize()
{
	htab_own s;

	member_info(s, false);

	return std::move(s);
}

void Wcc_ICacheData::debug_info(HashTable *ht)
{
	htab_ptr s(ht);
	member_info(s, true);
}

void 
Wcc_ICacheData::unserialize(zval_ptr uht)
{
	htab_ptr s(uht);

	//s.show_data("unserialize");

	key_ = s[IC_STR.key_key];
	data_ = s.get(IC_STR.data_key);
	ttl_ = zval_ptr(s.get(IC_STR.ttl_key)).zlong();
	stored_ = zval_ptr(s.get(IC_STR.stored_key)).zlong();
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

	Wcc_ICacheData* cobj = zval_toc<Wcc_ICacheData>(ZEND_THIS);
	zval_own zdata(data);

	cobj->construct(skey, zdata, ttl);
}


ZEND_METHOD(Wcc_ICacheData, __serialize)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();	

	Wcc_ICacheData* cobj = zval_toc<Wcc_ICacheData>(ZEND_THIS);

	zval_own data = cobj->serialize();
	data.move_zv(return_value);

}

ZEND_METHOD(Wcc_ICacheData, __unserialize)
{
	zval* data;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ARRAY(data)
	ZEND_PARSE_PARAMETERS_END();	

	Wcc_ICacheData* cobj = zval_toc<Wcc_ICacheData>(ZEND_THIS);

	cobj->unserialize(data);
}

ZEND_METHOD(Wcc_ICacheData, getData)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();	

	Wcc_ICacheData* cobj = zval_toc<Wcc_ICacheData>(ZEND_THIS);
	zval_own result = cobj->getData();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_ICacheData, getExpiry)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();	

	Wcc_ICacheData* cobj = zval_toc<Wcc_ICacheData>(ZEND_THIS);
	zend_long expiry = cobj->getExpiry();

	RETURN_LONG(expiry);
}

ZEND_METHOD(Wcc_ICacheData, getKey)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();	

	Wcc_ICacheData* cobj = zval_toc<Wcc_ICacheData>(ZEND_THIS);
	zstr_own result = cobj->getKey();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_ICacheData, getStored)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();	

	Wcc_ICacheData* cobj = zval_toc<Wcc_ICacheData>(ZEND_THIS);
	zend_long stored = cobj->getStored();

	RETURN_LONG(stored);
}

ZEND_METHOD(Wcc_ICacheData, getTTL)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();	

	Wcc_ICacheData* cobj = zval_toc<Wcc_ICacheData>(ZEND_THIS);
	zend_long ttl = cobj->getTTL();

	RETURN_LONG(ttl);
}

ZEND_METHOD(Wcc_ICacheData, isSaved)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();	

	Wcc_ICacheData* cobj = zval_toc<Wcc_ICacheData>(ZEND_THIS);
	bool b = cobj->isSaved();

	RETURN_BOOL(b);
}

ZEND_METHOD(Wcc_ICacheData, setStored)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();	

	Wcc_ICacheData* cobj = zval_toc<Wcc_ICacheData>(ZEND_THIS);
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

	Wcc_ICacheData* cobj = zval_toc<Wcc_ICacheData>(ZEND_THIS);
	zval_own dw(data);
	
	cobj->update(dw, ttl);
}

PHP_MINIT_FUNCTION(Wcc_ICacheData_reg)
{
	auto ce = register_class_Wcc_ICacheData();

	icachedata_mgr.classEntry(ce);

	return SUCCESS;
}

//icachedata.cpp
#endif