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
		str_rc class_;
		str_rc func_;
		str_rc module_;


		virtual void debug_info(htab_wr di);
	
		static obj_rc go(str_ptr cname, str_ptr fname);
		
		static base_obj_mgr<Target> omg;

		void construct(str_ptr cname, str_ptr fname);

		str_ptr getClass();
		str_ptr getFunc();
		str_ptr getModule();

		void setFunc(str_ptr name);
		void setModule(str_ptr name);

		htab_rc serialize();
		void unserialize(htab_rd htab);

		obj_rc copy();

		VIRTUAL_ZOBJPTR	

	};

}; //namespace wcc

//target.h
#endif