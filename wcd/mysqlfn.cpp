#ifndef MYSQLFN_WDC_CPP
#define MYSQLFN_WDC_CPP

#ifndef MYSQLFN_WDC_H
#include "mysqlfn.h"
#endif

namespace wcd {

using namespace zpp;

base_obj_mgr<Mysqlfn> Mysqlfn::omg;

class MSFInit : public state_init {
public:
	str_intern mysql_s;

	str_intern mysqli_s;
	str_intern report_mode_s;
	str_intern mysqli_query_s;
	str_intern mysqli_prepare_s;
	str_intern mysqli_commit_s;

	str_intern set_charset_s;

	str_intern fetch_array_s;
	str_intern fetch_object_s;
	str_intern fetch_assoc_s;
	str_intern mysqli_fetch_all_s;
	str_intern real_escape_string_s;
	str_intern insert_id_s;

	str_intern begin_transaction_s;
	str_intern begin_commit_s;

	str_intern bind_param_s;
	str_intern stmt_execute_s;
	str_intern stmt_close_s;

	str_intern mysqli_rollback_s;
	str_intern mysqli_close_s;



	void init() override;
	void init_req() override;

};

MSFInit Msfi;

class MsFnTable {
public:
	bool            configured_;

	fn_call         mysqli_commit_fn;
	fn_call         mysqli_rollback_fn;
	fn_call         fetch_array_fn;
	fn_call			set_charset_fn;
	fn_call			mysqli_query_fn;
	fn_call         mysqli_prepare_fn;
	fn_call         mysqli_close_fn;
	fn_call         report_fn;
	fn_call         begin_transaction_fn;
	fn_call         mysqli_fetch_all_fn;
	fn_call         fetch_assoc_fn;
	fn_call         real_escape_string_fn;
	fn_call         insert_id_fn;
	fn_call         bind_param_fn;
	fn_call         stmt_execute_fn;
	fn_call         stmt_close_fn;


	MsFnTable() : configured_(false) {}

	void init();

