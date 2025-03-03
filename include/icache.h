#ifndef ICACHE_H
#define ICACHE_H


#ifndef WC_BASE_H
#include "wc_base.h"
#endif



namespace wcc {

class  Wcc_ICache : public base_d {
protected:
	htab_own cached_;
	htab_own svc_cache_;
	zstr_own prefix_;
	htab_own options_;
	zobj_own services_;
	zend_long ttl_;
public:

	static const char* class_name;

	static zval_own make_cache( zval_ptr options, zval_ptr services);

	void __construct(zval_ptr options, zval_ptr services);

	void addLocal(zval_own& pkg);

	bool clear();

	bool clearPrefix(zend_string* prefix);

	bool deleteKey(zend_string* key);

	htab_own getExpired();
	
	int  deleteExpired();

	bool deleteMultiple(zval_own& keys);

	zval_own get(zend_string* key, zval_own& noval);

	zval_own getCached(zend_string* key);

	zval_own getData(zend_string* key);

	zval_own getMultiple(zval_own& keys, zval_own& noval);

	zstr_own getPrefix()
	{
		return prefix_;
	}

	zval_own getService(zend_string* key);

	zend_long getTTL() {
		return ttl_;
	}

	zval_own getUnsaved();

	zval_own getOption(zend_string* key);

	void setOption(zend_string* key, zval_ptr value);
	
	bool set(zend_string* key, zval_own& data, zend_long ttl);

	zval_own setCached(zend_string* key, zval_own& data, zend_long ttl);

	bool setMultiple(zval_own& values, zend_long ttl);

	void setServices(zval_own& svc);

	void setTTL(zend_long ttl)
	{
		ttl_ = ttl;
	}

	virtual void debug_info(HashTable *ht);


};

	typedef base_obj_mgr<Wcc_ICache>  Wcc_ICache_Mgr;

	extern Wcc_ICache_Mgr icache_mgr;

}; // namespace

//icache.h
#endif