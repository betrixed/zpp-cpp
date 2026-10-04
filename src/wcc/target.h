#ifndef WCC_TARGET_H
#define WCC_TARGET_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

#ifndef WCC_DISPATCH_H
#include "wcc/dispatch.h"
#endif

namespace wcc {

	/**
	 * This class is meant to be extended
	 * by PHP script classes.
	 * 
	 */ 

	class Target : public base_d {
	public:
		static obj_rc go(str_ptr cname, str_ptr method, htab_ptr params);
		
		static base_obj_mgr<Target> omg;

		void construct(str_ptr cname, str_ptr fname, htab_ptr params);

		/*
		htab_rc serialize();
		void unserialize(htab_ptr htab);
		*/

		// params
		void set(str_ptr key, val_ptr value);
		val_rc get(str_ptr key);

		val_rc refParams();

		str_rc   getModule();
		str_rc   getClass();
		str_rc   getMethod();
		val_rc   getRoles();

		void   setModule(str_ptr s);
		void   setMethod(str_ptr s);

		htab_cow params();

		VIRTUAL_ZOBJPTR	

	};

	class TargetNames : public state_init {
	public:
		void init() override;

		str_intern p_objclass;
		str_intern p_objmethod;
		str_intern p_params;
		str_intern s_index;

	};

	extern TargetNames  TNinit;

}; //namespace wcc

//target.h
#endif