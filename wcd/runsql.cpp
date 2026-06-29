#ifndef WCD_RUNSQL_CPP
#define WCD_RUNSQL_CPP

#ifndef WCD_RUNSQL_H
#include "runsql.h"
#endif

#ifndef WCD_IDRIVER_H
#include "idriver.h"
#endif

#ifndef WCC_DEBUGLOG_H
#include "wcc/debuglog.h"
#endif

#ifndef SQL_ARGINFO_H
#define SQL_ARGINFO_H
extern "C" {
	#include "stub/sqlipart_arginfo.h"
};
#endif

namespace wcd {

	base_obj_mgr<RunSql> RunSql::omg;

	void 
	RunSql::construct(obj_ptr db, str_ptr sql, 
			 htab_ptr bind, bool rval)
	{
		db_ = db;
		sql_ = sql;
		bind_ = bind;
		retval_ = rval;
	}

	val_return 
	RunSql::operation()
	{
		val_return exresult;

		IDriver* db = zobj_toc<IDriver>(db_);

		obj_return stmt_ret = db->prepare(sql_);

		if (stmt_ret.has_errors())
		{
			exresult = stmt_ret.move_error();
			return exresult;
		}

		obj_ptr stmt = stmt_ret.value_;
		
		htab_ptr  params = bind_;

		//showdata("bind", params);

		auto pct = params.size();

		if (pct)
		{
			if (params.is_list()) //Case of multiple sets of values.
			{
				val_ptr v0 = params.get(int(0));
				if (v0.isArray())
				{
					// create result array
					htab_rw result(exresult.value_);

					htab_walk wk;

					auto val = wk.value();
					for(wk.start(params); wk.ok(); wk.next())
					{
						db->bind(stmt, val.zarray());
						val_return x2 = db->execute(stmt, false, retval_);
						if (x2.has_errors())
						{
							return x2;
						}
						result.push_back(x2.value_);
					}
					db->closeStmt(stmt);
					return exresult; // return multiple rows
				}
			}
			db->bind(stmt, params);
		}
		exresult = db->execute(stmt, true, retval_);
		//showmem("exresult Runsql", exresult.value_);
		return exresult;

	}

	val_return //static 
	RunSql::op(obj_ptr db, str_ptr sql, htab_ptr bind, bool rval)
	{
		//showstr("runsql", sql);
		//showarray("bind", bind);
		
		obj_rc obj = RunSql::omg.new_zobj();
		RunSql*  rs = zobj_toc<RunSql>(obj);
		rs->construct(db, sql, bind, rval);

		return rs->run();
	}

	val_return RunSql::run()
	{
		val_return result = operation();
		return result;
	}
}; // namespace wcd

using namespace wcd;

/*
ZEND_METHOD(Wcd_Sql_RunSql, __construct);
ZEND_METHOD(Wcd_Sql_RunSql, Op);
ZEND_METHOD(Wcd_Sql_RunSql, operation);
ZEND_METHOD(Wcd_Sql_RunSql, run);
*/

ZEND_METHOD(Wcd_Sql_RunSql, __construct)
{
	zval* db;
	zend_string* sql;
	zval*        binds = nullptr;
	bool         retval = false;

	ZEND_PARSE_PARAMETERS_START(2,4)
	Z_PARAM_OBJECT_OF_CLASS(db, IDriver::omg.classEntry())
	Z_PARAM_STR(sql)
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY_OR_NULL(binds)
	Z_PARAM_BOOL(retval)
	ZEND_PARSE_PARAMETERS_END();

	RunSql* cobj = zval_toc<RunSql>(ZEND_THIS);

	cobj->construct(db, sql, binds, retval);

}

ZEND_METHOD(Wcd_Sql_RunSql, Op)
{
	zval* db;
	zend_string* sql;
	zval*        binds = nullptr;
	bool         retval = false;

	ZEND_PARSE_PARAMETERS_START(2,4)
	Z_PARAM_OBJECT_OF_CLASS(db, IDriver::omg.classEntry())
	Z_PARAM_STR(sql)
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY_OR_NULL(binds)
	Z_PARAM_BOOL(retval)
	ZEND_PARSE_PARAMETERS_END();

	val_return result = RunSql::op(db, sql, binds, retval);
	result.value_.move_zv(return_value);
	result.throw_errors();
}

ZEND_METHOD(Wcd_Sql_RunSql, operation)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RunSql* cobj = zval_toc<RunSql>(ZEND_THIS);

	val_return result = cobj->operation();
	result.value_.move_zv(return_value);
	result.throw_errors();

}
ZEND_METHOD(Wcd_Sql_RunSql, run)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RunSql* cobj = zval_toc<RunSql>(ZEND_THIS);

	val_return result = cobj->run();
	result.value_.move_zv(return_value);
	result.throw_errors();

}


#endif
