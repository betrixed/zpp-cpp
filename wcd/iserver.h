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

		zstr_mgr svc_key_;

		htab_mgr active_;

		htab_mgr config_;

		htab_mgr alias_;

		zobj_mgr dbCache_;

		htab_mgr sqlClasses_;

		htab_mgr driverClasses_;


		zobj_mgr activate(zstr_user name);
		IConfig* needConfig(zstr_user name);

	public:

		static base_obj_mgr<IServer> omg;
		
		void construct(zstr_user svckey);

		void initDone();

		zobj_mgr getDataCache();

		zobj_mgr getConect(zstr_user name);

		static zobj_mgr connect(zstr_user name);

		zobj_mgr getConnect(zstr_user name);
		zobj_mgr getConfig(zstr_user name);
		
		zstr_user getSqlClass(zstr_user dkey);
		zstr_user getDriverClass(zstr_user dkey);

		htab_read getSqlClasses();
		htab_read getDriverClasses();

		void config(htab_read data);

		void addConfig(zobj_mgr iconfig, zstr_user name);

		void setAlias(zstr_user alias, zstr_user name);

	};

}; // namespace wcd



#endif