#ifndef WCD_PDODRIVER_CPP
#define WCD_PDODRIVER_CPP

#ifndef WCD_PDODRIVER_H
#include "pdodriver.h"
#endif

#ifndef PDO_PGSQL_ARGINFO
#define PDO_PGSQL_ARGINFO
extern "C" {
	#include "stub/pdo_pgsql_arginfo.h"
}
#endif

#ifndef ICACHE_H
#include "wcc/icache.h"
#endif

#ifndef WCC_SERVICES_H
#include "wcc/services.h"
#endif

#ifndef WCD_RUNSQL_H
#include "runsql.h"
#endif

#ifndef SQL_ISQL_H
#include "sql_isql.h"
#endif

#ifndef WCD_MODEL_H
#include "model.h"
#endif

#ifndef WCD_ISQL_H
#include "isql.h"
#endif

#ifndef WCD_PDO_PGSQL_H
#include "pdo_pgsql.h"
#endif

#ifndef DIR_CACHE_H
#include "wcc/dircache.h"
#endif

namespace wcd {


using namespace zpp;

base_obj_mgr<PdoDriver> PdoDriver::omg;

PdoInit PDOI;

void PdoInit::init() {
	setattribute_fn = "setattribute";
	getattribute_fn = "getattribute";
}

str_return 
PdoDriver::getDSN()
{
	str_return result;
	str_rc dname = icfg_c()->getDriverName();
	
	dname.lowercase();

	if (dname.starts_with(DBS.pdo_prefix)) {
		dname = dname.substr(4);
	}
	else {
		result.error() << "PDO Driver names need to begin with 'pdo_' : " << dname;
		return result;
	}

	IConfig* cfg = icfg_c();
	str_rc host = cfg->getHost();
	str_rc dbname = cfg->getDatabase();

	str_buf buf;

	buf << dname << ':' << "host=" << host
	    << ";dbname=" << dbname;
	result.value_ = buf.zstr();

	return result;
}

htab_rc 
PdoDriver::getConnectOptions()
{
	htab_rc result;

	htab_rw options(result);

	options.push_back((int)PDO_ATTR_ERRMODE);
	options.push_back((int)PDO_ERRMODE_EXCEPTION);
	return result;
}



error_return 
PdoDriver::connect()
{
	error_return result;

	

	if (handle_.ok())
	{
		return result;
	}

	str_return dsn = getDSN();

	if (dsn.has_errors())
	{
		result = std::move(dsn);
		return result;
	}

	htab_rc options = getConnectOptions();

	IConfig* cfg = icfg_c();

	str_rc user = cfg->getUsername();
	str_rc pw = cfg->getPassword();

	htab_rc  args_mgr;
	htab_rw args(args_mgr);

	args.push_back(dsn.value_);
	args.push_back(user);
	args.push_back(pw);
	args.push_back(options);

	obj_rc h = ReflectCache::staticInstanceArgs(DBS.pdo_class, args_mgr);

	if (!h.ok())
	{
		result.error() << "PDO connect handle failed for " << dsn.value_;
	}
	else {
		handle_ = h;

		afterConnect();
	}

	return result;
}


void PdoDriver::afterConnect()
{
 	//zend_printf("Connected!\n");
}

int //static
PdoDriver::pdoType(val_ptr val)
{
	switch(val.ztype())
	{
	case IS_LONG:
		return PDO_PARAM_INT;
	case IS_TRUE:
	case IS_FALSE:
		return PDO_PARAM_BOOL;
	case IS_STRING:
	default:
		return PDO_PARAM_STR;
	}
}


htab_return 
PdoDriver::query_lcase(str_ptr sql, int fmode)
{
	val_rc attr_val = getCaseAttribute();
	long attr = attr_val.zlong();

	if (attr != PDO_CASE_LOWER)
	{
		setCaseAttribute(PDO_CASE_LOWER);
	}	

	htab_return result;
	obj_return stmt = prepareQuery(sql, htab_ptr(), htab_ptr());

	if (stmt.has_errors())
	{
		result = stmt.move_error();
		return result;
	}
	result = fetchAllRows(stmt.value_, fmode);
	if (attr != PDO_CASE_LOWER)
	{
		setCaseAttribute(attr);
	}
	return result;
}

bool 
PdoDriver::commit(htab_ptr args)
{
	obj_return pdo = handle();

	if (pdo.throw_errors())
	{
		return false;
	}
	obj_ptr h = pdo.value_;
	{
		val_rc result = h.call(DBS.commit_fn);
		return result.isTrue();
	}
	return false;
}

str_rc 
PdoDriver::quoteName(str_ptr name)
{
	return isql_c()->quoteName(name);
}	

error_return 
PdoDriver::closeStmt(obj_ptr sobj)
{
	error_return result;
	val_rc test = sobj.call(DBS.close_cursor);
	if (!test.isTrue()) {
		result.error() << "error in " << DBS.close_cursor;
	};
	return result;
}

str_rc 
PdoDriver::param(unsigned pno)
{
	str_buf buf;

	buf << ":p" << Numf::DEC << pno;

	return buf.zstr();
}



error_return 
PdoDriver::bind(obj_ptr stmt, htab_ptr params)
{
	error_return result;

	if (params.size())
	{
		// construct a reused call to method "bindValue" for spdo statement object
		fn_call bind_call(DBS.bind_value, stmt);
		fn_params<3> bvcall(bind_call);
		
		if (params.is_list())
		{
			for_key_value wk;
		
			for(wk.start(params); wk.ok(); wk.next())
			{
				int pno = wk.index() + 1;
				val_ptr value = wk.value();
				// clear the buffers
				zval* args = bvcall.argsptr(); 
				ZVAL_LONG(args, pno);
				ZVAL_COPY_VALUE(args+1, value);

				int ptype = pdoType(value);
				ZVAL_LONG(args+2, ptype);
				
				if (!bvcall.zbool())
				{
					result.error() << "Indexed value " << pno << " not bound\n";
				}
			}
		}
		else {
			for_key_value wk;

			for(wk.start(params); wk.ok(); wk.next())
			{
				zval* args = bvcall.argsptr();

				str_rc key = wk.key();
				val_ptr::string_bind(args, key);
				val_ptr value = wk.value();
				ZVAL_COPY_VALUE(args+1, value);
				ZVAL_LONG(args+2, pdoType(value));
				if (!bvcall.zbool())
				{
					result.error() << "Param " << key << " failed bind\n";
				}
			}
		}
	}
	return result;
}

val_rc 
PdoDriver::getCaseAttribute()
{
	return getAttribute(PDO_ATTR_CASE);
}

void 
PdoDriver::setCaseAttribute(int value)
{
	val_rc arg(value);

	setAttribute(PDO_ATTR_CASE, arg);
}

bool 
PdoDriver::begin(htab_ptr args)
{
	obj_return pdo = handle();

	if (pdo.throw_errors())
	{
		return false;
	}

	val_rc test = pdo.value_.call(DBS.begin_trans);
	return test.isTrue();
}

str_rc 
PdoDriver::escape(str_ptr value)
{
	str_rc result;

	obj_return pdo = handle();

	if (pdo.throw_errors())
	{
		return false;
	}

	obj_ptr h(pdo.value_);
	val_rc  arg(value);
	
	result = h.call(DBS.quote_fn, arg);
	result = preg_replace(DBS.regex_quoted, DBS.rx_cap1, result);
	return result;
}

val_return
PdoDriver::execute(obj_ptr stmt, bool close, bool fetch)
{

	obj_ptr sobj(stmt);

	val_rc pdo_result = sobj.call(DBS.execute_fn);
	val_return result;

	//showmem("execute call PDO ", pdo_result);
	bool good_result = pdo_result.isTrue();

	if (good_result)
	{
		if (fetch) {
			val_rc farg(ifetch_);

			result.value_ = sobj.call(DBS.fetchall_fn, farg);
		}
		else {
			result.value_ = sobj.call(DBS.rowcount_fn);
		}
	}
	if (close || !good_result)
	{
		closeStmt(stmt);

		if (isAutoCommit() && inTransaction())
		{
			commit(htab_ptr());
		}
	}

	if (good_result) {
		if (fetch)
		{
			error_return derr = IDriver::check_results(result.value_);
			if (derr.has_errors())
			{
				result = std::move(derr);
			}
		}
	}
	else {
		result.error() << "SQL Execute failure";
		result.value_.set_bool(false);
	}

	return result;
}

htab_return 
PdoDriver::fetchAllRows(obj_ptr sobj, int mode)
{
	htab_return result;

	if (sobj.ok())
	{
		val_rc farg(mode);
		val_rc test = sobj.call(DBS.fetchall_fn, farg);
		if (test.isArray())
		{
			result.value_ = test.zarray();
		}
		else {
			result.value_ = htab_rc::empty_array();
		}
	}
	else {
		result.error() << "Null statement";
		return result;
	}
	return result;
}

val_return 
PdoDriver::fetchRow(obj_ptr sobj, int mode)
{
	val_return result;
	val_rc farg(mode);
	result.value_ = sobj.call(DBS.fetch_fn, farg);
	return result;
}



val_rc 
PdoDriver::getAttribute(int key)
{
	val_rc result;

	obj_return h = handle();

	if (h.throw_errors())
	{
		return result;
	}

	val_rc arg(key);
	obj_ptr pdo(h.value_);
	return pdo.call(PDOI.getattribute_fn, arg);

}

val_return 
PdoDriver::querySingle(str_ptr query)
{
	val_return result;

	obj_return h = handle();

	if (h.has_errors())
	{
		result = h.move_error();
		return result;
	}

	obj_ptr pdo = h.value_;

	val_rc arg1(query);
	obj_rc stmt = pdo.call(DBS.query_fn, arg1);

	if (!stmt.ok()) // Not an object
	{
		result.error() << "pdo query fail " << query;
		return result;
	}
	arg1 = (zend_long) ifetch_;
	val_rc rows = stmt.call(DBS.fetchall_fn, arg1);
	val_rc closed = stmt.call(DBS.close_cursor);

	if (!rows.isFalse())
	{
		result.value_ = std::move(rows);
	}
	else {
		result.value_ = closed;
	}

	return result;
} 


bool 
PdoDriver::inTransaction()
{
	obj_return h = handle();
	if (h.throw_errors())
	{
		return false;
	}
	val_rc result = h.value_.call(DBS.intransaction_fn);
	return result.isTrue();
}


bool 
PdoDriver::isConnected()
{
	obj_return h = handle();
	return !h.has_errors() && h.value_.ok();
}


val_return 
PdoDriver::lastInsertId(str_ptr name)
{
	val_return result;
	result = handle();
	if (result.has_errors())
	{
		return result;
	}
	obj_ptr pdo =result.value_.zobject();
	val_rc  arg;

	if (name.ok())
	{
		arg = name;
	}
	result.value_ = pdo.call(DBS.lastinsertid_fn, arg);
	if (result.value_.isFalse())
	{
		result.error() << "No lastInsertId for " << name;
	}
	return result;
}


obj_return
PdoDriver::prepare(str_ptr query, htab_ptr options)
{
	//zend_printf("PdoDriver::prepare-- ");
	//showstr("query ", query);
	//showarray("options", options);

	obj_return h;
	

	h = handle();
	if (h.has_errors())
	{
		return h;
	}
	obj_rc pdo(h.value_);


	obj_ptr(self_).property(DBS.lastsql_s, query);

	//showmem("lastsql", lastsql_ptr_);


	obj_return stmt;
	if (!options.ok())
	{
		options = htab_ptr::empty_array();
	}
	val_rc     arg2(options);
	//showarray("options arg", options);
	val_rc     arg1(query);

	val_rc test = pdo.call(DBS.prepare_fn, arg1, arg2);
	
	if (!test.isObject())
	{
		//showobj("PDO is ", pdo);
		//showstr("call to ", DBS.prepare_fn);
		//showmem("prepare returned", stmt.value_);
		str_rc code = pdo.call(DBS.errorcode_fn);
		stmt.error() << "PDO errorcode: " << code << endl;
		stmt.error() << "Prepare fail: " << query;
	}
	stmt.value_ = test.zobject();
	return stmt;
}


obj_return
PdoDriver::prepareQuery(str_ptr query, htab_ptr values, htab_ptr bindTypes)
{
	obj_return result;

	result = prepare(query);
	
	if (result.has_errors())
	{
		return result;
	}

	obj_ptr stmt(result.value_);
    //showobj("stmt ", stmt);
	val_rc test;
	val_rc temp;
	val_rc null;

	if (values.size())
	{
		if (values.has_index(0))
		{
			temp = values;
			test = stmt.call(DBS.execute_fn, temp);
		}
		else {
			htab_walk wk;
			auto key = wk.key();
			auto val = wk.value();
			for(wk.start(values); wk.ok(); wk.next())
			{
				if (bindTypes.size())
				{
					temp = bindTypes.get(key);
				}
				else {
					temp = (zend_long)pdoType(val);
				}
				stmt.call(DBS.bind_value, val, temp);
			}
			test = stmt.call(DBS.execute_fn, null);
		}
	}
	else {
		test = stmt.call(DBS.execute_fn, null);
	}
	if (!test.ok())
	{	
		result.error() << "Statement execute failed: " << query;
	}
	//showmem("prepareQuery test", test);
	return result;
}

val_return
PdoDriver::query(str_ptr query, htab_ptr params)
{
	htab_ptr btypes;
	val_return result;

	obj_return stmt_ret = prepareQuery(query, params, btypes);

	if (stmt_ret.has_errors())
	{
		result = stmt_ret.move_error();
		return result;
	}

	obj_ptr stmt = stmt_ret.value_;

	val_rc data = stmt.call(DBS.fetchall_fn);

	stmt.call(DBS.close_cursor);

	error_return check = check_results(data);

	if (check.has_errors()) 
	{
		result.value_.set_bool(false);
		result = std::move(check);
	}
	else {
		result.value_ = data;
	}

	return result;
}

bool 
PdoDriver::rollback(htab_ptr args)
{
	obj_return h = handle();

	if (h.throw_errors())
	{
		return false;
	}

	val_rc result = h.value_.call(DBS.rollback_fn);
	return result.isTrue();
}


bool 
PdoDriver::setAttribute(int key, val_ptr value)
{
	obj_return h = handle();

	if (h.throw_errors())
	{
		return false;
	}

	val_rc arg1(key);
	val_rc result = h.value_.call(PDOI.setattribute_fn, arg1, value);

	return result.isTrue();
}

zend_class_entry*
PdoDriver::register_class(zend_class_entry* idriver_ce)
{
	zend_class_entry* dclass = register_class_Wcd_Ext_PdoDriver(idriver_ce);
	PdoDriver::omg.classEntry(dclass);

	STATE_INIT_ADD(PDOI);

	return dclass;
}

}//namespace wcd

