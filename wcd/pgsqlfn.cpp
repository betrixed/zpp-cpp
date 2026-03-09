#ifndef PGSQLFN_WDC_CPP
#define PGSQLFN_WDC_CPP

#ifndef PGSQLFN_WDC_H
#include "pgsqlfn.h"
#endif

#ifndef PDO_PGSQL_ARGINFO
#define PDO_PGSQL_ARGINFO
extern "C" {
	#include "stub/pdo_pgsql_arginfo.h"
}
#endif

#ifndef PGSQLFN_ARGINFO
#define PGSQLFN_ARGINFO

extern "C" {
	#include "stub/pgsfnwrap_arginfo.h"
}
#endif

namespace wcd {

using namespace zpp;

PgInit   Pgfi;

class PgfnTable {
public:
	fn_call_args2 pg_connect;
	fn_call_args1 free_result;
	fn_call_args3 pg_prepare;
	fn_call_args3 pg_execute;

	fn_call_args1 pg_affected_rows;
	fn_call_args3 pg_fetch_array;
	fn_call_args4 pg_fetch_object;

	fn_call_args1 pg_fetch_assoc;
	fn_call_args2 pg_fetch_all;
	fn_call_args2 pg_fetch_all_columns;

	fn_call_args1 pg_close;
	fn_call_args2 pg_query;
	fn_call_args2 pg_escape_string;
	fn_call_args1 pg_last_error;

	zend_class_entry*  pgsql_result_ce;

