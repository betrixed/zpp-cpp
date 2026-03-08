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

namespace wcd {

PgInit   PGFN;


class PgfnTable {
	fn_call_args1 free_result;
	fn_call_args3 pg_prepare;
	fn_call_args3 pg_execute;
	fn_call_args1 pg_last_error;
	fn_call_args3 pg_fetch_array;
	fn_call_args4 pg_fetch_object;
	fn_call_args1 pg_fetch_assoc;
	fn_call_args2 pg_fetch_all;
	fn_call_args2 pg_fetch_all_columns;
	fn_call_args1 pg_close;
	fn_call_args2 pg_query;
	fn_call_args2 pg_escape_string;

	zend_class_entry*  pgsql_result_ce;

	void init(const PGInit &pg)
	{
		free_result.set_fci(pg.pg_free_result_fn);
		pg_prepare.set_fci(pg.pg_prepare_fn);
		pg_last_error.set_fci(pg.pg_last_error);
		pg_execute.set_fci(pg.pg_execute_fn);
		pg_last_error.set_fci(pg.pg_fetch_array_fn);
		pg_fetch_object.set_fci(pg.pg_fetch_object_fn);
		pg_fetch_assoc.set_fci(pg.pg_fetch_assoc_fn);
		pg_fetch_all.set_fci(pg.pg_pg_fetch_all_fn);
		pg_fetch_all_columns.set_fci(pg.pg_fetch_all_columns_fn);
		pg_close.set_fci(pg.pg_close_fn);
		pg_query.set_fci(pg.pg_query_fn);
		pg_escape_string.set_fci(pg.pg_escape_string_fn);


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
	pg_fetch_all_fn = "pg_fetch_all";
	pg_fetch_all_columns_fn = "pg_fetch_all_columns";
	pg_connect_fn = "pg_connect";
	pg_close_fn = "pg_close";
	pg_query_fn = "pg_query"
	pg_execute_fn = "pg_execute";
	pg_last_error_fn = "pg_last_error";
	pg_escape_string_fn = "pg_escape_string"

	pgsql_result_class = "pgsql\\result";

	squote_char = "'";
	esc_squote = "\\'";

	dbname_s = "dbname";
	host_s = "host";
	port_s = "port"
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

void pg_free_result(val_ptr h)
{
	fn_call_args1& fn = PGfn.free_result;
	zval* args = fn.argsptr();
	ZVAL_COPY_VALUE(args, h);
	fn.call_fn();	
}

obj_rc 
pg_prepare(val_ptr h, str_ptr id, str_ptr sql)
{
	fn_call_args3& fn = PGfn.pg_prepare;
	zval* zv = fn.argsptr();
	ZVAL_COPY_VALUE(zv, h);
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
	fn_call_args1& = PGfn.pg_last_error;
	zval* zv = fn.argsptr();
	ZVAL_OBJ(zv, connect);
	obj_rc result = fn.call_fn();
	return result;
}

val_rc 
pg_query(obj_ptr connect, str_ptr query)
{
	fn_call_args2& = PGfn.pg_query;
	zval* zv = fn.argsptr();
	ZVAL_OBJ(zv, connect);
	zv++;
	ZVAL_STR(zv, query);
	val_rc result = fn.call_fn();
	return result;
}

void pg_close(obj_ptr connect)
{
	fn_call_args1& = PGfn.pg_close;
	zval* zv = fn.argsptr();
	ZVAL_OBJ(zv, connect);
	fn.call_fn();
}

val_rc
pg_execute(obj_ptr connect, str_ptr sname, htab_ptr params)
{
	fn_call_args3& = PGfn.pg_execute;
	zval* zv = fn.argsptr();
	ZVAL_OBJ(zv,pghandle_);
	zv++;
	ZVAL_STR(zv, sname);
	zv++;
	ZVAL_ARR(zv, params);
	zv++;
	val_rc result = call_fn();
	return result;
}

val_rc 
pg_fetch_object(obj_ptr robj, val_ptr row = val_ptr::nullval(), 
			str_ptr cname = str_ptr(), htab_ptr args = htab_ptr::empty_array())
{
	fn_call_args1& = PGfn.pg_fetch_object;
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

val_rc 
pg_fetch_assoc(obj_ptr robj)
{
	fn_call_args1& = PGfn.pg_fetch_assoc;
	zval* zv = fn.argsptr();
	ZVAL_OBJ(zv,robj);
	val_rc result = fn.call_fn();
	return result;
}

val_rc
pg_fetch_array(obj_ptr robj, val_ptr row, int fmode =  PGSQL_BOTH)
{
	fn_call_args3& = PGfn.pg_fetch_array;
	zval* zv = fn.argsptr();
	ZVAL_OBJ(zv,robj);
	zv++;
	ZVAL_COPY_VALUE(zv, row);
	zv++;
	ZVAL_LONG(zv, fmode);
	zv++;
	val_rc result = call_fn();
	return result;
}

val_rc
pg_fetch_all(obj_ptr robj,int fmode = PGSQL_ASSOC)
{
	fn_call_args2& = PGfn.pg_fetch_all;
	zval* zv = fn.argsptr();
	ZVAL_OBJ(zv, robj);
	zv++;
	ZVAL_LONG(zv, fmode);
	zv++;	
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
		rhandle_.set_null();
	}
}

void 
PgQuery::construct(obj_ptr connect, str_ptr query)
{
	Pgsqlfn* pgc = zobj_toc<Pgsqlfn>(connect);
	val_return h = pgc->handle();

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
PgQuery::execute(bool asResult = true)
{
	val_rc result;
	val_rc test = pg_execute(pghandle_, id_, params_);
	if (!test.isFalse())
	{
		if (asResult) 
		{
			result.value_ = test;
		}
		else {
			result = pg_affected_rows(test);
			pg_free_result(test);
		}
	}
	else {
		result.error() << pg_last_error(pghandle_);
	}
	return result;
}

val_rc  
Pgsqlfn::rowFetch(obj_ptr pgresult, zend_long fmode)
{
	val_rc result;//init null

	switch(fmode)
	{
	case IDriver::FETCH_NUM:
		result = pg_fetch_array(pgresult, result, PGSQL_NUM);
		break;
	case IDriver::FETCH_OBJ:
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
	case IDriver::FETCH_OBJ:
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

	if (value.contains(PGFN.squote_char))
	{
		eval = str_replace(PGFN.squote_char, PGFN.esc_squote, value);
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

	cp.push_back(Pgsqlfn::attribute(PGFN.dbname_s, dbname));
	cp.push_back(Pgsqlfn::attribute(PGFN.host_s, host));
	cp.push_back(Pgsqlfn::attribute(PGFN.port_s, port.zstr()));
	cp.push_back(Pgsqlfn::attribute(PGFN.user_s, user));
	cp.push_back(Pgsqlfn::attribute(PGFN.pwd_s, pwd));

	str_rc cstr = implode(blank_s, cparams);
	val_rc hconnect = pg_connect(cstr);
	error_return result;

	if (hconnect.isObject())
	{
		val_ptr::try_decref(handle_ptr_);
		ZVAL_COPY(handle_ptr_, hconnect);
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
	return inTransaction_
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

val_return //virtual
Pgsqlfn::prepare(str_ptr query, htab_ptr options=htab_ptr())
{
	lastsql_ = query;
	obj_rc qobj = PgQuery::omg.new_zobj();
	PgQuery* zobj = zobj_toc<PgQuery>(qobj);
	zobj->construct(self_, lastsql_);
	return qobj;
}

val_return //virtual
Pgsqlfn::execute(val_ptr stmt, bool close = true, bool fetch = false)
{
	val_return result;

	PgQuery* zobj = zobj_toc<PgQuery>(stmt.zobject());
	var_rc test = zobj->execute(fetch);

	if (fetch && test.isObject())
	{
		PgResult* pgr = zobj_toc<PgResult>(test);
		result.value_ = pgr->allRows(ifetch_);
	}
	else {
		result.value_ = test;
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
	val_return result = handle();
	if (result.has_errors())
	{
		return result;
	}

	lastsql_ = query;

	val_rc robj = pg_query(result.zobject(), query);

	if (robj.isFalse())
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
	return PGFN.pgsql;
}


str_rc  //virtual
Pgsqlfn::escape(str_ptr value)
{
	str_rc result;
	val_return test = handle();
	if (result.has_errors())
	{
		test.throw_errors();
		return result;
	}
	pg_escape_string(handle_ptr_, value);
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
	if (result.has_errors())
	{
		return result;
	}
	htab_ptr arval = result.value_.zarray();

	if (arval.ok())
	{
		arval = arval.get((int)0);
		if (argval.ok())
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
	val_return h = handle();
	htab_return result;

	if (h.has_errors())
	{
		result = h.move_error();
		return result;
	}
	obj_ptr conn = h.zobject();

	obj_rc robj = pg_query(conn, PGFN.table_table_names_qnames_q);
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
    if (!result.has_errors() && !result.IsFalse()) 
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
    if (!result.has_errors() && !result.IsFalse()) 
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
    if (!result.has_errors() && !result.IsFalse()) 
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

void //virtual
Pgsqlfn::closeStmt(val_ptr stmt)
{
	// ?? check object class
	obj_ptr sobj = stmt.zobject();
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
	}
}

Pgsqlfn::Pgsqlfn()
{
	idseq_  = 0;
	inTransaction_ = false;
}


}//namespace wcd

using namespace zpp;
using namespace wcd;

ZEND_METHOD(Wcd_Ext_Pgs_PgResource, __construct)
{
	zarg_rd args(execute_data);
	obj_ptr handle = args.str(args.need(0));
	if (!args.throw_errors())
	{
		PgResource* cobj = zobj_toc<PgResource>(ZEND_THIS);
		cobj->construct(handle);
	}
}

ZEND_METHOD(Wcd_Ext_Pgs_PgResource, __destruct)
{
	ZEND_PARSE_PARAMETERS_NONE();
	PgResource* cobj = zobj_toc<PgResource>(ZEND_THIS);
	cobj->destruct();

}

ZEND_METHOD(Wcd_Ext_Pgs_PgResource, close)
{
	ZEND_PARSE_PARAMETERS_NONE();
	PgResource* cobj = zobj_toc<PgResource>(ZEND_THIS);
	cobj->close();
}

ZEND_METHOD(Wcd_Ext_Pgs_PgQuery, __construct)
{
	zarg_rd args(execute_data);
	obj_ptr pgconnect = args.obj(args.need(0));
	str_ptr query = args.str(args.need(1));

}

ZEND_METHOD(Wcd_Ext_Pgs_PgQuery, __destruct)
{
	ZEND_PARSE_PARAMETERS_NONE();
	PgQuery* cobj = zobj_toc<PgQuery>(ZEND_THIS);
	cobj->destruct();
}

ZEND_METHOD(Wcd_Ext_Pgs_PgQuery, setParams)
{
	zarg_rd args(execute_data);
	htab_ptr params = args.htab(args.need(0));
	if (!args.throw_errors())
	{
		PgQuery* cobj = zobj_toc<PgQuery>(ZEND_THIS);
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
		PgQuery* cobj = zobj_toc<PgQuery>(ZEND_THIS);
		cobj->execute(usePgResult);
	}
}

ZEND_METHOD(Wcd_Ext_Pgs_PgResult, rowFetch)
{
	zarg_rd args(execute_data);
	zend_long    fmode;
	val_rc       result;

	args.zlong(fmode, args.need(0));
	if (!args.throw_errors())
	{
		PgResult* cobj = zobj_toc<PgResult>(ZEND_THIS);
		result = cobj->rowFetch(usePgResult);
		result.move_zv(return_value);
	}	
}

ZEND_METHOD(Wcd_Ext_Pgs_PgResult, allRows)
{
	zarg_rd args(execute_data);
	htab_rc result;
	zend_long rtype;

	args.zlong_null(rtype, args.options(0), IDriver::FETCH_ASSOC);
	if (!args.throw_errors())
	{
		PgResult* cobj = zobj_toc<PgResult>(ZEND_THIS);
		result = cobj->allRows(rtype);
		result.move_zv(return_value);
	}
}

ZEND_METHOD(Wcd_Ext_Pgs_PgResult, allColumns)
{
	zarg_rd args(execute_data);
	htab_rc result;
	zend_long colNum = 0;
	args.zlong(rtype, args.nested(0));
	if (!args.throw_errors())
	{
		PgResult* cobj = zobj_toc<PgResult>(ZEND_THIS);
		result = cobj->allColumns(colNum);
		result.move_zv(return_value);
	}
}

ZEND_METHOD(Wcd_Ext_Pgs_PgResult, objFetch)
{
	ZEND_PARSE_PARAMETERS_NONE();
	PgResult* cobj = zobj_toc<PgResult>(ZEND_THIS);
	obj_rc    result = cobj->objFetch();
	result.move_zv(return_value);

}

ZEND_METHOD(Wcd_Ext_Pgsqlfn, nextId)
{
	ZEND_PARSE_PARAMETERS_NONE();
	Pgsqlfn* cobj = zobj_toc<Pgsqlfn>(ZEND_THIS);
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
	STATE_INIT_ADD(DBS)

	zend_class_entry* dclass = register_class_Wcd_IDriver();

	IDriver::omg.classEntry(dclass);

	class_data cval(IDriver::omg.class_entry_);

	cval.add_constant("FETCH_OBJECT", PDO_FETCH_OBJ);
	cval.add_constant("FETCH_ASSOC", PDO_FETCH_ASSOC);
	cval.add_constant("FETCH_NUM", PDO_FETCH_NUM);
	cval.add_constant("FETCH_COLUMN", PDO_FETCH_COLUMN);


	zend_class_entry* pdo = PdoDriver::register_class(dclass);

	Pdo_pgsql::register_class(pdo);
	Pdo_mysql::register_class(pdo);



	return SUCCESS;
}

#endif