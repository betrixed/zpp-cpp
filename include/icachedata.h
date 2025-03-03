#ifndef ICACHE_DATA_H
#define ICACHE_DATA_H

#ifndef WC_BASE_H
#include "wc_base.h"
#endif


namespace wcc {

	class  Wcc_ICacheData : public base_d {
	protected:
		zstr_own  key_;
		zval_own  data_;
		int       ttl_;
		int       stored_;
		bool      saved_;

		void member_info(htab_ptr& s, bool store);
	public:

		static const char* class_name;

		static zval_own new_ICacheData(zend_string* key, 
			zval_own& value, zend_long ttl);

		void construct(zend_string* key, 
			zval_own& value, zend_long ttl);

		const zstr_own& getKey() {
			return key_;
		}

		zval_own& getData() {
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

		void update(zval_own& data, int ttl)
		{
			data_ = data;
			ttl_ = ttl;
			saved_ = false;
		}

		bool isSaved() const {
			return saved_;
		}

		void setStored();

		zval_own serialize();

		void unserialize(zval_ptr uht);

		virtual void debug_info(HashTable *ht);
};



typedef base_obj_mgr<Wcc_ICacheData>  Wcc_ICacheData_Mgr;

extern Wcc_ICacheData_Mgr icachedata_mgr;

};//namespace
//icachedata.h
#endif