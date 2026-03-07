#ifndef PGSQLFN_WDC_CPP
#define PGSQLFN_WDC_CPP

#ifndef PGSQLFN_WDC_H
#include "pgsqlfn.h"
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
	}
};

thread_local PgfnTable PGfn;


void PgInit::init()
{
	pg_free_result_fn = "pg_free_result";
	pg_fetch_array_fn = "pg_fetch_array";
	pg_fetch_object_fn = "pg_fetch_object";
	pg_fetch_assoc_fn = "pg_fetch_assoc";
	pg_fetch_all_fn = "pg_fetch_all_fn";
	pg_fetch_all_columns_fn = "pg_fetch_all_columns";
	pg_connect_fn = "pg_connect_fn";
	pg_close_fn = "pg_close_fn";
	pg_execute_fn = "pg_execute";
	pg_last_error_fn = "pg_last_error";
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
PgResource::construct(val_ptr reso)
{
	rhandle_ = reso;
}

void 	
PgResource::destruct(val_ptr reso)
{
	close();
}

void 
PgResource::close()
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
			obj_rc robj = PgResult::omg.new_zobj();
			PgResult* rp = zobj_toc<PgResult>(robj);
			rp->construct(test);
			result = robj;
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
PgResult::rowFetch(zend_long fmode)
{
	val_rc result;//init null

	switch(fmode)
	{
	case IDriver::FETCH_NUM:
		result = pg_fetch_array(rhandle_, result, PGSQL_NUM);
		break;
	case IDriver::FETCH_OBJ:
		result = pg_fetch_object(rhandle_);
		break;
	default:
		result =  pg_fetch_assoc(rhandle_);
	}
	return result;
}

htab_rc 
PgResult::allRows(zend_long fmode = IDriver::FETCH_ASSOC)
{
	htab_rc result;
	val_rc data;
	switch(fmode)
	{
	case IDriver::FETCH_OBJ:
		htab_rw rows(result);
		while(true) {
			data = pg_fetch_object(rhandle_);
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
		result = pg_fetch_all(rhandle_);
		break;
	case IDriver::FETCH_NUM:
		result = pg_fetch_all(rhandle_, PGSQL_NUM);
		break;
	}
	return result;
}

htab_rc 
PgResult::allColumns(zend_long colnum = 0)
{
	htab_rc result = pg_fetch_all_columns(rhandle_, colnum);
	return result;
}

obj_rc   
PgResult::objFetch()
{
	val_rc nullval;
	obj_rc result = pg_fetch_object(rhandle_);
	return result;
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

#endif