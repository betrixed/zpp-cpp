#ifndef MYSQLFN_WDC_CPP
#define MYSQLFN_WDC_CPP

#ifndef MYSQLFN_WDC_H
#include "mysqlfn.h"
#endif

#ifndef MYSQLI_PRIV_H
extern "C" {
	#include <ext/mysqli/mysqli_priv.h>
}
#endif

namespace wcd {

using namespace zpp;


class MSFInit : public state_init {
public:
	str_intern mysqli_s;
	str_intern report_mode_s;
	str_intern query_s;
	str_intern prepare_s;
	str_intern set_charset_s;

	str_intern fetch_array_s;
	str_intern fetch_object_s;
	str_intern fetch_assoc_s;
	str_intern fetch_all_s;
	str_intern real_escape_string_s;
	str_intern insert_id_s;

	str_intern begin_transaction_s;
	str_intern begin_commit_s;

	str_intern bind_param_s;
	str_intern stmt_execute_s;

	str_intern rollback_commit_s;



	void init() override;
	void init_req() override;

};

class MsFnTable {
public:
	bool            configured_;

	fn_call         commit_fn;
	fn_call         fetch_array_fn;
	fn_call			set_charset_fn;
	fn_call			query_fn;
	fn_call         report_fn;
	fn_call         begin_transaction_fn;
	fn_call         fetch_all_fn;
	fn_call         fetch_assoc_fn;
	fn_call         real_escape_string_fn;
	fn_call         insert_id_fn;
	fn_call         bind_param_fn;
	fn_call         stmt_execute_fn;


	MsFnTable() : configured_(false) {}

	void init()
	{
		if (configured_) {
			return;
		}
		initFn(Msfi);
	}

