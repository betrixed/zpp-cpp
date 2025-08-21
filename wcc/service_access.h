#ifndef SERVICE_ACCESS_H
#define SERVICE_ACCESS_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

namespace wcc {
	using namespace zpp;
	
	/**
	 * This class is intended to be extended
	 * by PHP script classes.
	 * 
	 */ 
	class ServiceAccess : public base_d {
	protected:
		obj_rc    		services_;
		obj_rc          gservices_;

		htab_rc    		cache_;
		class_data      extender_;
	public:

		static base_obj_mgr<ServiceAccess> omg;

		virtual void debug_info(htab_rw hw);

		virtual void init_access();

		virtual str_ptr extender();

		void construct(val_ptr services_obj);

		void destruct();
		
		obj_ptr getServices();

		val_rc service(str_ptr name);

		void set(str_ptr name, val_ptr value);

		bool has(str_ptr name);

		void unset(str_ptr name);

		void setExtender(obj_ptr obj);

		val_rc nullService(str_ptr name);

		VIRTUAL_ZOBJPTR

	};

};//namespace
//service_access.h
#endif