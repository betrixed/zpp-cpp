#ifndef WCD_IDRIVER_H
#define WCD_IDRIVER_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

#ifndef WCD_ICONFIG_H
#include "iconfig.h"
#endif

namespace wcd {

	using namespace zpp;
	using namespace wcc;

	class IDriver : base_d {

	public:

		void construct(zobj_user icfg, zstr_user name);
		void destruct();


	protected:
		zval_mgr 	handle_;

		int         ifetch_;
		bool        logging_;
		zstr_mgr    lastsql_;

		zobj_mgr    isql_;

		zstr_mgr    cfg_name_;
		htab_mgr    table_models_;

		zobj_mgr    schema_def_;

		friend class IBuild;

	};

};

#endif