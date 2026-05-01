#ifndef MODULE_WCC_H
#define MODULE_WCC_H

#ifndef WCC_CONFIG_H
#include "config.h"
#endif


namespace wcc {

using namespace zpp;

	class  Module : public Config {
	protected:
		
		htab_rc requires_;
		bool    active_;
		str_rc  cfg_path_;
		str_rc  name_;

	public:

		virtual void debug_info(htab_rw hw);
		
		static ConfigMgr<Module> omg;

		void  construct();

		void construct(str_ptr name, htab_ptr data);

		void activate(obj_ptr finder);

		void addDefaults(obj_ptr module);

		htab_rc getAssets();

		str_rc getName();

		htab_rc getValueList(str_ptr key);

		htab_rc getViewPaths();

		void setConfigPath(str_ptr path);

		VIRTUAL_ZOBJPTR

	};

}; // namespace wcc
#endif