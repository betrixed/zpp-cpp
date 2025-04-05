#ifndef ZOBJ_MGR_H
#define ZOBJ_MGR_H

#ifndef ZOBJ_USER_H
#include "zobj_user.h"
#endif

namespace zpp {

	class zobj_user;
	class base_d;

	class zobj_mgr : public zobj_user {
	protected:
		void own();

		void lose();


	public:

		static void try_addref(zend_object* zo);
		static bool try_decref(zend_object* zo);

		zobj_mgr() : zobj_user() {}

		zobj_mgr(zend_object* rc);

    	//! From settled cobj
    	zobj_mgr(base_d* cobj);
    	
    	zobj_mgr(const zobj_mgr& rc);

	    zobj_mgr(zobj_mgr&& rc);
		
		zobj_mgr(zval_mgr&& m);

		zobj_mgr& operator=(const zobj_user &rc);

		zobj_mgr& operator=(zobj_mgr&& rc);

		zobj_mgr& operator=(zval_mgr&& rc);

		zobj_mgr& operator=(const zval_user& zv);
		
		const zobj_mgr& operator=(zend_object* rc);

		const zobj_mgr& operator=(zval* rc);
		
		void init();

		~zobj_mgr(){
		    lose();
		}

		/*! Special adoption to set tgo zend_object* of a new base_d,
		 *  without increment of its reference count 
		 */
		void adopt(zend_object* ob);

		operator zend_object*() const 
		{
		    return (zend_object*)obj_;
		}

		void move_zv(zval* ret);
		void return_zv(zval* ret);


		static bool new_object(zstr_user classname, zobj_mgr& host);
	};

};


//zobj_mgr.h
#endif