	void initFn(MSFInit& ms);

};





void MsFnTable::init()
	{
		if (configured_) {
			return;
		}
		configured_ = true;
		initFn(Msfi);
	}

void MsFnTable::initFn(MSFInit& ms)
	{
		begin_transaction_fn.set_fci(ms.begin_transaction_s);
		mysqli_commit_fn.set_fci(ms.mysqli_commit_s);
		mysqli_rollback_fn.set_fci(ms.mysqli_rollback_s);
		mysqli_fetch_all_fn.set_fci(ms.mysqli_fetch_all_s);

		fetch_array_fn.set_fci(ms.fetch_array_s);
		fetch_assoc_fn.set_fci(ms.fetch_assoc_s);
		insert_id_fn.set_fci(ms.insert_id_s);

		bind_param_fn.set_fci(ms.bind_param_s);
		stmt_execute_fn.set_fci(ms.stmt_execute_s);
		stmt_close_fn.set_fci(ms.stmt_close_s);

		mysqli_query_fn.set_fci(ms.mysqli_query_s);
		report_fn.set_fci(ms.report_mode_s);

		real_escape_string_fn.set_fci(ms.real_escape_string_s);
		set_charset_fn.set_fci(ms.set_charset_s);
		mysqli_close_fn.set_fci(ms.mysqli_close_s);

		mysqli_prepare_fn.set_fci(ms.mysqli_prepare_s);

	}




void
MSFInit::init()
{
	mysql_s = "mysql";
	mysqli_s = "mysqli";
	report_mode_s = "mysqli_report";

	mysqli_prepare_s = "mysqli_prepare";
	mysqli_query_s = "mysqli_query";

	set_charset_s = "mysqli_set_charset";

	mysqli_commit_s = "mysqli_commit";
	begin_transaction_s = "mysqli_begin_transaction";
	mysqli_fetch_all_s = "mysqli_fetch_all";

	fetch_array_s = "mysqli_fetch_array";
	fetch_assoc_s = "mysqli_fetch_assoc";
	fetch_object_s = "mysqli_fetch_object";

	bind_param_s = "mysqli_stmt_bind_param";
	stmt_execute_s = "mysqli_stmt_execute";
	stmt_close_s = "mysqli_stmt_close";

	insert_id_s = "mysqli_insert_id";
	real_escape_string_s = "mysqli_real_escape_string";
	mysqli_rollback_s = "mysqli_rollback";
}




thread_local MsFnTable MSfn;




bool 
mysqli_begin_transaction(obj_ptr msi, int flags, str_ptr name)
{
	fn_params<3> fn(MSfn.begin_transaction_fn);
	zval* pz = fn.argsptr();
	val_ptr::object_bind(pz, msi);

	pz++; ZVAL_LONG(pz, flags);

	pz++; val_ptr::string_bind(pz, name);
	return fn.zbool();
}

bool mysqli_stmt_bind_param(obj_ptr stmt, str_ptr types, htab_ptr params)
{
	fn_varparams fn(MSfn.bind_param_fn, params.size() + 2);
	zval* pz = fn.argsptr();

	val_ptr::object_bind(pz, stmt);
	pz++; val_ptr::string_bind(pz, types);

	for_key_value wk;
	for(wk.start(params); wk.ok(); wk.next())
	{
		pz++;  ZVAL_COPY_VALUE(pz, wk.value());
	}
	return fn.zbool();
}

bool 
mysqli_stmt_execute(obj_ptr stmt, htab_ptr params)
{
	fn_params<2> fn(MSfn.stmt_execute_fn);
	zval* pz = fn.argsptr();
	val_ptr::object_bind(pz, stmt);
	pz++;
	if (params.ok())
	{
		val_ptr::array_bind(pz, params);
	}
	else {
		ZVAL_NULL(pz);
	}
	return fn.zbool();
}

bool
mysqli_commit(obj_ptr msi, int flags, str_ptr name)
{
	fn_params<3>  fn(MSfn.mysqli_commit_fn);
	zval* pz = fn.argsptr();
	val_ptr::object_bind(pz, msi);
	pz++; ZVAL_LONG(pz, flags);
	pz++; val_ptr::string_bind(pz, name);
	return fn.zbool();
}

bool 
mysqli_rollback(obj_ptr msi, int flags, str_ptr name)
{
	fn_params<3>  fn(MSfn.mysqli_rollback_fn);
	zval* pz = fn.argsptr();
	val_ptr::object_bind(pz, msi);
	pz++; ZVAL_LONG(pz, flags);
	pz++; val_ptr::string_bind(pz, name);
	return fn.zbool();

}

bool 
mysqli_set_charset(obj_ptr msi, str_ptr cset)
{
	fn_params<2>  fn(MSfn.set_charset_fn);
	zval* pz = fn.argsptr();
	val_ptr::object_bind(pz, msi);
	pz++; val_ptr::string_bind(pz, cset);
	return fn.zbool();
}


val_rc
mysqli_query(obj_ptr msi, str_ptr query, int rmode)
{
	fn_params<3>  fn(MSfn.mysqli_query_fn);
	zval* pz = fn.argsptr();
	val_ptr::object_bind(pz, msi);
	val_ptr::string_bind(pz+1, query);
	ZVAL_LONG(pz+2, rmode);
	return fn.mixed();
}


bool 
mysqli_report(int flags)
{
	fn_params<1>  fn(MSfn.report_fn);
	zval* pz = fn.argsptr();
	ZVAL_LONG(pz, flags);
	return fn.zbool();
}

htab_rc 
mysqli_fetch_array(obj_ptr robj, int mode)
{
	fn_params<2>  fn(MSfn.fetch_array_fn);
	zval* pz = fn.argsptr();
	val_ptr::object_bind(pz, robj);
	pz++;
	ZVAL_LONG(pz, mode);
	return fn.array();
}

htab_rc 
mysqli_fetch_assoc(obj_ptr robj)
{
	fn_params<1>  fn(MSfn.fetch_array_fn);
	val_ptr::object_bind(fn.argsptr(), robj);
	return fn.array();
}

htab_rc
mysqli_fetch_all(obj_ptr robj, int mode)
{
	fn_params<2>  fn(MSfn.mysqli_fetch_all_fn);
	zval* pz = fn.argsptr();
	val_ptr::object_bind(pz, robj);
	ZVAL_LONG(pz+1, mode);
	return fn.array();
}

obj_rc 
mysqli_fetch_object(obj_ptr robj, str_ptr cname, htab_ptr args)
{
	fn_params<3>  fn(MSfn.fetch_array_fn);
	zval* pz = fn.argsptr();
	val_ptr::object_bind(pz, robj);
	str_rc class_name;
	if (cname.ok())
	{
		class_name = cname;
	}
	else {
		class_name = "stdClass";
	}
	pz++; val_ptr::string_bind(pz, cname);

	if (!args.ok())
	{
		args = htab_ptr::empty_array();
	}
	pz++; val_ptr::array_bind(pz, args);
	return fn.obj();
}

val_rc 
mysqli_insert_id(obj_ptr msi)
{
	fn_params<1>  fn(MSfn.insert_id_fn);
	val_ptr::object_bind(fn.argsptr(), msi);
	return fn.mixed();
}

str_rc 
mysqli_real_escape_string(obj_ptr msi, str_ptr str)
{
	fn_params<2>  fn(MSfn.real_escape_string_fn);
	zval* pz = fn.argsptr();
	val_ptr::object_bind(pz, msi);
	pz++; val_ptr::string_bind(pz, str);
	return fn.str();
}

val_rc
mysqli_prepare(obj_ptr msi, str_ptr sql)
{
	fn_params<2>  fn(MSfn.mysqli_prepare_fn);
	zval* pz = fn.argsptr();
	val_ptr::object_bind(pz, msi);
	val_ptr::string_bind(pz+1, sql);
	return fn.mixed();
}

void 
register_class(zend_class_entry* idriver_ce)
{
	
}


/*
htab_rc 
allRows(obj_ptr result, zend_long fmode = IDriver::FETCH_ASSOC)
{
	
}

htab_rc 
resultObjects(obj_ptr result)
{
	
}

htab_rc 
resultNum(obj_ptr result)
{
	
}
*/
htab_rc 
Mysqlfn::resultObjects(obj_ptr robj)
{
	htab_rc rows;
	htab_rw wr(rows);

	obj_rc obj;

	while(true)
	{
		obj = mysqli_fetch_object(robj);
		if (!obj.ok())
		{
			break;
		}
		else {
			wr.push_back(obj);
		}
	}
	return rows;
}

val_return 
Mysqlfn::getResults(obj_ptr robj, int mode)
{
	val_return result;

	if (!robj.ok())
	{
		result.error() << "getResults not a mysqli::result";
		return result;
	}

	htab_rc test;
	switch(mode)
	{
	case IDriver::FETCH_OBJECT:
		test = Mysqlfn::resultObjects(robj);
		break;
	case IDriver::FETCH_NUM:
		test = mysqli_fetch_array(robj);
		break;
	case IDriver::FETCH_ASSOC:
	default:
		test = mysqli_fetch_assoc(robj);
		break;

	}
	if (test.ok())
	{
		result.value_ = test;
	}
	return result;
}

/*
str_rc 
attribute(str_ptr name, str_ptr value)
{
	
}
*/

val_return 
Mysqlfn::fetchRow(obj_ptr stmt, int fmode)
{
	val_return result;
	return result;
}

bool 
Mysqlfn::begin(htab_ptr args)
{
	if (inTransaction_)
	{
		return false; // throw error
	}
	unsigned argct = args.size();

	zend_long flags = 0;
	str_rc    name;

	if (argct > 0)
	{
		val_ptr test = args.get((int) 0);
		flags = test.zlong();
	}
	if (argct > 1)
	{
		name = args.get((int) 1);
	}


	bool ok = mysqli_begin_transaction(handle_, flags, name);

	inTransaction_ = ok;

	return ok;
}

error_return 
Mysqlfn::bind(obj_ptr stmt, htab_ptr params)
{

	error_return result;

	str_buf type;

	for_key_value wkv;

	for(wkv.start(params); wkv.ok(); wkv.next())
	{
		val_ptr param = wkv.value();
		int ztype = param.ztype();
		switch (ztype)
		{
		case IS_LONG:
			type << 'i';
			break;
		case IS_DOUBLE:
			type << 'd';
			break;
		case IS_STRING:
		default:
			type << 's';
			break;
		}
	}
	str_rc tstr = type.zstr();

	if (tstr.size())
	{
		if (!mysqli_stmt_bind_param(stmt, tstr, params))
		{
			result.error() << "bind params error";
		}
	}
	return result;
}

void 
Mysqlfn::close()
{
	if (handle_.ok())
	{
		mysqli_close(handle_);
		handle_.init();
	}
}

error_return 
Mysqlfn::closeStmt(obj_ptr stmt)
{
	error_return result;

	if (stmt.ok())
	{
		mysqli_stmt_close(stmt);
	}
	else {
		result.error() << "Passed invalid object to Mysqlfn::closeStmt";

	}
	return result;
}

bool 
Mysqlfn::commit(htab_ptr args)
{
	unsigned nct = args.size();

	int flags = 0;
	str_rc name;

	if (nct > 0)
	{
		val_ptr test = args.get((int)0);
		flags = test.zlong();
	}
	if (nct > 1)
	{
		name = args.get((int)1);
	}

	return mysqli_commit(handle_, flags, name);
}

error_return 
Mysqlfn::connect()
{
	IConfig* cfg = zobj_toc<IConfig>(this->iconfig());

	str_rc host = cfg->getHost();
	str_rc dbname = cfg->getDatabase();
	str_rc user = cfg->getUsername();
	str_rc pwd = cfg->getPassword();
	//val_rc portval = cfg->getPort();

	str_rc charset = cfg->getCharset();
	str_rc collation = cfg->getCollation();


	htab_rc cparams;

	htab_rw cp(cparams);

	cp.push_items(host, user, pwd, dbname);

	obj_rc hconnect = ReflectCache::staticInstanceArgs(Msfi.mysqli_s, cparams);

	error_return result;

	if (!hconnect.ok())
	{

		result.error() << "Failed to connect " << user
		 << " to host: " << host << " db: " << dbname;
		return result;
	}

	mysqli_report(MYSQLI_REPORT_ERROR|MYSQLI_REPORT_STRICT);

	handle_ = hconnect;

	mysqli_set_charset(hconnect, charset );
	
	if (collation.ok())
	{
		str_buf buf;
		buf << "SET collation_connection=" << collation;

		mysqli_query(hconnect, buf.zstr());
	}
	return result;	
}

str_rc 
Mysqlfn::escape(str_ptr value)
{
	return mysqli_real_escape_string(handle_, value);
}

val_return 
Mysqlfn::execute(obj_ptr stmt, bool close, bool fetch)
{
	val_return result;
	obj_rc robj; // result object

	bool test = mysqli_stmt_execute(stmt);
	if (!test)
	{
		result.error() << "stmt_execute failed";
		return result;
	}
	if (fetch)
	{

		robj = mysqli_stmt_get_result(stmt);
		if (!robj.ok())
		{
			result.error() << "stmt_get_result failed";
			return result;
		}
	}
	else {
		result.value_ = mysqli_stmt_affected_rows(stmt);
	}

	if (close)
	{
		test = mysqli_stmt_close(stmt);
	}

	result = this->getResults(robj, ifetch_);

	return result;
}

str_rc 
Mysqlfn::getSqlType()
{
	return Msfi.mysql_s;
}

htab_return 
Mysqlfn::getTableNames()
{
	/*$result = $this->handle->query('SHOW TABLES');
        $rows = $result->fetch_all(MYSQLI_NUM);
        $reduce = [];
        foreach ($rows as $row) {
            $reduce[] = $row[0];
        }
        return $reduce;*/
	str_rc sql("SHOW TABLES");
	obj_rc rdata = mysqli_query(handle_, sql);

	htab_rc rows = mysqli_fetch_all(rdata);

	htab_return result;
	result.value_ = htab_ptr::empty_array();
	htab_rw reduce(result.value_);

	htab_walk wk;

	auto vptr = wk.value();
	for(wk.start(rows); wk.ok(); wk.next())
	{
		htab_ptr rval = vptr.zarray();

		reduce.push_back( rval.get(int(0)) );
	}


	return result;
}

bool 
Mysqlfn::inTransaction()
{
	return inTransaction_;
}

val_return 
Mysqlfn::lastInsertId(str_ptr name)
{
	return mysqli_insert_id(handle_);
}

val_return 
Mysqlfn::lastSeqValue(str_ptr name)
{
	val_return result;

	str_buf buf;

	buf << "select preview value for " << name;

	val_rc test = mysqli_query(handle_, buf.zstr(), MYSQLI_NUM);

	htab_ptr rows = test.zarray();

	if (rows.size())
	{
		htab_ptr r0 = rows.get((int)0);
		if (r0.size()) {
			result.value_ = r0.get((int)0);
		}
	}
	return result;
}

obj_return 
Mysqlfn::prepare(str_ptr query, htab_ptr options)
{
	last_sql_ = query;

	obj_rc stmt = mysqli_prepare(handle_, query);

	return stmt;
}

val_return 
Mysqlfn::querySingle(str_ptr query)
{
	val_return result;

	last_sql_ = query;

	val_rc test = mysqli_query(handle_, query);

	if (test.isObject()) {
		result = Mysqlfn::getResults(test.zobject(), ifetch_);
	}
	else {
		result.value_ = test;
	}

	return result;

}

bool 
Mysqlfn::rollback(htab_ptr args)
{

	unsigned nct = args.size();

	int flags = 0;
	str_rc name;

	if (nct > 0)
	{
		val_ptr test = args.get((int)0);
		flags = test.zlong();
	}
	if (nct > 1)
	{
		name = args.get((int)1);
	}
	return mysqli_rollback(handle_, flags, name);
}

Mysqlfn::Mysqlfn()
{
	MSfn.init();
}

Mysqlfn::~Mysqlfn()
{

}

}; // namespace wcd



#endif