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
		obj_rc   config_;

	private:
		Services*    svc_ptr();
		Config*      config_ptr();
		RouteMatch*  rm_ptr();		


		void call_module_activate(obj_ptr module);
		htab_rc   call__module_requires(obj_ptr module);

		htab_rc   module_viewpaths(obj_ptr module);
		 

	protected:
		obj_rc createModule(str_ptr name, htab_ptr mcfg);

	public:

		void debug_info(htab_rw hw) override;
		
		static base_obj_mgr<Dispatch> omg;

		void  construct();

		void action(htab_ptr to);

		obj_return addModule(str_ptr name, val_ptr modspec);

		bool clearRouteCache(str_ptr file, str_ptr cache_name);

		error_return dispatch(obj_ptr rmatch);

		void dispatchError(obj_ptr rmatch, str_ptr err_class, str_ptr method, htab_ptr errArgs);

		void forward( val_ptr fto);

		obj_rc getActiveModule();

		htab_ptr getArgs();

		obj_rc getRoutesCache(str_ptr cache_name);

		obj_return getDefaultModule();

		str_rc getMethodName();

		obj_rc getModule(str_ptr name);

		str_rc getModuleName();

		htab_rc getRoles();

		obj_rc  getRoute();

		obj_rc  getRouteMatch();

		str_rc getUri();

		htab_return loadRoutes(str_ptr file, str_ptr cache_name);

		val_return  obcall(obj_ptr rmatch);

		val_return  obcallEx(str_ptr obclass, str_ptr obmethod, htab_ptr obargs);

		htab_return parseRaw(htab_ptr input);

		error_return respond(val_ptr content);

		void setLog(bool val);

		obj_return setModule(str_ptr name);

		void  setModuleCfg(htab_ptr cfg);

		VIRTUAL_ZOBJPTR

	};


class Disp_init : public state_init {
public:
	str_intern  finder_str;
	str_intern  default_str;
	str_intern  route_match_str;
	str_intern  run_str;

	str_intern  config_str;
	str_intern  config_dir;
	str_intern  activate_fn;
	str_intern  cache_routes_str;

	str_intern  getassets_fn;
	str_intern  getviewpaths_fn;
	str_intern  engine_str;
	str_intern  getfinder_fn;
	str_intern  addpaths_fn;

	str_intern  viewdata_str;
	str_intern  sharewithall_fn;
	str_intern  redirect_str;
	str_intern  response_str;

	str_intern  classname_str;
	str_intern  php_extn;
	str_intern  route_parser;
	str_intern  parseraw_str;

	str_intern  beforecall_str;
	str_intern  set_str;
	str_intern  modcfg_str;
	str_intern  services_str;

	str_intern  roles_str;
	str_intern  active_str;
	str_intern  modules_str;

	void init() override;
};

extern Disp_init DSPi;
}; // namespace wcc
#endif