	void init(const PgInit& pg)
	{
		free_result.set_fname(pg.pg_free_result_fn);
		pg_prepare.set_fname(pg.pg_prepare_fn);
		pg_last_error.set_fname(pg.pg_last_error_fn);
		pg_execute.set_fname(pg.pg_execute_fn);
		pg_affected_rows.set_fname(pg.pg_affected_rows_fn);

		pg_fetch_array.set_fname(pg.pg_fetch_array_fn);
		pg_last_error.set_fname(pg.pg_last_error_fn);
		pg_fetch_object.set_fname(pg.pg_fetch_object_fn);
		pg_fetch_assoc.set_fname(pg.pg_fetch_assoc_fn);
		pg_fetch_all.set_fname(pg.pg_fetch_all_fn);

		pg_fetch_all_columns.set_fname(pg.pg_fetch_all_columns_fn);
		pg_connect.set_fname(pg.pg_connect_fn);

		pg_close.set_fname(pg.pg_close_fn);
		pg_query.set_fname(pg.pg_query_fn);
		pg_escape_string.set_fname(pg.pg_escape_string_fn);


		pgsql_result_ce = class_data::get_class(pg.pgsql_result_class);
	}
};

thread_local PgfnTable PGfn;


void PgInit::init()
{
	pg_free_result_fn = "pg_free_result";
	pg_fetch_array_fn = "pg_fetch_array";
	pg_fetch_object_fn = "pg_fetch_object";
	pg_fetch_assoc_fn = "pg_fetch_assoc";

	pg_affected_rows_fn = "pg_affected_rows";

	pg_fetch_all_fn = "pg_fetch_all";
	pg_fetch_all_columns_fn = "pg_fetch_all_columns";

	pg_connect_fn = "pg_connect";
	pg_close_fn = "pg_close";
	pg_query_fn = "pg_query";
	pg_execute_fn = "pg_execute";
	pg_last_error_fn = "pg_last_error";
	pg_escape_string_fn = "pg_escape_string";

	pgsql_result_class = "pgsql\\result";

	squote_char = "'";
	esc_squote = "\\'";

	dbname_s = "dbname";
	host_s = "host";
	port_s = "port";
	user_s = "user";
	pwd_s = "password";
	blank_s = " ";

	pgsql = "pgsql";

	table_names_q = "select tablename from pg_tables" 

	" where schemaname not in ('information_schema', 'pg_catalog')"
	" order by tablename";
}

void PgInit::init_req()
{
	PGfn.init(*this);
}

void pg_free_result(obj_ptr h)
{
	auto& fn = PGfn.free_result;
	zval* args = fn.argsptr();
	ZVAL_OBJ(args, h);
	fn.call_fn();	
}

str_rc pg_escape_string(obj_ptr conn, str_ptr s)
{
	auto& fn = PGfn.pg_escape_string;
	zval* zv = fn.argsptr();
	ZVAL_OBJ(zv, conn);
	zv++;
	ZVAL_STR(zv, s);
	str_rc es = fn.call_fn();
	return es;
}

long pg_affected_rows(obj_ptr result)
{
	auto& fn = PGfn.pg_affected_rows;
	zval* args = fn.argsptr();
	ZVAL_OBJ(args, result);
	val_rc rows = fn.call_fn();
	return rows.zlong();
}


obj_rc 
pg_prepare(obj_ptr h, str_ptr id, str_ptr sql)
{
	fn_call_args3& fn = PGfn.pg_prepare;
	zval* zv = fn.argsptr();
	ZVAL_OBJ(zv, h);
	zv++;
	ZVAL_STR(zv, id);
	zv++;
	ZVAL_STR(zv, sql);
	obj_rc result = fn.call_fn();
	return result;
}

obj_rc 
pg_last_error(obj_ptr connect)
{
	auto& fn  = PGfn.pg_last_error;
	zval* zv = fn.argsptr();
	ZVAL_OBJ(zv, connect);
	obj_rc result = fn.call_fn();
	return result;
}




val_rc 
pg_query(obj_ptr connect, str_ptr query)
{
	auto& fn = PGfn.pg_query;
	zval* zv = fn.argsptr();
	ZVAL_OBJ(zv, connect);
	zv++;
	ZVAL_STR(zv, query);
	val_rc result = fn.call_fn();
	return result;
}


obj_rc pg_connect(str_ptr s, int flags)
{
	auto& fn = PGfn.pg_connect;
	zval* zv = fn.argsptr();
	ZVAL_STR(zv, s);
	zv++;
	ZVAL_LONG(zv, flags);

	obj_rc result = fn.call_fn();
	return result;

}

void pg_close(obj_ptr connect)
{
	auto& fn = PGfn.pg_close;
	zval* zv = fn.argsptr();
	ZVAL_OBJ(zv, connect);
	fn.call_fn();
}



val_rc
pg_execute(obj_ptr connect, str_ptr sname, htab_ptr params)
{
	auto& fn = PGfn.pg_execute;
	zval* zv = fn.argsptr();
	ZVAL_OBJ(zv, connect);
	zv++;
	ZVAL_STR(zv, sname);
	zv++;
	ZVAL_ARR(zv, params);

	val_rc result = fn.call_fn();
	return result;
}



val_rc 
pg_fetch_object(obj_ptr robj, val_ptr row, 
			str_ptr cname, htab_ptr args)
{
	auto& fn = PGfn.pg_fetch_object;
	str_rc    objclass;

	zval* zv = fn.argsptr();
	ZVAL_OBJ(zv,robj);
	zv++;
	if (row.ok()) {
		ZVAL_COPY_VALUE(zv, row);
	}
	else {
		ZVAL_NULL(zv);
	}
	zv++;
	if (!cname.ok())
	{
		objclass = "stdClass";
	}
	else {
		objclass = cname;
	}
	ZVAL_STR(zv, objclass);
	zv++;
	ZVAL_ARR(zv, args);

	val_rc result = fn.call_fn();
	return result;
}

htab_rc pg_fetch_all_columns(obj_ptr robj, int colnum)
{
	auto& fn = PGfn.pg_fetch_all_columns;
	zval* zv = fn.argsptr();
	ZVAL_OBJ(zv,robj);
	zv++;
	ZVAL_LONG(zv, colnum);

	htab_rc result = fn.call_fn();
	return result;
}
val_rc 
pg_fetch_assoc(obj_ptr robj)
{
	auto& fn = PGfn.pg_fetch_assoc;
	zval* zv = fn.argsptr();
	ZVAL_OBJ(zv,robj);
	val_rc result = fn.call_fn();
	return result;
}

val_rc
pg_fetch_array(obj_ptr robj, val_ptr row, int fmode)
{
	auto& fn = PGfn.pg_fetch_array;
	zval* zv = fn.argsptr();
	ZVAL_OBJ(zv,robj);
	zv++;
	ZVAL_COPY_VALUE(zv, row);
	zv++;
	ZVAL_LONG(zv, fmode);

	val_rc result = fn.call_fn();
	return result;
}

val_rc
pg_fetch_all(obj_ptr robj, int fmode)
{
	auto& fn = PGfn.pg_fetch_all;
	zval* zv = fn.argsptr();
	ZVAL_OBJ(zv, robj);
	zv++;
	ZVAL_LONG(zv, fmode);

	val_rc result = fn.call_fn();
	return result;	
}


void 	
PgQuery::destruct()
{
	close();
}

void 
PgQuery::close()
{
	if (!rhandle_.isNull())
	{
		pg_free_result(rhandle_);
		rhandle_.init();
	}
}

void 
PgQuery::construct(obj_ptr connect, str_ptr query)
{
	Pgsqlfn* pgc = zobj_toc<Pgsqlfn>(connect);
	obj_return h = pgc->handle();

	if (!h.has_errors())
	{
		pghandle_ = h.value_;
	}
	str_buf buf;
	buf << "q" << pgc->nextId();
	id_ = buf.zstr();
	sql_ = query;
	rhandle_ = pg_prepare(pghandle_, id_, query);
}


void 
PgQuery::setParams(htab_ptr params)
{
	params_ = params;
}


val_return 
PgQuery::execute(bool asResult)
{
	val_return result;
	val_rc test = pg_execute(pghandle_, id_, params_);
	if (!test.isFalse())
	{
		if (asResult) 
		{
			result.value_ = test;
		}
		else {
			result.value_ = pg_affected_rows(test);
			pg_free_result(test);
		}
	}
	else {
		result.error() << pg_last_error(pghandle_);
	}
	return result;
}

val_rc  //static
Pgsqlfn::rowFetch(obj_ptr pgresult, zend_long fmode)
{
	val_rc result;//init null

	switch(fmode)
	{
	case IDriver::FETCH_NUM:
		result = pg_fetch_array(pgresult, result, PGSQL_NUM);
		break;
	case IDriver::FETCH_OBJECT:
		result = pg_fetch_object(pgresult);
		break;
	default:
		result =  pg_fetch_assoc(pgresult);
	}
	return result;
}

htab_rc 
Pgsqlfn::allRows(obj_ptr pgresult, zend_long fmode = IDriver::FETCH_ASSOC)
{
	htab_rc result;
	val_rc data;
	switch(fmode)
	{
	case IDriver::FETCH_OBJECT:
		{
			htab_rw rows(result);
			while(true) {
				data = pg_fetch_object(pgresult);
				if (data.isArray())
				{
					rows.push_back(data.zarray());
				}
				else {
					break;
				}
			}
		}
		break;
	case IDriver::FETCH_ASSOC:
		result = pg_fetch_all(pgresult);
		break;
	case IDriver::FETCH_NUM:
		result = pg_fetch_all(pgresult, PGSQL_NUM);
		break;
	}
	return result;
}


str_rc  // static 
Pgsqlfn::attribute(str_ptr name, str_ptr value)
{
	str_rc eval;
	str_rc result;

	if (value.contains(Pgfi.squote_char))
	{
		eval = str_replace(Pgfi.squote_char, Pgfi.esc_squote, value);
	}
	else {
		eval = value;
	}
	str_buf buf;
	buf << name << '=' << eval;
	result = buf.zstr();
	return result;
}

str_rc //virtual
Pgsqlfn::param(unsigned pno)
{
	str_buf buf;
	buf << "$" << (int) pno;
	return buf.zstr();
}

error_return //virtual
Pgsqlfn::connect()
{
	IConfig* cfg = zobj_toc<IConfig>(this->iconfig());

	str_rc host = cfg->getHost();
	str_rc dbname = cfg->getDatabase();
	str_rc user = cfg->getUsername();
	str_rc pwd = cfg->getPassword();
	val_rc port = cfg->getPort();
	port.toString();

	htab_rc cparams;

	htab_rw cp(cparams);

	cp.push_back(Pgsqlfn::attribute(Pgfi.dbname_s, dbname));
	cp.push_back(Pgsqlfn::attribute(Pgfi.host_s, host));
	cp.push_back(Pgsqlfn::attribute(Pgfi.port_s, port.zstr()));
	cp.push_back(Pgsqlfn::attribute(Pgfi.user_s, user));
	cp.push_back(Pgsqlfn::attribute(Pgfi.pwd_s, pwd));

	str_rc cstr = implode(Pgfi.blank_s, cparams);
	obj_rc hconnect = pg_connect(cstr);
	error_return result;

	if (hconnect.ok())
	{
		val_rc::try_decref(handle_ptr_);
		handle_ptr_.bind_object(hconnect);
	}
	else {
		result.error() << "Failed to connect: " << cstr;
	}
	return result;
}

unsigned long 
Pgsqlfn::nextId()
{
	idseq_++;
	return idseq_;
}

bool  //virtual
Pgsqlfn::inTransaction()
{
	return inTransaction_;
}

void //virtual
Pgsqlfn::close()
{
	if (handle_ptr_.ok())
	{
		val_rc h(handle_ptr_);
		pg_close(h);
	}
}

obj_return //virtual
Pgsqlfn::prepare(str_ptr query, htab_ptr options)
{
	obj_rc qobj = PgQuery::omg.new_zobj();
	PgQuery* zobj = zobj_toc<PgQuery>(qobj);

	val_rc::try_decref(lastsql_ptr_);
	lastsql_ptr_.bind_string(query);

	zobj->construct(self_, query);

	obj_return result;
	result.value_ = qobj;
	return result;
}

val_return //virtual
Pgsqlfn::execute(obj_ptr stmt, bool close,  bool fetch)
{
	val_return result;

	PgQuery* zobj = zobj_toc<PgQuery>(stmt);
	val_return test = zobj->execute(fetch);

	if (test.has_errors())
	{
		result = test.move_error();
		return result;
	}


	if (fetch && test.value_.isObject())
	{
		result.value_ = Pgsqlfn::allRows(test.value_.zobject(), ifetch_);
	}
	else {
		result.value_ = test.value_;
	}
	if (close)
	{
		zobj->close();
	}
	return result;
}

val_return  //virtual
Pgsqlfn::querySingle(str_ptr query)
{
	val_return result;

	obj_return h = handle();
	if (h.has_errors())
	{	
		result = h.move_error();
		return result;
	}

	val_rc::try_decref(lastsql_ptr_);
	lastsql_ptr_.bind_string(query);

	obj_rc robj = pg_query(h.value_, query);

	if (!robj.ok())
	{
		result.error() << "SQL failed: " << query;
		return result;
	}

	result.value_= Pgsqlfn::allRows(robj, ifetch_);
	return result;

}

str_rc  //virtual
Pgsqlfn::getSqlType() 
{
	return Pgfi.pgsql;
}


str_rc  //virtual
Pgsqlfn::escape(str_ptr value)
{
	str_rc result;
	obj_return test = handle();
	if (test.has_errors())
	{
		test.throw_errors();
		return result;
	}
	result = pg_escape_string(test.value_, value);
	return result;
}

val_return  //virtual
Pgsqlfn::lastInsertId(str_ptr name)
{
	val_return result;
	IDriver::notImplementedMsg(result.error(),__FUNCTION__);
	return result;
}

val_return  //virtual
Pgsqlfn::lastSeqValue(str_ptr name)
{

	str_rc sql = isql_c()->seqLastValue(name);
	int fetch = ifetch_;
	ifetch_ = IDriver::FETCH_NUM;
	val_return result = querySingle(sql);
	ifetch_ = fetch;
	if (result.has_errors())
	{
		return result;
	}
	htab_ptr arval = result.value_.zarray();

	if (arval.ok())
	{
		arval = arval.get((int)0);
		if (arval.ok())
		{
			result.value_ = arval.get((int)0);
			return result;
		}
	}
	result.error() << "lastSeqValue fail: " << name;
	return result;

}

htab_return  //virtual
Pgsqlfn::getTableNames()
{
	obj_return h = handle();
	htab_return result;

	if (h.has_errors())
	{
		result = h.move_error();
		return result;
	}
	obj_ptr conn = h.value_;

	obj_rc robj = pg_query(conn, Pgfi.table_names_q);
	if (robj.ok())
	{
		result.value_ = pg_fetch_all_columns(robj);
		pg_free_result(robj);
	}
	else {
		result.error() << pg_last_error(conn);
	}
	return result;
}


bool  //virtual
Pgsqlfn::begin() 
{
	str_rc sql("BEGIN");
	val_return result = querySingle(sql);
    if (!result.has_errors() && !result.value_.isFalse()) 
    {
        inTransaction_ = true;
        return true;
    }
    else {
    	result.error() << "begin failed";
    	result.throw_errors();
    }
    return false;
}

bool  //virtual
Pgsqlfn::commit()
{
	str_rc sql("COMMIT");
	inTransaction_ = false;
	val_return result = querySingle(sql);
    if (!result.has_errors() && !result.value_.isFalse()) 
    {
        inTransaction_ = true;
        return true;
    }
    else {
    	result.error() << "commit failed";
    	result.throw_errors();
    }
    return false;	
}

bool //virtual
Pgsqlfn::rollback()
{
	str_rc sql("ROLLBACK");
	inTransaction_ = false;
	val_return result = querySingle(sql);
    if (!result.has_errors() && !result.value_.isFalse()) 
    {
        inTransaction_ = true;
        return true;
    }
    else {
    	result.error() << "rollback failed";
    	result.throw_errors();
    }
    return false;		
}

error_return //virtual
Pgsqlfn::closeStmt(obj_ptr sobj)
{
	// ?? check object class
	error_return result;
	if (sobj.ok())
	{
		if (sobj.instanceof(PgQuery::omg.class_entry_))
		{
			PgQuery* pg = zobj_toc<PgQuery>(sobj);
			pg->close();
		}
		else if (sobj.instanceof(PGfn.pgsql_result_ce))
		{
			pg_free_result(sobj);
		}
		else {
			result.error() << "unknown object " << sobj.className();
		}
	}
	return result;
}

Pgsqlfn::Pgsqlfn()
{
	idseq_  = 0;
	inTransaction_ = false;
}


void 
Pgsqlfn::register_class(zend_class_entry* idriver_ce)
{

	zend_class_entry* dclass = register_class_Wcd_Ext_Pgsqlfn(idriver_ce);

	Pgsqlfn::omg.classEntry(dclass);

	register_class_Wcd_Ext_Pgs_PgQuery();

}

}//namespace wcd

