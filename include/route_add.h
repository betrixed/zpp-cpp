#ifndef WCC_ROUTE_ADD_H
#define WCC_ROUTE_ADD_H

#ifndef WC_BASE_H
#include "wc_base.h"
#endif


namespace wcc {



	class RouteAdd : public base_d 
	{
	protected:
		zobj_own route_set_;
		zstr_own module_name_;
		zstr_own method_sfx_;
		zstr_own url_prefix_;
		zval_own fallback_;
	public:

		static base_obj_mgr<RouteAdd> omg;

		static zstr_ptr rex_url();
		
		virtual void debug_info(HashTable* ht);

		void construct(zobj_ptr rset);

		void addRoutes(htab_ptr list, zstr_ptr prefix, zstr_ptr module);

		void fallback(zval_ptr backup);

		const zobj_own& getRouteSet() const;

		void methodSfx(zstr_ptr sfx);

		void module(zstr_ptr name);

		void prefix(zstr_ptr start);

		void ready(Route* route);



	};
}; // namespace wcc;

#endif
