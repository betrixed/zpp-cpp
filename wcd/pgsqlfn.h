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
	str_intern pg_execute_fn;
	str_intern pg_last_error_fn;


	void init();
	void init_req();
	
};

extern PgInit   PGFN;

void pg_free_result(val_handle h);


class PgResource : public base_d 
{
protected:
	obj_     rhandle_;
public:
	
	static base_obj_mgr<PgResource> omg;

	void 	construct(val_ptr reso);
	void 	destruct();
	void 	close();

};

class PgResult : public PgResource
{
protected:
public:
	enum {
		PGSQL_ASSOC           1<<0,
		PGSQL_NUM             1<<1,
		PGSQL_BOTH            (PGSQL_ASSOC|PGSQL_NUM)
	};
	static base_obj_mgr<PgResult> omg;

	val_rc   rowFetch(zend_long fmode);
	htab_rc allRows(zend_long fmode);
	htab_rc allColumns(zend_long colnum = 0);

	obj_rc   objFetch();

};

class PgQuery : public PgResource 
{
protected:
	obj_rc  pghandle_; 
	str_rc  id_;
	str_rc  sql_;
	htab_rc params_;

public:

	static base_obj_mgr<PgQuery> omg;

	void construct(obj_ptr connect, str_ptr query);

	void setParams(htab_ptr params);

	val_return execute(bool asResult = true);


};



class Pgsqlfn : public IDriver {
protected:
	unsigned long idseq_;
	bool          inTransaction_;
public:
	static base_obj_mgr<Pgsqlfn> omg;


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

	val_rc fetchRow(val_ptr stmt, int mode) override;

	str_rc escape(str_ptr value) override;

	val_return lastInsertId(str_ptr name) override;

	val_return lastSeqValue(str_ptr name) override;

	htab_return getTableNames() override;

	str_rc quoteName(str_ptr name) override;

	bool begin() override;
	bool commit() override;
	bool rollback() override;

	bool closeStmt(val_ptr stmt) override;

	Pgsqlfn();



};


}//namespace wcd


#endif