using namespace zpp;
using namespace wcd;


ZEND_METHOD(Wcd_Ext_Pgs_PgQuery, __construct)
{
	zarg_rd args(execute_data);
	obj_ptr pgconnect = args.obj(args.need(0));
	str_ptr query = args.str(args.need(1));

	if (!args.throw_errors())
	{
		PgQuery* cobj = zval_toc<PgQuery>(ZEND_THIS);
		cobj->construct(pgconnect, query);
	}
}

ZEND_METHOD(Wcd_Ext_Pgs_PgQuery, __destruct)
{
	ZEND_PARSE_PARAMETERS_NONE();
	PgQuery* cobj = zval_toc<PgQuery>(ZEND_THIS);
	cobj->destruct();
}

ZEND_METHOD(Wcd_Ext_Pgs_PgQuery, setParams)
{
	zarg_rd args(execute_data);
	htab_ptr params = args.htab(args.need(0));
	if (!args.throw_errors())
	{
		PgQuery* cobj = zval_toc<PgQuery>(ZEND_THIS);
		cobj->setParams(params);
	}
}

ZEND_METHOD(Wcd_Ext_Pgs_PgQuery, execute)
{
	zarg_rd args(execute_data);
	bool    usePgResult = true;
	args.zbool(usePgResult, args.option(0));
	if (!args.throw_errors())
	{
		PgQuery* cobj = zval_toc<PgQuery>(ZEND_THIS);
		cobj->execute(usePgResult);
	}
}


ZEND_METHOD(Wcd_Ext_Pgsqlfn, nextId)
{
	ZEND_PARSE_PARAMETERS_NONE();
	Pgsqlfn* cobj = zval_toc<Pgsqlfn>(ZEND_THIS);
	zend_long result = cobj->nextId();
	RETURN_LONG(result);
}

ZEND_METHOD(Wcd_Ext_Pgsqlfn, attribute)
{
	zarg_rd args(execute_data);
	str_rc  result;
	str_ptr name = args.str(args.need(0));
	str_ptr value = args.str(args.need(1));
	if (!args.throw_errors())
	{
		result = Pgsqlfn::attribute(name, value);
		result.move_zv(return_value);
	}
}

PHP_MINIT_FUNCTION(Wcd_Pgsqlfn_reg)
{
	STATE_INIT_ADD(Pgfi)


	



	return SUCCESS;
}

#endif