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
		zobj_mgr    	services_;
		htab_mgr    	cache_;
		class_data      extender_;
	public:

		static base_obj_mgr<ServiceAccess> omg;

		virtual void debug_info(htab_write hw);

		virtual void init_access();

		virtual zstr_user extender();

		void construct(zval_user services_obj);

		void destruct();
		
		zobj_user getServices();

		void setServices(zval_user svc);

		zval_mgr service(zstr_user name);

		void set(zstr_user name, zval_user value);

		bool has(zstr_user name);

		void unset(zstr_user name);

		void setExtender(zobj_user obj);

		zval_mgr nullService(zstr_user name);

		VIRTUAL_ZOBJPTR

	};

};//namespace
//service_access.h
#endif