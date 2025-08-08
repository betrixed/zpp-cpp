#ifndef WCC_ROUTE_ADD_H
#define WCC_ROUTE_ADD_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif


namespace wcc {

	class RouteAdd : public base_d 
	{
	protected:
		obj_rc route_set_;
		str_rc module_name_;
		str_rc method_sfx_;
		str_rc url_prefix_;
		val_rc fallback_;
	public:

		static base_obj_mgr<RouteAdd> omg;

		static str_ptr rex_url();
		
		virtual void debug_info(htab_rw hw);

		void construct(obj_ptr rset);

		void addRoutes(htab_rd list, str_ptr prefix, str_ptr module);

		void fallback(val_ptr backup);

		obj_ptr getRouteSet() const;

		void methodSfx(str_ptr sfx);

		void module(str_ptr name);

		void prefix(str_ptr start);

		void ready(Route* route);

		VIRTUAL_ZOBJPTR

	};
}; // namespace wcc;

#endif
