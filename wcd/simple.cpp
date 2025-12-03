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
Simple::construct(obj_ptr db, int fetch)
{
	db_ = db;
	fetch_ =  (fetch >= 0) ? fetch : IDriver::FETCH_ASSOC;
	//showobj("\nSimple construct", vobj());

}

void 
Simple::destruct()
{
	//showobj("\nSimple destruct", vobj());

	if (stmt_.ok())
	{
		//showmem("set_null stmt", stmt_);
		stmt_.set_null();
	}
	//showstr("Simple destruct sql_", sql_);
	//sql_.init();
}

void 
Simple::debug_info(htab_rw di)
{	
	di.set(SQSTR.driver, db_);
	//di.set(SQSTR.sql, sql_);
	di.set(SQSTR.fetch_key, fetch_);
	di.set(SQSTR.values_key, values_);
	di.set(SQSTR.returns_str, retval_);
	di.set(SQSTR.statement, stmt_);
}

htab_rc 
Simple::arrayMap(str_ptr keycol, 
					str_ptr valcol, str_ptr table)
{	
	IDriver* db = zobj_toc<IDriver>(db_);
	
	ISql* isql = db->isql_c();

	str_rc msql;
	{
		str_buf buf;

		str_rc key1 = isql->quoteName(keycol);
		str_rc val2 = isql->quoteName(valcol);
		str_rc table3 = isql->quoteName(table);
		buf << "select " << key1
		    << ", " << val2
		    << " from " << table3;
	    
		msql = buf.zstr();
	}

	fetch_ = IDriver::FETCH_NUM;

	htab_rc rows = this->arraySet(msql);
	htab_rc result_mgr;

	if (rows.size())
	{
		htab_rw result(result_mgr);

		htab_walk wk;
		auto row = wk.value();

		for(wk.start(rows); wk.ok(); wk.next())
		{
			htab_ptr rd(row.zarray());
			val_ptr row0(rd.get(int(0)));
			val_ptr row1(rd.get(int(1)));

			result.set(row0, row1);
		}

		//showdata("\narrayMap results", result_mgr);
	}
	return result_mgr;

}

htab_rc 
Simple::arraySet(str_ptr sql, htab_ptr params)
{
	htab_rc result;
	IDriver* db = zobj_toc<IDriver>(db_);

	stmt_ = db->prepare(sql);


	if (stmt_.ok())
	{
		autoclose_ = false;
		if (params.size())
		{
			setValues(params);
		}
		result = this->send(true);
		db->closeStmt(stmt_);
		stmt_.set_null();

	}
	return result;
}

str_rc 
Simple::bind(val_ptr value)
{
	htab_rw hw(values_);
	hw.push_back(value);
	IDriver* db = zobj_toc<IDriver>(db_);
	return db->param(values_.size());
}

val_rc 
Simple::exec(str_ptr sql, htab_ptr params)
{
	this->prepare(sql);
	autoclose_ = true;
	if (params.size())
	{
		setValues(params);
	}
	return this->run();
}

val_rc 
Simple::firstrow(str_ptr sql, htab_ptr params)
{
	htab_rc aset = this->arraySet(sql, params);
	val_rc result;

	if (aset.size())
	{
		result =  aset.get(int(0));
	}
	return result;
}

val_rc 
Simple::getRows()
{
	autoclose_ = true;
	return this->send(true);
}

str_rc 
Simple::getSchemaName()
{
	IDriver* db = zobj_toc<IDriver>(db_);
	return db->getDatabaseName();
}

val_rc 
Simple::insert(htab_ptr values)
{
	setValues(values);
	return this->send(retval_);
}

bool_return 
Simple::prepare(str_ptr sql)
{
	bool_return result;

	IDriver* db = zobj_toc<IDriver>(db_);
	if (stmt_.ok())
	{
		db->closeStmt(stmt_);
		stmt_.set_null();
	}

	stmt_ = db->prepare(sql);


	if (!stmt_.ok())
	{
		result.error() << "Prepare failed for: " << sql.data();
		result = false;
	}
	else {
		result = true;
	}
	return result;
}

str_rc 
Simple::quoteName(str_ptr name)
{
	IDriver* db = zobj_toc<IDriver>(db_);
	return db->quoteName(name);
}



void 
Simple::returnsValues(bool rval)
{
	retval_ = rval;
}

val_return 
Simple::run()
{
	return this->send(retval_);
}

val_return
Simple::send(bool retval)
{
	val_return result;
	int fsave = -1;
	if (!stmt_.ok())
	{
		result.error() << "Statement not prepared";
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
	bool ac = autoclose_;
	//showobj("call execute", vobj());

	result = db->execute(stmt_, ac, retval);

	if (retval)
	{
		//restore value
		db->setFetch(fsave);
	}
	if (ac)
	{	
		if (stmt_.ok())
		{
			obj_ptr sobj(stmt_);
			val_rc qstr = sobj.property(SQSTR.queryString);
			stmt_.set_null();
		}
	}
	values_.reset();
	return result;
}



void 
Simple::setValues(htab_ptr values)
{
	values_ = values;
}

val_return 
Simple::update(htab_ptr values)
{
	setValues(values);
	return this->send(retval_);
}

};
//simple.cpp
#ifndef ZARG_RD_H
#include "zpp/zarg_rd.h"
#endif

using namespace wcd;
using namespace zpp;

