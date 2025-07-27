#ifndef WCD_SIMPLE_CPP
#define WCD_SIMPLE_CPP

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
		stmt_.init();
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
	di.set(SQSTR.stmt, stmt_);
}

htab_mgr 
Simple::arrayMap(zstr_user keycol, 
					zstr_user valcol, zstr_user table)
{	
	IDriver* db = zobj_toc<IDriver>(db_);
	int fsave = db->setFetch(IDriver::FETCH_NUM);
	ISql* isql = db->isql_c();

	zstr_buffer buf;

	buf << "select " << isql->quoteName(keycol)
	    << ", " . isql->quoteName(valcol)
	    << " from " << isql->quoteName(table);
	    
	sql_ = buf.zstr();

	htab_mgr rows = this->arraySet(sql_);

	zval_mgr result_mgr;
	htab_write result(result_mgr);

	htab_walk wk;
	auto row = wk.value();

	for(wk.start(rows); wk.ok(); wk.next())
	{
		zval_user row0(row.get(int(0)));
		zval_user row1(row.get(int(1)));

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
	values_.push_back(value);
	IDriver* db = zobj_toc<IDriver>(db_);
	return db->param(values_.size());
}

zval_mgr 
Simple::exec(zstr_user sql, htab_read params)
{
	IDriver* db = zobj_toc<IDriver>(db_);
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

zval_mgr 
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
	}
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
	if (autoclose_)
	{
		stmt_.init();
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
#endif