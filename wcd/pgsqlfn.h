#ifndef PGSQLFN_WDC_H
#define PGSQLFN_WDC_H

#ifndef WCD_IDRIVER_H
#include "idriver.h"
#endif

namespace wcd {

using namespace zpp;

class PgInit : public state_init {
public:
	str_intern pg_free_result_fn;
	str_intern pg_fetch_array_fn;
	str_intern pg_fetch_object_fn;
	str_intern pg_fetch_assoc_fn;

	str_intern pg_affected_rows_fn;
	str_intern pg_fetch_all_fn;
	str_intern pg_fetch_all_columns_fn;
	str_intern pg_connect_fn;
	
	str_intern pg_close_fn;
	str_intern pg_query_fn;
	str_intern pg_escape_string_fn;
	str_intern pg_prepare_fn;
	
	str_intern pg_execute_fn;
	str_intern pg_last_error_fn;

	str_intern table_names_q;

	str_intern squote_char;
	str_intern esc_squote;

	str_intern  dbname_s;
	str_intern  host_s;
	str_intern  port_s;
	str_intern  user_s;
	str_intern  pwd_s;
	str_intern  blank_s;

	str_intern  pgsql;
	str_intern  pgsql_result_class;

	void init();
	void init_req();
	
};

extern PgInit   PGFN;


class PgQuery : public base_d {
protected:
	obj_rc  pghandle_; 
	str_rc  id_;
	str_rc  sql_;
	htab_rc params_;
	obj_rc  rhandle_;

public:

	static base_obj_mgr<PgQuery> omg;

	void construct(obj_ptr connect, str_ptr query);

	void destruct();

	error_return setParams(htab_ptr params);

	void close();

	val_return execute(bool asResult = true);

};


class Pgsqlfn : public IDriver {
protected:
	unsigned long idseq_;
	bool          inTransaction_;
public:

	enum {
		PGSQL_ASSOC       =    1<<0,
		PGSQL_NUM         =   1<<1,
		PGSQL_BOTH        =    (PGSQL_ASSOC|PGSQL_NUM)
	};

	static base_obj_mgr<Pgsqlfn> omg;

	static void register_class(zend_class_entry* idriver_ce);

	static val_rc rowFetch(obj_ptr pgresult, zend_long fmode = IDriver::FETCH_ASSOC);

	static htab_rc allRows(obj_ptr pgresult, zend_long fmode = IDriver::FETCH_ASSOC);

	static str_rc attribute(str_ptr name, str_ptr value);

	str_rc param(unsigned pno) override;

	error_return connect() override;

	unsigned long nextId();

	bool inTransaction() override;

	void close() override;

	obj_return prepare(str_ptr query, htab_ptr options=htab_ptr()) override;

	error_return bind(obj_ptr stmt, htab_ptr params) override;

	val_return execute(obj_ptr stmt, bool close = true, bool fetch = false) override;

	val_return querySingle(str_ptr query) override;

	str_rc getSqlType() override;

	//val_rc fetchRow(val_ptr stmt, int mode) override;

	str_rc escape(str_ptr value) override;

	val_return lastInsertId(str_ptr name) override;

	val_return lastSeqValue(str_ptr name) override;

	htab_return getTableNames() override;

	bool begin(htab_ptr args) override;
	bool commit(htab_ptr args) override;
	bool rollback(htab_ptr args) override;

	error_return closeStmt(obj_ptr stmt) override;

	Pgsqlfn();

};


obj_rc pg_connect(str_ptr s, int flags = 0);

val_rc pg_fetch_all(obj_ptr robj,int fmode = Pgsqlfn::PGSQL_ASSOC);
val_rc pg_fetch_array(obj_ptr robj, val_ptr row, int fmode =  Pgsqlfn::PGSQL_BOTH);
val_rc pg_fetch_assoc(obj_ptr robj);
val_rc pg_fetch_object(obj_ptr robj, val_ptr row = val_ptr::nullval(), 
			str_ptr cname = str_ptr(), htab_ptr args = htab_ptr::empty_array());

htab_rc pg_fetch_all_columns(obj_ptr robj, int colnum = 0);

val_rc pg_execute(obj_ptr connect, str_ptr sname, htab_ptr params);
void   pg_close(obj_ptr connect);
val_rc pg_query(obj_ptr connect, str_ptr query);
obj_rc pg_last_error(obj_ptr connect);
obj_rc pg_prepare(obj_ptr h, str_ptr id, str_ptr sql);
void   pg_free_result(obj_ptr h);
str_rc pg_escape_string(obj_ptr conn, str_ptr s);



}//namespace wcd


#endif