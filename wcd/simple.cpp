#ifndef WCD_SIMPLE_CPP
#define WCD_SIMPLE_CPP

#ifndef WCD_SIMPLE_H
#include "simple.h"
#endif

#ifndef SQLOPS_ARGINFO_H
#define SQLOPS_ARGINFO_H
extern "C" {
	#include "stub/operations_arginfo.h"
};
#endif

#ifndef SQL_PART_H
#include "sql_ipart.h"
#endif


namespace wcd {
	using namespace zpp;
	using namespace wcc;

base_obj_mgr<Simple> Simple::omg;

void 
Simple::construct(zobj_user db, int fetch)
{
	db_ = db;
	fetch_ =  (fetch >= 0) ? fetch : IDriver::FETCH_ASSOC;
}

void 
Simple::destruct()
{
	if (stmt_.ok() && db_.ok())
	{
		IDriver* db = zobj_toc<IDriver>(db_);
		db->closeStmt(stmt_);
		stmt_.set_null();
	}
}

void 
Simple::debug_info(htab_write di)
{	
	di.set(SQSTR.driver, db_);
	di.set(SQSTR.sql, sql_);
	di.set(SQSTR.fetch_key, fetch_);
	di.set(SQSTR.values_key, values_);
	di.set(SQSTR.returns_str, retval_);
	di.set(SQSTR.statement, stmt_);
}

htab_mgr 
Simple::arrayMap(zstr_user keycol, 
					zstr_user valcol, zstr_user table)
{	
	IDriver* db = zobj_toc<IDriver>(db_);

	
	ISql* isql = db->isql_c();

	zstr_buffer buf;

	buf << "select " << isql->quoteName(keycol)
	    << ", " << isql->quoteName(valcol)
	    << " from " << isql->quoteName(table);
	    
	sql_ = buf.zstr();
	fetch_ = IDriver::FETCH_NUM;

	htab_mgr rows = this->arraySet(sql_);

	

	zval_mgr result_mgr;
	htab_write result(result_mgr);

	htab_walk wk;
	auto row = wk.value();

	for(wk.start(rows); wk.ok(); wk.next())
	{
		htab_read r(row);
		zval_user row0(r.get(int(0)));
		zval_user row1(r.get(int(1)));

		result.set(row0, row1);
	}
	return result_mgr;

}

htab_mgr 
Simple::arraySet(zstr_user sql, htab_read params)
{
	IDriver* db = zobj_toc<IDriver>(db_);
	sql_ = sql;
	stmt_ = db->prepare(sql_);
	autoclose_ = true;
	if (params.size())
	{
		setValues(params);
	}
	return this->send(true);
}

zstr_mgr 
Simple::bind(zval_user value)
{
	htab_write hw(values_);
	hw.push_back(value);
	IDriver* db = zobj_toc<IDriver>(db_);
	return db->param(values_.size());
}

zval_mgr 
Simple::exec(zstr_user sql, htab_read params)
{
	this->prepare(sql);
	autoclose_ = true;
	if (params.size())
	{
		setValues(params);
	}
	return this->run();
}

zval_mgr 
Simple::firstrow(zstr_user sql, htab_read params)
{
	htab_mgr aset = this->arraySet(sql, params);
	zval_mgr result;

	if (aset.size())
	{
		result =  aset.get(int(0));
	}
	return result;
}

zval_mgr 
Simple::getRows()
{
	return this->send(true);
}

zstr_mgr 
Simple::getSchemaName()
{
	IDriver* db = zobj_toc<IDriver>(db_);
	return db->getDatabaseName();
}

zval_mgr 
Simple::insert(htab_read values)
{
	setValues(values);
	return this->send(retval_);
}

bool 
Simple::prepare(zstr_user sql)
{
	IDriver* db = zobj_toc<IDriver>(db_);
	if (stmt_.ok())
	{
		db->closeStmt(stmt_);
	}
	sql_ = sql;
	stmt_ = db->prepare(sql_);
	if (!stmt_.ok())
	{
		zend_throw_error(zend_ce_error, "Prepare failed for: %s", sql_.data());
		return false;
	}
	return true;
}

zstr_mgr 
Simple::quoteName(zstr_user name)
{
	IDriver* db = zobj_toc<IDriver>(db_);
	return db->quoteName(name);
}



void 
Simple::returnsValues(bool rval)
{
	retval_ = rval;
}

zval_mgr 
Simple::run()
{
	return this->send(retval_);
}

zval_mgr
Simple::send(bool retval)
{
	zval_mgr result;
	int fsave = -1;
	if (!stmt_.ok())
	{
		zend_throw_error(zend_ce_error, "Statement not prepared");
		return result;
	}
	IDriver* db = zobj_toc<IDriver>(db_);
	if (values_.size())
	{
		db->bind(stmt_, values_);
	}
	if (retval)
	{
		fsave = db->setFetch(fetch_);
	}
	result = db->execute(stmt_, autoclose_, retval);
	if (retval)
	{
		db->setFetch(fsave);
	}
	if (autoclose_)
	{
		stmt_.set_null();
	}
	values_.reset();
	return result;
}



void 
Simple::setValues(htab_read values)
{
	values_ = values;
}

zval_mgr 
Simple::update(htab_read values)
{
	setValues(values);
	return this->send(retval_);
}

};
//simple.cpp
#ifndef ZARG_EXEC_H
#include "zpp/zarg_exec.h"
#endif

using namespace wcd;
using namespace zpp;

