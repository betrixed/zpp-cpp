#ifndef WCD_RUNSQL_CPP
#define WCD_RUNSQL_CPP

#ifndef WCD_RUNSQL_H
#include "runsql.h"
#endif

#ifndef WCD_IDRIVER_H
#include "idriver.h"
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
		IDriver* db = zobj_toc<IDriver>(db_);

		zval_mgr stmt = db->prepare(sql_);

		htab_mgr  params_mgr = bind_;
		htab_read params(params_mgr);

		auto pct = params.size();
		htab_mgr result_mgr;

		if (pct)
		{
			zval_user a0 = params.get(int(0));
			if (a0.isArray())
			{
				if (pct > 1)
				{
					
					htab_write result(result_mgr);

					htab_walk wk
					auto ix = wk.key();
					auto val = wk.value();
					for(wk.start(params); wk.ok(); wk.next())
					{
						db_->bind(stmt, val.zarray());
						zval_mgr x1 = db->execute(stmt, false, retval_);
						result.push_back(x1);
					}
					db->closeStmt(stmt);
					return result_mgr;
				}
				else {
					params_mgr = a0;
					params = params_mgr;
				}
			}
		}
		result_mgr  = db->execute(stmt, true, retval_);
		return result;

	}

	zval_mgr //static 
	RunSql::op(zobj_user db, zstr_user sql, htab_read params, bool rval = false )
	{
		zobj_mgr obj = RunSql::omg.new_zobj();
		RunSql*  rs = zobj_toc<RunSql>(obj);
		rs->construct(db, sql, params, rval);

		return rs->run();
	}

	zval_mgr RunSql::run()
	{
		zval_mgr result = operation();
		return result;
	}
};

ZEND_METHOD(Wcd_RunSql, __construct)
{
	zval* db;
	zend_string* sql;
	zval*        binds = nullptr;
	bool         retval = false;

	ZEND_PARSE_PARAMETERS_START(2,4)
	Z_PARAM_OBJECT_OF_CLASS(db, IDriver::omg.classEntry())
	Z_PARAM_STR(sql)
	ZEND_PARSE_OPTIONAL
	Z_PARAM_ARRAY_OR_NULL(binds)
	Z_PARAM_BOOL(retval)
	ZEND_PARSE_PARAMETERS_END();

	RunSql* cobj = zval_toc<RunSql>(ZEND_THIS);

	cobj->construct(db, sql, binds, retval);

}

ZEND_METHOD(Wcd_RunSql, Op)
{
	zval* db;
	zend_string* sql;
	zval*        binds = nullptr;
	bool         retval = false;

	ZEND_PARSE_PARAMETERS_START(2,4)
	Z_PARAM_OBJECT_OF_CLASS(db, IDriver::omg.classEntry())
	Z_PARAM_STR(sql)
	ZEND_PARSE_OPTIONAL
	Z_PARAM_ARRAY_OR_NULL(binds)
	Z_PARAM_BOOL(retval)
	ZEND_PARSE_PARAMETERS_END();

	zval_mgr result = RunSql::Op(db, sql, binds, retval);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_RunSql, operation)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RunSql* cobj = zval_toc<RunSql>(ZEND_THIS);

	zval_mgr result = cobj->operation();
	result.move_zv(return_value);

}
ZEND_METHOD(Wcd_RunSql, run)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RunSql* cobj = zval_toc<RunSql>(ZEND_THIS);

	zval_mgr result = cobj->run );
	result.move_zv(return_value);

}

/*
ZEND_METHOD(Wcd_RunSql, __construct);
ZEND_METHOD(Wcd_RunSql, Op);
ZEND_METHOD(Wcd_RunSql, operation);
ZEND_METHOD(Wcd_RunSql, run);
*/
#endif