using namespace wcd;



ZEND_METHOD(Wcd_Ext_PdoDriver, getAttribute)
{
	zend_long   key;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_LONG(key)
	ZEND_PARSE_PARAMETERS_END();

	PdoDriver* db = zval_toc<PdoDriver>(ZEND_THIS);	

	val_rc result = db->getAttribute(key);

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Ext_PdoDriver, getCaseAttribute)
{
	ZEND_PARSE_PARAMETERS_NONE();
	PdoDriver* db = zval_toc<PdoDriver>(ZEND_THIS);	

	val_rc result = db->getCaseAttribute();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Ext_PdoDriver, setCaseAttribute)
{
	zend_long value;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_LONG(value)
	ZEND_PARSE_PARAMETERS_END();


	PdoDriver* db = zval_toc<PdoDriver>(ZEND_THIS);	

	db->setCaseAttribute(value);

}


ZEND_METHOD(Wcd_Ext_PdoDriver, getConnectOptions)
{
	ZEND_PARSE_PARAMETERS_NONE();

	PdoDriver* db = zval_toc<PdoDriver>(ZEND_THIS);

	htab_rc result = db->getConnectOptions();
	result.move_zv(return_value);
}





ZEND_METHOD(Wcd_Ext_PdoDriver, pdoType)
{
	zarg_rd args(execute_data);

	val_ptr pval = args.need(0);
	long   result = -1;

	if (!args.throw_errors())
	{
		PdoDriver* cobj = zval_toc<PdoDriver>(ZEND_THIS);
		result = cobj->pdoType(pval);
	}
	RETURN_LONG(result);
}



ZEND_METHOD(Wcd_Ext_PdoDriver, query_lcase)
{
	htab_return result;
	str_ptr     sql;
	zend_long   fmode;

	zarg_rd args(execute_data);

	sql = args.str(args.need(0));
	args.zlong(fmode, args.need(1));

	if (!args.throw_errors())
	{
		PdoDriver* cobj = zval_toc<PdoDriver>(ZEND_THIS);	
		result = cobj->query_lcase(sql, fmode);
		if (!result.throw_errors())
		{
			result.value_.move_zv(return_value);
		}
	}	
}



//public function setAttribute(int $attkey, mixed $value)
ZEND_METHOD(Wcd_Ext_PdoDriver, setAttribute)
{
	zend_long attkey;
	zval*     value;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_LONG(attkey)
	Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	PdoDriver* db = zval_toc<PdoDriver>(ZEND_THIS);

	bool result = db->setAttribute(attkey, value);

	RETURN_BOOL(result);
}

#endif