	void initFn(MSFInit& ms)
	{
		begin_transaction_fn.set_fci(ms.begin_transaction_s);
		commit_fn.set_fci(ms.commit_s);
		fetch_all_fn.set_fci(ms.fetch_all_s);

		fetch_array_fn.set_fci(ms.fetch_array_s);
		fetch_assoc_fn.set_fci(ms.fetch_assoc_s);
		insert_id_fn.set_fci(ms.insert_id_s);

		bind_param_fn.set_fci(ms.bind_param_s);
		stmt_execute_fn.set_fci(ms.stmt_execute_s);

		query_fn.set_fci(ms.query_s);
		report_fn.set_fci(ms.report_mode_s);

		real_escape_string_fn.set_fci(ms.real_escape_string_s);
		set_charset_fn.set_fci(ms.set_charset_s);

	}
};

base_obj_mgr<Mysqlfn> Mysqlfn::omg;
MFSInit Msfi;

void
MSFInit::init()
{
	mysqli_s = "mysqli";
	report_mode_s = "mysqli_report";
	prepare_s = "mysqli_prepare";
	query_s = "mysqli_query";

	set_charset_s = "mysqli_set_charset";

	
	commit_s = "mysqli_commit";
	begin_transaction_s = "mysqli_begin_transaction";
	fetch_all_s = "mysqli_fetch_all";

	fetch_array_s = "mysqli_fetch_array";
	fetch_assoc_s = "mysqli_fetch_assoc";
	fetch_object_s = "mysqli_fetch_object";

	bind_param_s = "mysqli_stmt_bind_param";
	stmt_execute_s = "mysqli_stmt_execute";

	insert_id_s = "insert_id";
	real_escape_string_s = "mysqli_real_escape_string";
	rollback_commit_s = "rollback_commit";
}


Mysqlfn::Mysqlfn()
{
	wrap_ = nullptr;
}

Mysqlfn::~Mysqlfn()
{
	if (wrap_)
	{
		delete wrap_;
	}
}

thread_local MsFnTable MSfn;




bool 
mysqli_begin_transaction(obj_ptr msi, int flags, str_ptr name)
{
	fn_params  fn(MSfn.set_charset_fn);
	zval* pz = fn.argsptr();
	val_ptr::object_bind(pz, msi);
	pz++; ZVAL_LONG(pz, flags);
	pz++; val_ptr::string_bind(pz, name);
	return fn.zbool();
}

bool mysqli_stmt_bind_param(obj_ptr stmt, str_ptr types, htab_ptr params)
{
	fn_params fn(MSfn.bind_param_fn);
	zval* pz = fn.argsptr();
	val_ptr::object_bind(pz, stmt);
	pz++; val_ptr::string_bind(pz, types);
	for_key_value wkv;
	for(wkv.start(params); wkv.ok(); wkv.next())
	{
		pz++;  ZVAL_COPY_VALUE(pz, wkv.value());
	}
	return fn.zbool();
}

bool 
mysqli_stmt_execute(obj_ptr stmt, htab_ptr params = htab_ptr())
{
	fn_params(MSfn.stmt_execute_fn);
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
	fn_params  fn(MSfn.commit_fn);
	zval* pz = fn.argsptr();
	val_ptr::object_bind(pz, msi);
	pz++; ZVAL_LONG(pz, flags);
	pz++; val_ptr::string_bind(pz, name);
	return fn.zbool();
}


bool 
mysqli_set_charset(obj_ptr msi, str_ptr cset)
{
	fn_params  fn(MSfn.set_charset_fn);
	zval* pz = fn.argsptr();
	val_ptr::object_bind(pz, msi);
	pz++; val_ptr::string_bind(pz, cset);
	return fn.zbool();
}


obj_rc
mysqli_query(obj_ptr msi, int rmode)
{
	fn_params  fn(MSfn.query_fn);
	zval* pz = fn.argsptr();
	val_ptr::object_bind(pz, msi);
	pz++; ZVAL_LONG(pz, rmode);
	return fn.obj();
}


bool 
mysqli_report(int flags)
{
	fn_params  fn(MSfn.report_fn);
	zval* pz = fn.argsptr();
	ZVAL_LONG(pz, flags);
	return fn.zbool();
}

htab_rc 
mysqli_fetch_array(obj_ptr robj, int mode)
{
	fn_params  fn(MSfn.fetch_array_fn);
	zval* pz = fn.argsptr();
	val_ptr::object_bind(pz, robj);
	pz++;
	ZVAL_LONG(pz, mode);
	return fn.array();
}

htab_rc 
mysqli_fetch_assoc(obj_ptr robj)
{
	fn_params  fn(MSfn.fetch_array_fn);
	val_ptr::object_bind(fn.argsptr(), robj);
	return fn.array();
}

obj_rc 
mysqli_fetch_object(obj_ptr robj, str_ptr cname, htab_ptr args)
{
	fn_params  fn(MSfn.fetch_array_fn);
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
	fn_params  fn(MSfn.insert_id_fn);
	val_ptr::object_bind(fn.argsptr(), robj);
	return fn.mixed();
}

str_rc 
mysqli_real_escape_string(obj_ptr msi, str_ptr str)
{
	fn_params  fn(MSfn.real_escape_string_fn);
	zval* pz = fn.argsptr();
	val_ptr::object_bind(pz, msi);
	pz++; val_ptr::string_bind(pz, str);
	return fn.str();
}

void 
register_class(zend_class_entry* idriver_ce)
{
	
}

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
Mysqlfn::getResults(obj_ptr robj)
{
	val_return result;

	if (!robj.ok())
	{
		result.error() << "getResults not a mysqli::result";
		return result;
	}

	htab_rc test;
	switch(ifetch_)
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


str_rc 
attribute(str_ptr name, str_ptr value)
{
	
}

val_rc 
rowFetch(obj_ptr stmt, zend_long fmode = IDriver::FETCH_ASSOC)
{
	
}

bool 
begin()
{
	if (inTransaction_)
	{
		return false; // throw error
	}
	bool ok = mysqli_begin_transation(handle_ptr_.zobject());

	inTransaction_ = ok;

	return ok;
}

void 
Mysqlfn::bind(obj_ptr stmt, htab_ptr params)
{
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
			
		else

	}
}

void 
close()
{
	
}

error_return 
closeStmt(obj_ptr stmt)
{
	
}

bool 
commit()
{
	
}



error_return 
connect()
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

	cp.push_back(host,user,pwd,dbname);

	obj_rc hconnect = ReflectCache::staticInstanceArgs(Msfi.mysqli_s, cparams);

	error_return result;

	if (!hconnect.ok())
	{
		result.error() << "Failed to connect: " << cstr;
		return result;
	}


	mysqli_report(MYSQLI_REPORT_ERROR|MYSQLI_REPORT_STRICT);

	val_rc::try_decref(handle_ptr_);
	handle_ptr_.bind_object(hconnect);

	// make a private wrapper
	if (wrap_)
	{
		delete wrap_;
	}

	mysqli_set_charset(hconnect,charset );
	

	if (collation.ok())
	{
		str_buf buf;
		buf << "SET collation_connection=" << collation;

		mysqli_query(hconnect, buf.zstr());
	}
	return result;	
}

str_rc 
escape(str_ptr value)
{
	
}

val_return 
execute(obj_ptr stmt, bool close, bool fetch)
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

	result = this->getResults($robj);
}

str_rc 
getSqlType()
{
	
}

htab_return 
getTableNames()
{
	
}

bool 
inTransaction()
{
	return inTransaction_;
}

val_return 
lastInsertId(str_ptr name)
{
	
}

val_return 
lastSeqValue(str_ptr name)
{
	
}

obj_return 
prepare(str_ptr query, htab_ptr options=htab_ptr())
{
	
}

val_return 
querySingle(str_ptr query)
{
	
}

bool 
Mysqlfn::rollback()
{

}

Mysqlfn::Mysqlfn()
{
	MSfn.init();
}


}; // namespace wcd



#endif