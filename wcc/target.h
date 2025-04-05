#ifndef WCC_TARGET_H
#define WCC_TARGET_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif


namespace wcc {

	/**
	 * This class is meant to be extended
	 * by PHP script classes.
	 * 
	 */ 

	class Target : public base_d {
	public:
		zstr_mgr class_;
		zstr_mgr func_;
		zstr_mgr module_;


		virtual void debug_info(htab_write di);
	
		static zobj_mgr go(zstr_user cname, zstr_user fname);
		
		static base_obj_mgr<Target> omg;

		void construct(zstr_user cname, zstr_user fname);

		zstr_user getClass();
		zstr_user getFunc();
		zstr_user getModule();

		void setFunc(zstr_user name);
		void setModule(zstr_user name);

		htab_mgr serialize();
		void unserialize(htab_read htab);

		zobj_mgr copy();

		VIRTUAL_ZOBJPTR	

	};

}; //namespace wcc

//target.h
#endif