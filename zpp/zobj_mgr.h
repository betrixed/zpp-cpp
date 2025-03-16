#ifndef ZOBJ_MGR_H
#define ZOBJ_MGR_H

namespace zpp {

	class zobj_user;
	class base_d;

	class zobj_mgr {
	protected:
		zend_object* obj_;

		void own();

		void lose();

		friend class zobj_user;

	public:

		static void try_addref(zend_object* zo);
		static bool try_delref(zend_object* zo);

		zobj_mgr() : obj_(nullptr) {}

		zobj_mgr(zend_object* rc) : obj_(rc)
    	{
	        own();
    	}
    	//! From settled cobj
    	zobj_mgr(base_d* cobj);
    	
    	zobj_mgr(const zobj_mgr& rc) : obj_(rc.obj_)
	    {
	        own();
	    }


	    zobj_mgr(zobj_mgr&& rc) : obj_(rc.obj_)
		{
		    rc.obj_ = nullptr;
		}
		
		zobj_mgr(zval_mgr&& m);

		bool isNull() const { return !(obj_); }
		bool ok() const { return (obj_); }

		zobj_mgr& operator=(const zobj_user &rc);

		zobj_mgr& operator=(zobj_mgr&& rc);

		zobj_mgr& operator=(zval_mgr&& rc);

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