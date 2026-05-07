#ifndef MODULE_WCC_H
#define MODULE_WCC_H

#ifndef WCC_CONFIG_H
#include "config.h"
#endif


namespace wcc {

using namespace zpp;

	class  Module : public Config 
	{
	protected:
		
		htab_rc requires_;
		bool    active_;
		str_rc  cfg_path_;
		str_rc  name_;

	public:

		void debug_info(htab_rw hw) override;
		
		template <typename T> class Module_Mgr : public Config::Config_Mgr<T>{};

		static Module::Module_Mgr<Module> omg;

		void  construct();

		void construct(str_ptr name, htab_ptr data);

		void activate(obj_ptr finder);

		void addDefaults(obj_ptr module);

		htab_rc getAssets();

		str_rc getName();

		htab_rc getViewPaths();

		void setRequires(htab_ptr rlist);

		htab_rc getRequires();

		str_rc getAssetsFile();

		htab_rc getValueList(str_ptr key);

		bool getActive();

		void setActive(bool value);

		void setConfigPath(str_ptr path);
		str_rc getConfigPath();

		VIRTUAL_ZOBJPTR

	};



	class Module_init : public state_init {
	public:
		str_intern  DEFAULT_MOD;
		str_intern  BASE;
		str_intern  ALIAS;
		str_intern  ROUTES;

		str_intern  VIEWPATHS;
		str_intern  NAMESPACES;
		str_intern  REQUIRES;
		str_intern  CLASSFILES;

		str_intern  DATABASE;
		str_intern  ASSETS;
		str_intern  ASSET_FILE;

		str_intern  dispatch_str;

		str_intern  views_str;
		str_intern  addpatharray_fn;
		str_intern  addclasses_fn;
		str_intern  loadassetfile_fn;

		str_intern  setmodule_fn;
		str_intern  dot_str;
		str_intern  active_str;
		str_intern  cfg_path_str;

		str_intern  name_str;

		void init() override;
	};

	extern Module_init MODi;
} // namespace wcc

#endif // MODULE_WCC_H