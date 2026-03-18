#ifndef MYSQLFN_WCD_H
#define MYSQLFN_WCD_H


#ifndef WCD_IDRIVER_H
#include "idriver.h"
#endif

namespace wcd {

using namespace zpp;

class MsiWrap;

class Mysqlfn : public IDriver {
protected:
	bool          inTransaction_;
	MsiWrap*      wrap_;

public:

	static base_obj_mgr<Mysqlfn> omg;

	static void register_class(zend_class_entry* idriver_ce);

	static htab_rc allRows(obj_ptr result, zend_long fmode = IDriver::FETCH_ASSOC);
	
	static htab_rc resultObjects(obj_ptr result);

	static htab_rc resultNum(obj_ptr result);

	static val_rc  getResults(obj_ptr result);
	

	static str_rc attribute(str_ptr name, str_ptr value);
	
	val_rc rowFetch(obj_ptr stmt, zend_long fmode = IDriver::FETCH_ASSOC);

	bool begin() override;

	void bind(obj_ptr stmt, htab_ptr params) override;

	void close() override;

	error_return closeStmt(obj_ptr stmt) override;

	bool commit() override;

	error_return connect() override;

	str_rc escape(str_ptr value) override;

	val_return execute(obj_ptr stmt, bool close = true, bool fetch = false) override;

	str_rc getSqlType() override;

	htab_return getTableNames() override;

	bool inTransaction() override;

	val_return lastInsertId(str_ptr name) override;

	val_return lastSeqValue(str_ptr name) override;

	obj_return prepare(str_ptr query, htab_ptr options=htab_ptr()) override;

	val_return querySingle(str_ptr query) override;

	bool rollback() override;

	Mysqlfn();
	virtual ~Mysqlfn();

};


class MsiWrap {
protected:
	obj_rc  		handle_;
	fn_call_args1	set_charset_fn;
	fn_call_args2	query_fn;
public:

	MsiWrap(obj_ptr h);

	void set_charset(str_ptr cset);
	val_rc query(str_ptr query, int result_mode = MSQLI_STORE_RESULT);

};


#endif