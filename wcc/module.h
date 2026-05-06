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
		//str_rc  cfg_path_;
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
		

		VIRTUAL_ZOBJPTR

	};

} // namespace wcc

#endif // MODULE_WCC_H