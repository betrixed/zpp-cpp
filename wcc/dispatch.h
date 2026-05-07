#ifndef DISPATCH_WCC_H
#define DISPATCH_WCC_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif


namespace wcc {

using namespace zpp;

	class  Services;
	class  Run;

	class  Dispatch : public base_d {
	protected:
		
		int url_time_id_;
		bool doLog_;

		htab_rc roles_;

		int  dcount_;

		str_rc   loadedFile_;
		htab_rc  modules_;
		htab_rc  loaded_;
		obj_rc   active_;
		htab_rc  modcfg_;

		obj_rc   services_;
		obj_rc   finder_;
		obj_rc   route_match_;
		obj_rc   run_;

	private:
		Services* svc_ptr();
		Run*      run_ptr();
		RouteMatch* rm_ptr();		


		void call_module_activate(obj_ptr module);
		htab_rc   call__module_requires(obj_ptr module);

		htab_rc   module_viewpaths(obj_ptr module);
		 

	public:

		virtual void debug_info(htab_rw hw);
		
		static base_obj_mgr<Dispatch> omg;

		void  construct();

		void action(htab_rc to);

		obj_return addModule(str_ptr name, val_ptr modspec);

		bool clearRouteCache(str_ptr file, str_ptr cache_name);

		error_return dispatch(obj_ptr rmatch);

		void dispatchError(obj_ptr rmatch, str_ptr err_class, str_ptr method, htab_ptr errArgs);

		void forward( val_ptr fto);

		obj_rc getActiveModule();

		htab_rc getArgs();

		obj_rc getCache(str_ptr cache_name);

		obj_return getDefaultModule();

		str_rc getMethodName();

		obj_rc getModule(str_ptr name);

		str_rc getModuleName();

		htab_rc getRoles();

		obj_rc  getRoute();

		obj_rc  getRouteMatch();

		str_rc getUri();

		htab_return loadRoutes(str_ptr file, str_ptr cache_name);

		val_rc  obcall(obj_ptr rmatch);

		val_rc  obcallEx(str_ptr obclass, str_ptr obmethod, htab_ptr obargs);

		htab_return parseRaw(htab_ptr input);

		void respond(val_ptr content);

		void setLog(bool val);

		obj_return setModule(str_ptr name);


		VIRTUAL_ZOBJPTR

	};

}; // namespace wcc
#endif