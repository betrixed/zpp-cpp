#ifndef ZOBJ_USER_H
#define ZOBJ_USER_H


#ifndef ZVAL_MGR_H
#include "val_rc.h"
#endif

#ifndef ZSTR_USER_H
#include "str_ptr.h"
#endif

namespace zpp {

	class htab_rc;
	class obj_rc;

	class obj_ptr {
	protected:
		zend_object* obj_;

		friend class obj_rc;
		
	public:
		obj_ptr() : obj_(nullptr) {}

		obj_ptr(zend_object* rc) : obj_(rc) {}

		obj_ptr(const obj_ptr& rc) : obj_(rc.obj_) {}

		obj_ptr(const obj_rc& rc);

		obj_ptr(const val_rc& rc);

		obj_ptr(const val_ptr& rc);
		
		obj_ptr(zval* zp);

		obj_ptr(base_d* cobj); 
		
		obj_rc clone() const;

		const obj_ptr& operator=(zend_object* rc);
		const obj_ptr& operator=(zval* rc);
		//const obj_ptr& operator=(const val_rc& rc);

		void return_zv(zval* ret) const;
		
		bool property_list(htab_rc& mgr);

		//! Closures, isCallables
		val_rc callable();
		val_rc callable(zval* arg1);
		val_rc callable(zval* arg1, zval* arg2);

		

		//! get a property value
		val_rc  property(str_ptr key);
		zval* 	  property_get(str_ptr key, zval* ret);

		//! set a property value
		void      property(str_ptr key, val_ptr value);
		void      property(str_ptr key, str_ptr value);
		
		bool      has_property(str_ptr name);
		void      unset_property(str_ptr name);
		//! property-values list
		htab_rc  properties();

		bool isNull() const { return !(obj_); }
		bool ok() const { return (obj_); }

		zend_string* className();


		//! calling methods
		val_rc call(str_ptr method);

		val_rc call(str_ptr method, HashTable* args);

		val_rc call(str_ptr method, zval* arg1);

		val_rc call(str_ptr method, 
	        zval* arg1, zval* arg2);

		val_rc call(str_ptr method, 
	        zval* arg1, zval* arg2, zval* arg3);

		val_rc call(str_ptr method, 
	        zval* arg1, zval* arg2, zval* arg3, zval* arg4);

		bool instanceof(zend_class_entry *ce) const;
		bool method_exists(str_ptr method) const;
		bool isDateTime() const;

		
		zend_object* operator->() const { return obj_; }
		operator zend_object* () const { return (zend_object*) obj_; }
	};
};

//obj_ptr.h
#endif