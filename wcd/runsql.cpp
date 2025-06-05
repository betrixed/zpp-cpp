#ifndef WCD_RUNSQL_CPP
#define WCD_RUNSQL_CPP

#ifndef WCD_RUNSQL_H
#include "runsql.h"
#endif

namespace wcd {

	void 
	RunSql::construct(zobj_user db, zstr_user sql, 
			 zval_user bind, bool rval)
	{
		db_ = db;
		sql_ = sql;
		bind_ = bind;
		retval_ = rval;
	}

	zval_mgr 
	RunSql::operation()
	{
		zval_mgr stmt = db_.call()
	}
};


#endif
