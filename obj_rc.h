#ifndef OBJ_RC_H
#define OBJ_RC_H

/**
 * @file zpp/obj_rc.h
 * @author Michael Rynn <michael.rynn.500@gmail.com>
 * @brief obj_rc- A simple zend_object* wrapper with added reference counting
 * @copyright 2025 Michael Rynn
 * @license BSD 3-Clause License
 */
#ifndef OBJ_PTR_H
#include "obj_ptr.h"
#endif

namespace zpp {

	class obj_ptr;
	class base_d;

	class obj_rc : public obj_ptr {
	protected:
		void own();

		void lose();


	public:

		friend class ref_rc;
		
		static void try_decref(zend_object* ob);
		static void try_addref(zend_object* ob);
		



		obj_rc() : obj_ptr() {}

		obj_rc(zend_object* rc);
#ifndef OMIT_BASE_D
    	obj_rc(base_d* cobj); 
#endif
    	obj_rc(const obj_rc& rc);

	    obj_rc(obj_rc&& rc);
		
		obj_rc(val_rc&& m);

		obj_rc(const zval* zp);

		obj_rc(const obj_ptr& rc);
		
		obj_rc& operator=(const obj_ptr &rc);

		obj_rc& operator=(const obj_rc &rc);

		obj_rc& operator=(obj_rc&& rc);

		obj_rc& operator=(val_rc&& rc);

		obj_rc& operator=(const val_ptr& zv);
		
		const obj_rc& operator=(zend_object* rc);

		const obj_rc& operator=(const zval* rc);
		
		void init();

		~obj_rc(){
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

		static bool new_object(str_ptr classname, obj_rc& host);
	};

};


//obj_rc.h
#endif