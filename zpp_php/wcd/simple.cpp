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

//#define DBG_LOG_SIMPLE

#ifdef DBG_LOG_SIMPLE
#	ifndef WCC_DEBUG_LOG_H
#		include "wcc/debuglog.h"
#	endif
#endif

namespace wcd {
	using namespace zpp;
	using namespace wcc;

base_obj_mgr<Simple> Simple::omg;

void 
Simple::construct(const weak_ref& db, int fetch)
{
	db_ = db.get();
	fetch_ =  (fetch >= 0) ? fetch : IDriver::FETCH_ASSOC;
}

void 
Simple::destruct()
{
	if (stmt_.ok())
	{
		stmt_.init();
	}
}

void 
Simple::debug_info(htab_rw di)
{	
	di.set(SQSTR.driver, db_);
	di.set(SQSTR.fetch_key, fetch_);
	di.set(SQSTR.values_key, values_);
	di.set(SQSTR.returns_str, retval_);
	di.set(SQSTR.statement, stmt_);
}

htab_return 
Simple::arrayMap(str_ptr keycol, 
					str_ptr valcol, str_ptr table)
{	
	htab_return result;

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

	result = this->arraySet(msql);
	if (result.has_errors())
	{
		return result;
	}
	htab_rc& rows = result.value_;

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
	result.value_ = result_mgr;
	return result;

}

htab_return 
Simple::arraySet(str_ptr sql, htab_ptr params)
{
	htab_return result;

	IDriver* db = zobj_toc<IDriver>(db_);

	obj_return stmt_ret = db->prepare(sql);


	if (stmt_ret.has_errors())
	{
		result = stmt_ret.move_error();
		return result;
	}
	stmt_ = stmt_ret.value_;
	#ifdef DBG_LOG_SIMPLE
	DebugLog* log = DebugLog::cpp_global();
	if (log)
	{
		log->dump("arraySet prepared", stmt_);
	}
	#endif


	autoclose_ = false;
	if (params.size())
	{
		setValues(params);
	}
	#ifdef DBG_LOG_SIMPLE
	if (log)
	{
		log->dump("arraySet setValues", params);
	}
	#endif

	val_return rows = this->send(true);
	#ifdef DBG_LOG_SIMPLE
	if (log)
	{
		log->dump("arraySet rows", rows.value_);
	}
	#endif
	db->closeStmt(stmt_);
	stmt_.init();

	if (rows.has_errors())
	{
		result = std::move(rows);
		return result;
	}
	result.value_ = rows.value_.zarray();
	return result;
}

str_rc 
Simple::bind(val_ptr value)
{
	str_rc result;

	IDriver* db = zobj_toc<IDriver>(db_);
	result = db->param(values_.size()+1);
	htab_rw hw(values_);
	if ((result.size()==1)&&result.starts_with('?'))
	{
		hw.push_back(value);
	}
	else {
		hw.set(result, value);
	}
	return  result;
}

val_return 
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

val_return 
Simple::firstrow(str_ptr sql, htab_ptr params)
{
	val_return result;

	htab_return aset_ret = this->arraySet(sql, params);
	if (aset_ret.has_errors())
	{
		result = std::move(aset_ret);
		return result;
	}
	htab_rc& aset = aset_ret.value_;
	if (aset.size())
	{
		result.value_ =  aset.get(int(0));
	}
	return result;
}

val_return 
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

val_return
Simple::sendValues(htab_ptr values, bool retval)
{
	setValues(values);
	return this->send(retval);
}

bool_return 
Simple::prepare(str_ptr sql)
{
	bool_return result;

	IDriver* db = zobj_toc<IDriver>(db_);
	if (stmt_.ok())
	{
		db->closeStmt(stmt_);
		stmt_.init();
	}

	#ifdef DBG_LOG_SIMPLE
	DebugLog* log = DebugLog::cpp_global();
	log->dump("Simple::sql", sql);
	#endif

	obj_return stmt_ret = db->prepare(sql);

	#ifdef DBG_LOG_SIMPLE
	log->dump("Simple::stmt_", stmt_);
	#endif
	
	if (stmt_ret.has_errors())
	{
		result = std::move(stmt_ret);
		return result;
	}
	stmt_ = stmt_ret.value_;

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
Simple::run(bool retval)
{
	return this->send(retval);
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
		//zend_printf("Values \n");
		//showdata("Values ", values_);
		error_return msg = db->bind(stmt_, values_);
		if (msg.has_errors())
		{
			result = msg.move_error();
			return result;
		}
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
			stmt_.init();
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

	weak_ref db;
	zend_long fetch = IDriver::FETCH_ASSOC;

	args.weakref(db, args.need(0));

	if (!args.zlong_null(fetch, args.option(1)))
	{
		fetch = IDriver::FETCH_ASSOC;
	}

	if (args.throw_errors(__FUNCTION__))
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

	htab_return result;

	if (args.throw_errors(__FUNCTION__))
	{
		result = htab_rc::empty_array();
	}
	else {
		Simple* sobj = zval_toc<Simple>(ZEND_THIS);
		result = sobj->arrayMap(keycol,valcol, table);
	}
	result.throw_errors();
	result.value_.move_zv(return_value);
}

ZEND_METHOD(Wcd_Simple, arraySet)
{
	zarg_rd args(execute_data);

	str_ptr sql;
	htab_ptr params;

	args.zstring(sql, args.need(0));
	args.zarray_null(params, args.option(1));

	#ifdef DBG_LOG_SIMPLE
	DebugLog* log = DebugLog::cpp_global();
	if (log)
	{
		log->dump("arraySet param", params);
	}
	#endif
	htab_return result;

	if (args.throw_errors(__FUNCTION__))
	{
		result.value_ = htab_rc::empty_array();
	}
	else {
		Simple* sobj = zval_toc<Simple>(ZEND_THIS);
		result = sobj->arraySet(sql,params);
	}
	result.throw_errors();
	result.value_.move_zv(return_value);
}

ZEND_METHOD(Wcd_Simple, bind)
{
	zarg_rd args(execute_data);

	zval* arg = args.need(0);
	str_rc result;
	if (!args.throw_errors(__FUNCTION__))
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

	val_return result;

	if(!args.throw_errors(__FUNCTION__))
	{
		Simple* sobj = zval_toc<Simple>(ZEND_THIS);

		result = sobj->exec(sql, params);

		result.throw_errors();
	}
	result.value_.move_zv(return_value);
}

ZEND_METHOD(Wcd_Simple, firstRow)
{
	zarg_rd args(execute_data);

	str_ptr sql;
	htab_ptr params;

	args.zstring(sql, args.need(0));
	args.zarray(params, args.option(1));

	val_return result;

	if(!args.throw_errors(__FUNCTION__))
	{
		Simple* sobj = zval_toc<Simple>(ZEND_THIS);

		result = sobj->firstrow(sql, params);

		result.throw_errors();
	}
	result.value_.move_zv(return_value);
}

ZEND_METHOD(Wcd_Simple, getRows)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Simple* sobj = zval_toc<Simple>(ZEND_THIS);

	val_return result = sobj->getRows();
	result.throw_errors();
	result.value_.move_zv(return_value);
}

ZEND_METHOD(Wcd_Simple, getSchemaName)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Simple* sobj = zval_toc<Simple>(ZEND_THIS);