ZEND_METHOD(Wcd_Sql_Simple, __construct)
{
	zarg_exec args(execute_data);

	zobj_user db;
	zend_long fetch = IDriver::FETCH_ASSOC;

	args.obj_ofclass(db, args.need(1),IDriver::omg.class_entry_);
	if (!args.zlong_null(fetch, args.option(2)))
	{
		fetch = IDriver::FETCH_ASSOC;
	}

	if (args.throw_errors())
	{
		return;
	}

	Simple* sobj = zval_toc<Simple>(ZEND_THIS);

	sobj->construct(db, fetch);

}

ZEND_METHOD(Wcd_Sql_Simple, __destruct)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Simple* sobj = zval_toc<Simple>(ZEND_THIS);

	sobj->destruct();
}

ZEND_METHOD(Wcd_Sql_Simple, arrayMap)
{
	zarg_exec args(execute_data);

	zstr_user keycol;
	zstr_user valcol;
	zstr_user table;

	args.zstring(keycol, args.need(1));
	args.zstring(valcol, args.need(2));
	args.zstring(table, args.need(3));

	htab_mgr result;

	if (args.throw_errors())
	{
		result = htab_mgr::empty_array();
	}
	else {
		Simple* sobj = zval_toc<Simple>(ZEND_THIS);
		result = sobj->arrayMap(keycol,valcol, table);
	}
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Simple, arraySet)
{
	zarg_exec args(execute_data);

	zstr_user sql;
	htab_read params;

	args.zstring(sql, args.need(1));
	args.zarray_null(params, args.option(2));

	htab_mgr result;

	if (args.throw_errors())
	{
		result = htab_mgr::empty_array();
	}
	else {
		Simple* sobj = zval_toc<Simple>(ZEND_THIS);
		result = sobj->arraySet(sql,params);
	}
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Simple, bind)
{
	zarg_exec args(execute_data);

	zval* arg = args.need(1);
	zstr_mgr result;
	if (!args.throw_errors())
	{
		Simple* sobj = zval_toc<Simple>(ZEND_THIS);

		result = sobj->bind(arg);
	}
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Simple, exec)
{
	zarg_exec args(execute_data);

	zstr_user sql;
	htab_read params;

	args.zstring(sql, args.need(1));
	args.zarray(params, args.option(2));

	zval_mgr result;

	if(!args.throw_errors())
	{
		Simple* sobj = zval_toc<Simple>(ZEND_THIS);

		result = sobj->exec(sql, params);
	}
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Simple, firstRow)
{
	zarg_exec args(execute_data);

	zstr_user sql;
	htab_read params;

	args.zstring(sql, args.need(1));
	args.zarray(params, args.option(2));

	zval_mgr result;

	if(!args.throw_errors())
	{
		Simple* sobj = zval_toc<Simple>(ZEND_THIS);

		result = sobj->firstrow(sql, params);
	}
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Simple, getRows)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Simple* sobj = zval_toc<Simple>(ZEND_THIS);

	htab_mgr result = sobj->getRows();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Simple, getSchemaName)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Simple* sobj = zval_toc<Simple>(ZEND_THIS);

	zstr_mgr result = sobj->getSchemaName();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Simple, insert)
{
	zarg_exec args(execute_data);

	htab_read values;

	args.zarray(values, args.need(1));

	zval_mgr result;

	if(!args.throw_errors())
	{
		Simple* sobj = zval_toc<Simple>(ZEND_THIS);

		result = sobj->insert(values);
	}

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Simple, prepare)
{
	zarg_exec args(execute_data);

	htab_read values;

	args.zarray(values, args.need(1));

	zval_mgr result;

	if(!args.throw_errors())
	{
		Simple* sobj = zval_toc<Simple>(ZEND_THIS);

		result = sobj->insert(values);
	}
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Simple, quoteName)
{
	zarg_exec args(execute_data);

	zstr_user name;

	args.zstring(name, args.need(1));

	zstr_mgr result;

	if(!args.throw_errors())
	{
		Simple* sobj = zval_toc<Simple>(ZEND_THIS);

		result = sobj->quoteName(name);
	}
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Simple, returnsValues)
{
	zarg_exec args(execute_data);

	bool  bval;

	args.zbool(bval, args.need(1));

	if(!args.throw_errors())
	{
		Simple* sobj = zval_toc<Simple>(ZEND_THIS);

		sobj->returnsValues(bval);
	}
}

ZEND_METHOD(Wcd_Sql_Simple, run)
{
	ZEND_PARSE_PARAMETERS_NONE();
	Simple* sobj = zval_toc<Simple>(ZEND_THIS);
	zval_mgr result = sobj->run();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Simple, setValues)
{
	zarg_exec args(execute_data);
	htab_read values;

	args.zarray(values, args.need(1));

	if(!args.throw_errors())
	{
		Simple* sobj = zval_toc<Simple>(ZEND_THIS);

		sobj->setValues(values);
	}
}

ZEND_METHOD(Wcd_Sql_Simple, update)
{
	zarg_exec args(execute_data);
	htab_read values;

	args.zarray(values, args.need(1));
	
	zval_mgr result;

	if(!args.throw_errors())
	{
		Simple* sobj = zval_toc<Simple>(ZEND_THIS);

		result = sobj->update(values);
	}
	result.move_zv(return_value);
}

PHP_MINIT_FUNCTION(Wcd_Simple_reg)
{
	Simple::omg.classEntry(register_class_Wcd_Sql_Simple());
	return SUCCESS;
}

#endif