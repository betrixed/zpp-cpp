#ifndef ICACHE_DATA_H
#define ICACHE_DATA_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif


namespace wcc {

	class  ICacheData : public base_d {
	protected:
		zstr_mgr  key_;
		zval_mgr  data_;
		int       ttl_;
		int       stored_;
		bool      saved_;

		void member_info(htab_write s, bool store);
	public:

		static base_obj_mgr<ICacheData> omg;

		static zobj_mgr 
		new_ICacheData(zstr_user key, zval_user value, zend_long ttl);

		void construct(zstr_user key, 
			zval_user value, zend_long ttl);

		zstr_user getKey() {
			return key_;
		}

		zval_user getData() {
			return data_;
		}

		int getExpiry() const {
			return ttl_ + stored_;
		}

		int getTTL() const {
			return ttl_;
		}

		int getStored() const {
			return stored_;
		}

		void update(zval_user data, int ttl)
		{
			data_ = data;
			ttl_ = ttl;
			saved_ = false;
		}

		bool isSaved() const {
			return saved_;
		}

		void setStored();

		htab_mgr serialize();

		void unserialize(htab_read uht);

		virtual void debug_info(htab_write hw);

#ifndef BASE_ZOBJPTR
	VIRTUAL_ZOBJPTR
#endif
	
};

class icache_str : public state_init {
public:
	zstr_intern expiry_key;
	zstr_intern expire_key;
	zstr_intern prefix_key;
	zstr_intern key_key;
	zstr_intern data_key;
	
	zstr_intern ttl_key;
	zstr_intern stored_key;
	zstr_intern saved;
	zstr_intern class_key;
	zstr_intern service_key;

	zstr_intern cached;
	zstr_intern svc_cache;
	zstr_intern options;
	zstr_intern services;
	zstr_intern ttl;
    
    virtual void init();
};


extern icache_str IC_STR;


};//namespace
//icachedata.h
#endif