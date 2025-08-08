#ifndef WCD_ISERVER_H
#define WCD_ISERVER_H




#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif


namespace wcd {

	using namespace zpp;
	using namespace wcc;

	class IDriver;
	class IConfig;

	class IServer : public base_d {
	protected:	

		str_rc svc_key_;

		htab_rc active_;

		htab_rc config_;

		htab_rc alias_;

		obj_rc dbCache_;

		htab_rc sqlClasses_;

		htab_rc driverClasses_;


		obj_rc activate(str_ptr name);
		IConfig* needConfig(str_ptr name);

	public:

		static base_obj_mgr<IServer> omg;

		virtual void debug_info(htab_rw di);
		
		void construct(str_ptr svckey);

		void initDone();

		obj_rc getDataCache();

		obj_rc getConect(str_ptr name);

		static obj_rc connect(str_ptr name);

		obj_rc getConnect(str_ptr name);
		obj_rc getConfig(str_ptr name);
		
		str_rc getSqlClass(str_ptr dkey);
		str_rc getDriverClass(str_ptr dkey);

		htab_rd getSqlClasses();
		htab_rd getDriverClasses();

		void config(htab_rd data);

		void addConfig(obj_rc iconfig, str_ptr name);

		void setAlias(str_ptr alias, str_ptr name);
		htab_rd getAliases();
		htab_rc  getConfigNames();

	};

}; // namespace wcd



#endif