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
	str_intern pg_fetch_all_fn;
	str_intern pg_fetch_all_columns_fn;
	str_intern pg_connect_fn;
	str_intern pg_close_fn;
	str_intern pg_query_fn;
	str_intern pg_escape_string_fn;

	str_intern pg_execute_fn;
	str_intern pg_last_error_fn;

	str_intern table_names_q;

	str_intern squote_char;
	str_intern esc_squote;

	str_intern  dbname_s;
	str_intern  host_s;
	str_intern  port_s;
	str_intern  user_s;
	str_intern  pwd_d;
	str_intern  blank_s;

	str_intern  pgsql;
	str_intern  pgsql_result_class;

	void init();
	void init_req();
	
};

extern PgInit   PGFN;

void pg_free_result(val_handle h);


class PgQuery : public bind_d 
{
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

	void setParams(htab_ptr params);

	void close();

	val_return execute(bool asResult = true);

};


class Pgsqlfn : public IDriver {
protected:
	unsigned long idseq_;
	bool          inTransaction_;
public:
	static base_obj_mgr<Pgsqlfn> omg;

	static val_rc rowFetch(obj_ptr pgresult, int fmode);

	static htab_rc allRows(obj_ptr pgresult, int fmode);

	static str_rc attribute(str_ptr name, str_ptr value);

	str_rc param(unsigned pno) override;

	error_return connect() override;

	unsigned long nextId();

	bool inTransaction() override;

	void close() override;

	val_return prepare(str_ptr query, htab_ptr options=htab_ptr()) override;

	val_return execute(val_ptr stmt, bool close = true, bool fetch = false) override;

	val_return querySingle(str_ptr query) override;

	str_rc getSqlType() override;

	//val_rc fetchRow(val_ptr stmt, int mode) override;

	str_rc escape(str_ptr value) override;

	val_return lastInsertId(str_ptr name) override;

	val_return lastSeqValue(str_ptr name) override;

	htab_return getTableNames() override;

	bool begin() override;
	bool commit() override;
	bool rollback() override;

	void closeStmt(val_ptr stmt) override;

	Pgsqlfn();



};


}//namespace wcd


#endif