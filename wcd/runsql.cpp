#ifndef WCD_RUNSQL_CPP
#define WCD_RUNSQL_CPP

#ifndef WCD_RUNSQL_H
#include "runsql.h"
#endif

namespace wcd {

	base_obj_mgr<RunSql> RunSql::omg;

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

/*
ZEND_METHOD(Wcd_RunSql, __construct);
ZEND_METHOD(Wcd_RunSql, Op);
ZEND_METHOD(Wcd_RunSql, operation);
ZEND_METHOD(Wcd_RunSql, run);
*/
#endif