ZEND_METHOD(Wcd_Simple, __construct)
{
	zarg_rd args(execute_data);

	obj_ptr db;
	zend_long fetch = IDriver::FETCH_ASSOC;

	args.obj_ofclass(db, args.need(0),IDriver::omg.class_entry_);
	if (!args.zlong_null(fetch, args.option(1)))
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

ZEND_METHOD(Wcd_Simple, __destruct)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Simple* sobj = zval_toc<Simple>(ZEND_THIS);

	sobj->destruct();
}

ZEND_METHOD(Wcd_Simple, arrayMap)
{
	zarg_rd args(execute_data);

	str_ptr keycol;
	str_ptr valcol;
	str_ptr table;

	args.zstring(keycol, args.need(0));
	args.zstring(valcol, args.need(1));
	args.zstring(table, args.need(2));

	htab_rc result;

	if (args.throw_errors())
	{
		result = htab_rc::empty_array();
	}
	else {
		Simple* sobj = zval_toc<Simple>(ZEND_THIS);
		result = sobj->arrayMap(keycol,valcol, table);
	}
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Simple, arraySet)
{
	zarg_rd args(execute_data);

	str_ptr sql;
	htab_ptr params;

	args.zstring(sql, args.need(0));
	args.zarray_null(params, args.option(1));

	htab_rc result;

	if (args.throw_errors())
	{
		result = htab_rc::empty_array();
	}
	else {
		Simple* sobj = zval_toc<Simple>(ZEND_THIS);
		result = sobj->arraySet(sql,params);
	}
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Simple, bind)
{
	zarg_rd args(execute_data);

	zval* arg = args.need(0);
	str_rc result;
	if (!args.throw_errors())
	{
		Simple* sobj = zval_toc<Simple>(ZEND_THIS);

		result = sobj->bind(arg);
	}
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Simple, exec)
{
	zarg_rd args(execute_data);

	str_ptr sql;
	htab_ptr params;

	args.zstring(sql, args.need(0));
	args.zarray(params, args.option(1));

	val_rc result;

	if(!args.throw_errors())
	{
		Simple* sobj = zval_toc<Simple>(ZEND_THIS);

		result = sobj->exec(sql, params);
	}
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Simple, firstRow)
{
	zarg_rd args(execute_data);

	str_ptr sql;
	htab_ptr params;

	args.zstring(sql, args.need(0));
	args.zarray(params, args.option(1));

	val_rc result;

	if(!args.throw_errors())
	{
		Simple* sobj = zval_toc<Simple>(ZEND_THIS);

		result = sobj->firstrow(sql, params);
	}
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Simple, getRows)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Simple* sobj = zval_toc<Simple>(ZEND_THIS);

	htab_rc result = sobj->getRows();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Simple, getSchemaName)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Simple* sobj = zval_toc<Simple>(ZEND_THIS);

	str_rc result = sobj->getSchemaName();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Simple, insert)
{
	zarg_rd args(execute_data);

	htab_ptr values;

	args.zarray(values, args.need(0));

	val_rc result;

	if(!args.throw_errors())
	{
		Simple* sobj = zval_toc<Simple>(ZEND_THIS);

		result = sobj->insert(values);
	}

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Simple, prepare)
{
	zarg_rd args(execute_data);

	str_rc sql;

	args.zstring(sql, args.need(0));

	bool result = false;

	if(!args.throw_errors())
	{
		Simple* sobj = zval_toc<Simple>(ZEND_THIS);

		result = sobj->prepare(sql);
	}
	RETURN_BOOL(result);
}

ZEND_METHOD(Wcd_Simple, quoteName)
{
	zarg_rd args(execute_data);

	str_ptr name;

	args.zstring(name, args.need(0));

	str_rc result;

	if(!args.throw_errors())
	{
		Simple* sobj = zval_toc<Simple>(ZEND_THIS);

		result = sobj->quoteName(name);
	}
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Simple, returnsValues)
{
	zarg_rd args(execute_data);

	bool  bval;

	args.zbool(bval, args.need(0));

	if(!args.throw_errors())
	{
		Simple* sobj = zval_toc<Simple>(ZEND_THIS);

		sobj->returnsValues(bval);
	}
}

ZEND_METHOD(Wcd_Simple, run)
{
	ZEND_PARSE_PARAMETERS_NONE();
	Simple* sobj = zval_toc<Simple>(ZEND_THIS);
	val_return result = sobj->run();
	result.throw_errors();
	result.value_.move_zv(return_value);
}

ZEND_METHOD(Wcd_Simple, setValues)
{
	zarg_rd args(execute_data);
	htab_ptr values;

	args.zarray(values, args.need(0));

	if(!args.throw_errors())
	{
		Simple* sobj = zval_toc<Simple>(ZEND_THIS);

		sobj->setValues(values);
	}
}

ZEND_METHOD(Wcd_Simple, update)
{
	zarg_rd args(execute_data);
	htab_ptr values;

	args.zarray(values, args.need(0));
	
	val_rc result;

	if(!args.throw_errors())
	{
		Simple* sobj = zval_toc<Simple>(ZEND_THIS);

		result = sobj->update(values);
	}
	result.move_zv(return_value);
}

PHP_MINIT_FUNCTION(Wcd_Simple_reg)
{
	Simple::omg.classEntry(register_class_Wcd_Simple());
	return SUCCESS;
}

#endif