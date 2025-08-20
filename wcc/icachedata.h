#ifndef ICACHE_DATA_H
#define ICACHE_DATA_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif


namespace wcc {

	class  ICacheData : public base_d {
	protected:
		str_rc  key_;
		val_rc  data_;
		int       ttl_;
		int       stored_;
		bool      saved_;

		void member_info(htab_rw s, bool dinfo);
	public:

		static base_obj_mgr<ICacheData> omg;

		static obj_rc 
		new_ICacheData(str_ptr key, val_ptr value, zend_long ttl);

		void construct(str_ptr key, 
			val_ptr value, zend_long ttl);

		str_ptr getKey() {
			return key_;
		}

		val_ptr getData() {
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

		void update(val_ptr data, int ttl)
		{
			data_ = data;
			ttl_ = ttl;
			saved_ = false;
		}

		bool isSaved() const {
			return saved_;
		}

		void setStored();

		htab_rc serialize();

		void unserialize(htab_ptr uht);

		virtual void debug_info(htab_rw hw);

		VIRTUAL_ZOBJPTR
	
};

class icache_str : public state_init {
public:
	str_intern expiry_key;
	str_intern expire_key;
	str_intern prefix_key;
	str_intern key_key;
	str_intern data_key;
	
	str_intern ttl_key;
	str_intern stored_key;
	str_intern saved;
	str_intern class_key;
	str_intern service_key;

	str_intern cached;
	str_intern svc_cache;
	str_intern options;
	str_intern services;
	str_intern ttl;
    
    virtual void init();
};


extern icache_str IC_STR;


};//namespace
//icachedata.h
#endif