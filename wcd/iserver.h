#ifndef WCD_ISERVER_H
#define WCD_ISERVER_H




#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif


namespace wcd {

	using namespace zpp;
	using namespace wcc;

	class IServer : public base_d {
	protected:	

		zstr_mgr svc_key_;

		htab_mgr active_;

		htab_mgr config_;

		htab_mgr alias_;

		zobj_mgr dbCache_;

		htab_mgr sqlClasses_;

		htab_mgr driverClasses_;

	public:
		void construct(zstr_user svckey);

		void initDone();

		zobj_mgr getDataCache();


	};

};



#endif