#ifndef ICACHE_H
#define ICACHE_H


#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif



namespace wcc {

class  ICache : public base_d {
protected:
	htab_mgr  cached_;
	htab_mgr  svc_cache_;
	htab_mgr  options_;
	zstr_mgr  prefix_;
	zobj_mgr  services_;
	zend_long ttl_;
public:

	static base_obj_mgr<ICache> omg;

	static zobj_mgr make_cache( zval_user options, zval_user services);

	void __construct(zval_user options, zval_user services);

	void addLocal(zval_user pkg);

	bool clear();

	bool clearPrefix(zstr_user prefix);

	bool deleteKey(zstr_user key);

	htab_mgr getExpired();
	
	int  deleteExpired();

	bool deleteMultiple(htab_read keys);

	zval_mgr get(zstr_user key, zval_user noval);

	zval_mgr getCached(zstr_user key);

	zval_mgr getData(zstr_user key);

	zval_mgr getMultiple(htab_read keys, zval_user noval);

	zstr_user getPrefix()
	{
		return prefix_;
	}

	zval_mgr getService(zstr_user key);

	zend_long getTTL() {
		return ttl_;
	}

	htab_mgr getUnsaved();

	zval_mgr getOption(zstr_user key);

	void setOption(zstr_user key, zval_user value);
	
	bool set(zstr_user key, zval_user data, zend_long ttl);

	zobj_mgr setCached(zstr_user key, zval_user data, zend_long ttl);

	bool setMultiple(zval_user values, zend_long ttl);

	void setServices(zval_user svc);

	void setTTL(zend_long ttl)
	{
		ttl_ = ttl;
	}

 	void debug_info(htab_write s) override;


};


}; // namespace

//icache.h
#endif