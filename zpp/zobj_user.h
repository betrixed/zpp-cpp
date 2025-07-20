#ifndef ZOBJ_USER_H
#define ZOBJ_USER_H


#ifndef ZVAL_MGR_H
#include "zval_mgr.h"
#endif

#ifndef ZSTR_USER_H
#include "zstr_user.h"
#endif

namespace zpp {

	class htab_mgr;
	class zobj_mgr;

	class zobj_user {
	protected:
		zend_object* obj_;

		friend class zobj_mgr;
		
	public:
		zobj_user() : obj_(nullptr) {}

		zobj_user(zend_object* rc) : obj_(rc) {}

		zobj_user(const zobj_user& rc) : obj_(rc.obj_) {}

		zobj_user(const zobj_mgr& rc);

		zobj_user(const zval_mgr& rc);

		zobj_user(const zval_user& rc);
		
		zobj_user(zval* zp);

		zobj_user(base_d* cobj); 
		
		zobj_mgr clone() const;

		const zobj_user& operator=(zend_object* rc);
		const zobj_user& operator=(zval* rc);
		//const zobj_user& operator=(const zval_mgr& rc);

		void return_zv(zval* ret) const;
		
		bool property_list(htab_mgr& mgr);

		//! Closures, isCallables
		zval_mgr callable();
		zval_mgr callable(zval* arg1);
		zval_mgr callable(zval* arg1, zval* arg2);

		

		//! get a property value
		zval_mgr  property(zstr_user key);
		zval* 	  property_get(zstr_user key, zval* ret);

		//! set a property value
		void      property(zstr_user key, zval_user value);
		void      property(zstr_user key, zstr_user value);
		
		bool      has_property(zstr_user name);
		void      unset_property(zstr_user name);
		//! property-values list
		htab_mgr  properties();

		bool isNull() const { return !(obj_); }
		bool ok() const { return (obj_); }

		zend_string* className();


		//! calling methods
		zval_mgr call(zstr_user method);

		zval_mgr call(zstr_user method, HashTable* args);

		zval_mgr call(zstr_user method, zval* arg1);

		zval_mgr call(zstr_user method, 
	        zval* arg1, zval* arg2);

		zval_mgr call(zstr_user method, 
	        zval* arg1, zval* arg2, zval* arg3);

		zval_mgr call(zstr_user method, 
	        zval* arg1, zval* arg2, zval* arg3, zval* arg4);

		bool instanceof(zend_class_entry *ce) const;
		bool method_exists(zstr_user method) const;
		bool isDateTime() const;

		
		zend_object* operator->() const { return obj_; }
		operator zend_object* () const { return (zend_object*) obj_; }
	};
};

//zobj_user.h
#endif