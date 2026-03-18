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

base_obj_mgr<Mysqlfn> Mysqlfn::omg;

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
};

void
MSFInit::init()
{
	mysqli_s = "mysqli";
	report_mode_s = "report_mode";
	query_s = "query";
	prepare_s = "prepare";
	set_charset_s = "set_charset";

	fetch_array_s = "fetch_array";
	fetch_object_s = "fetch_object";
	fetch_assoc_s = "fetch_assoc";
	fetch_all_s = "fetch_all";
	real_escape_string_s = "real_escape_string";
	
	insert_id_s = "insert_id";
	begin_transaction_s = "begin_transaction";
	begin_commit_s = "begin_commit";
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

MSFInit  Msfi;

class MsiWrap {
protected:
	obj_rc  		handle_;
	fn_call	set_charset_fn;
	fn_call	query_fn;
public:

	MsiWrap(obj_ptr h);

	void set_charset(str_ptr cset);
	val_rc query(str_ptr query, int result_mode = MSQLI_STORE_RESULT);

};

MsiWrap::MsiWrap(obj_ptr h) 
{
	set_charset_fn.set_fci(h,Msfi.set_charset_s);
	query_fn.set_fci(h,Msfi.query_s);
}

void 
MsiWrap::set_charset(str_ptr cset)
{
	auto& fn = set_charset_fn;
	zval* zp = fn.argsptr();
	val_ptr::string_bind(zp, cset);
	fn.call_fn();
}

val_rc 
MsiWrap::query(str_ptr query, int result_mode = MSQLI_STORE_RESULT)
{
	auto& fn = query_fn;
	zval* zp = fn.argsptr();
	val_ptr::string_bind(zp, query);
	zp++;
	ZVAL_LONG(zp,result_mode);
	val_rc result = fn.call_fn();
	return result;
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

}


}; // namespace wcd



#endif