	str_rc result = sobj->getSchemaName();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Simple, sendValues)
{
	zarg_rd args(execute_data);

	htab_ptr values;
        bool     retval = true;

	args.zarray(values, args.need(0));
        args.zbool(retval, args.option(1));

	val_return result;

	if(!args.throw_errors(__FUNCTION__))
	{
		Simple* sobj = zval_toc<Simple>(ZEND_THIS);

		result = sobj->sendValues(values);

		result.throw_errors();
	}

	result.value_.move_zv(return_value);
}

ZEND_METHOD(Wcd_Simple, prepare)
{
	zarg_rd args(execute_data);

	str_rc sql;

	args.zstring(sql, args.need(0));

	bool_return result;

	if(!args.throw_errors(__FUNCTION__))
	{
		Simple* sobj = zval_toc<Simple>(ZEND_THIS);

		result = sobj->prepare(sql);

		result.throw_errors();
	}
	RETURN_BOOL(result.value_);
}

ZEND_METHOD(Wcd_Simple, quoteName)
{
	zarg_rd args(execute_data);

	str_ptr name;

	args.zstring(name, args.need(0));

	str_rc result;

	if(!args.throw_errors(__FUNCTION__))
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

	if(!args.throw_errors(__FUNCTION__))
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

	if(!args.throw_errors(__FUNCTION__))
	{
		Simple* sobj = zval_toc<Simple>(ZEND_THIS);

		sobj->setValues(values);
	}
}


PHP_MINIT_FUNCTION(Wcd_Simple_reg)
{
	Simple::omg.classEntry(register_class_Wcd_Simple());
	return SUCCESS;
}

#endif