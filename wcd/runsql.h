#ifndef WCD_RUNSQL_H
#define WCD_RUNSQL_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

namespace wcd {
	using namespace zpp;

	class RunSql : public base_d {
	protected:
		zobj_mgr  db_;
		zstr_mgr  sql_;
		htab_mgr  bind_;
		bool      retval_;

	public:
		void construct(zobj_user db, zstr_user sql, 
			 zval_user bind_, bool rval_);

		zval_mgr operation();
		zval_mgr run();
		
		static zval_mgr op(zobj_user db, zstr_user sql, htab_read params, bool rval = false );

	};
};


#endif