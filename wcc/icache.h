#ifndef ICACHE_H
#define ICACHE_H


#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

#ifndef WCC_SERVICES_H
#include "services.h"
#endif


namespace wcc {

	using namespace zpp;
	
class  ICache : public base_d {
protected:
	htab_rc  cached_;
	htab_rc  svc_cache_;
	htab_rc  options_;
	str_rc  prefix_;
	obj_rc  services_;
	zend_long ttl_;
public:

	static base_obj_mgr<ICache> omg;

	static obj_rc make_cache( val_ptr options, val_ptr services);

	void construct(val_ptr options, val_ptr services);

	void addLocal(val_ptr pkg);

	bool clear();

	bool clearPrefix(str_ptr prefix);

	bool deleteKey(str_ptr key);

	htab_rc getExpired();
	
	int  deleteExpired();

	bool deleteMultiple(htab_ptr keys);

	val_rc get(str_ptr key, val_ptr noval = val_ptr());

	val_rc getCached(str_ptr key);

	val_rc getData(str_ptr key);

	val_rc getMultiple(htab_ptr keys, val_ptr noval);

	str_ptr getPrefix()
	{
		return prefix_;
	}

	val_rc getService(str_ptr key);

	zend_long getTTL() {
		return ttl_;
	}

	htab_rc getUnsaved();

	val_rc getOption(str_ptr key);

	void setOption(str_ptr key, val_ptr value);
	
	bool set(str_ptr key, val_ptr data, zend_long ttl = 0);

	obj_rc setCached(str_ptr key, val_ptr data, zend_long ttl = 0);

	bool setMultiple(val_ptr values, zend_long ttl = 0);

	void setServices(val_ptr svc);

	void setTTL(zend_long ttl)
	{
		ttl_ = ttl;
	}

 	virtual void debug_info(htab_rw s);
 	
	VIRTUAL_ZOBJPTR


};


}; // namespace

//icache.h
#endif