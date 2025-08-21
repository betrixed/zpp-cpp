#ifndef OBJ_RC_H
#define OBJ_RC_H

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
		static void try_decref(zend_object* ob)
		{
			if (!ob || (GC_FLAGS(ob) & GC_IMMUTABLE))
            {
                return;
            }
			auto& rct = ob->gc.refcount;
			if (rct==1) 
			{
				zend_object_release(ob);
				return;
			}
			rct--;	
		}

		static void try_addref(zend_object* ob)
		{
			if (!ob || (GC_FLAGS(ob) & GC_IMMUTABLE))
            {
                return;
            }
            ob->gc.refcount++;
		}


		obj_rc() : obj_ptr() {}

		obj_rc(zend_object* rc);

    	obj_rc(base_d* cobj); 
    	
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