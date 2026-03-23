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
		query_fn.set_fci(ms.query_s);
		report_fn.set_fci(ms.report_mode_s);
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
	fetch_object_s = "fetch_object";
	insert_id_s = "insert_id";
	real_escape_string_s = "real_escape_string";
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

val_rc  
getResults(obj_ptr result)
{
	
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
	
}

void 
bind(obj_ptr stmt, htab_ptr params)
{
	
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


	val_rc::try_decref(handle_ptr_);
	handle_ptr_.bind_object(hconnect);

	// make a private wrapper
	if (wrap_)
	{
		delete wrap_;
	}

	wrap_ = new MsiWrap(hconnect);

	hconnect.property(Msfi.report_mode_s, MYSQLI_REPORT_ERROR|MYSQLI_REPORT_STRICT);

	str_buf buf;

	buf << "SET collation_connection=" << collation;

	wrap_->query(buf.zstr());

	return result;	
}

str_rc 
escape(str_ptr value)
{
	
}

val_return 
execute(obj_ptr stmt, bool close = true, bool fetch = false)
{
	
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