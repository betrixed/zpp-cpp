#ifndef OBJ_PTR_H
#define OBJ_PTR_H

/**
 * @file zpp/obj_ptr.h
 * @author Michael Rynn <michael.rynn.500@gmail.com>
 * @brief obj_ptr - A simple zend_object* wrapper
 * @copyright
 * @license Artistic License 2.0
 */
#ifndef VAL_RC_H
#include "val_rc.h"
#endif

#ifndef STR_PTR_H
#include "str_ptr.h"
#endif

namespace zpp {

	class htab_rc;
	class obj_rc;
	class val_rc;
	/**
	 * @class obj_ptr
	 * @brief A simple zend_object* wrapper
	 * @details
	 * Construction, callable, call methods, property get/set.
	 */
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

		bool get_proplist(htab_rc& mgr);

		//! Closures, isCallables
		val_rc callable();
		val_rc callable(zval* arg1);
		val_rc callable(zval* arg1, zval* arg2);

		

		//! get a property value
		val_rc    property(str_ptr key);
		zval* 	  property_get(str_ptr key, zval* ret);

		zval*     property_ptr(str_ptr key);
		
		//! set a property value
		void      property(str_ptr key, val_ptr value);
		void      property(str_ptr key, str_ptr value);
		void      property(str_ptr key, obj_ptr value);
		void      property(str_ptr key, val_rc& value);
		
		bool      has_property(str_ptr name);
		void      unset_property(str_ptr name);
		//! property-values list
		htab_rc  properties();

		bool isNull() const { return !(obj_); }
		bool ok() const { return (obj_); }

		zend_string* className();

		/** Calling methods of obj_ptr.
  			If obj_ptr contains a nullptr, its a global function call.
  			else its a call to a method of its zend_object
		*/
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