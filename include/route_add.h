#ifndef WCC_ROUTE_ADD_H
#define WCC_ROUTE_ADD_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif


namespace wcc {



	class RouteAdd : public base_d 
	{
	protected:
		zobj_mgr route_set_;
		zstr_mgr module_name_;
		zstr_mgr method_sfx_;
		zstr_mgr url_prefix_;
		zstr_mgr fallback_;
	public:

		static base_obj_mgr<RouteAdd> omg;

		static zstr_mgr rex_url();
		
		virtual void debug_info(htab_write hw);

		void construct(zobj_user rset);

		void addRoutes(htab_read list, zstr_user prefix, zstr_user module);

		void fallback(zval_user backup);

		zobj_user getRouteSet() const;

		void methodSfx(zstr_user sfx);

		void module(zstr_user name);

		void prefix(zstr_user start);

		void ready(Route* route);



	};
}; // namespace wcc;

#endif
