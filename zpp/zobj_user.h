#ifndef ZOBJ_USER_H
#define ZOBJ_USER_H

#ifndef ZOBJ_MGR_H
#include "zobj_mgr.h"
#endif

#ifndef ZVAL_MGR_H
#include "zval_mgr.h"
#endif

namespace zpp {

	class htab_mgr;
	
	class zobj_user {
	protected:
		zend_object* obj_;

		friend class zobj_mgr;

	public:
		zobj_user() : obj_(nullptr) {}

		zobj_user(zend_object* rc) : obj_(rc) {}

		zobj_user(const zobj_user& rc) : obj_(rc.obj_) {}

		zobj_user(const zobj_mgr& rc) : obj_(rc.obj_) {}

		operator zend_object* () const { return (zend_object*) obj_; }

		const zobj_user& operator=(zend_object* rc);

		void return_zv(zval* ret);
		
		bool property_list(htab_mgr& mgr);

		//! Closures, isCallables
		zval_mgr callable();
		zval_mgr callable(zval_user arg1);
		zval_mgr callable(zval_user arg1, zval_user arg2);

		

		//! get a property value
		zval_mgr property(zstr_user key);
		void     property(zstr_user key, const zval_mgr& value);

		bool isNull() const { return !(obj_); }
		bool ok() const { return (obj_); }

		zend_string* className();

		//! calling methods
		zval_mgr call(zstr_user method);

		zval_mgr call(zstr_user method, 
	        const zval_mgr& arg1);

		zval_mgr call(zstr_user method, 
	        const zval_mgr& arg1, const zval_mgr& arg2);

		zval_mgr call(zstr_user method, 
	        const zval_mgr& arg1, const zval_mgr& arg2, 
	        const zval_mgr& arg3);

		zval_mgr call(zstr_user method, 
	        const zval_mgr& arg1, const zval_mgr& arg2, 
	        const zval_mgr& arg3, const zval_mgr& arg4);

	};
};

//zobj_user.h
#endif