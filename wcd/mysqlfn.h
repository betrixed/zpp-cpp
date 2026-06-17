#ifndef MYSQLFN_WCD_H
#define MYSQLFN_WCD_H


#ifndef WCD_IDRIVER_H
#include "idriver.h"
#endif

/*
#ifndef MYSQLI_STUFF
extern "C" {
	#include <ext/mysqli/php_mysqli_structs.h>
}
#endif
*/

namespace wcd {

using namespace zpp;

enum {
	MYSQLI_STORE_RESULT = 0,
	MYSQLI_ASSOC = 1,
	MYSQLI_NUM = 2,
	MYSQLI_BOTH = 3
};

//C++ MSI procedural function prototypes
bool mysqli_begin_transaction(obj_ptr msi, 
		int flags=0, str_ptr name=str_ptr());
bool mysqli_commit(obj_ptr msi, int flags=0, str_ptr name=str_ptr());

bool mysqli_set_charset(obj_ptr msi, str_ptr cset);

obj_rc mysqli_query(obj_ptr msi, int rmode=MYSQLI_STORE_RESULT);

bool mysqli_report(int flags);

htab_rc mysqli_fetch_all(obj_ptr msi, int rmode=MYSQLI_NUM);

htab_rc mysqli_fetch_array(obj_ptr robj, int mode=MYSQLI_NUM);

htab_rc mysqli_fetch_assoc(obj_ptr robj);

obj_rc mysqli_fetch_object(obj_ptr robj, str_ptr cname=str_ptr(),
		htab_ptr args=htab_ptr());

str_rc mysqli_real_escape_string(obj_ptr msi, str_ptr str);

val_rc mysqli_insert_id(obj_ptr msi);

bool mysqli_stmt_bind_param(obj_ptr stmt, str_ptr types, htab_ptr params);

bool mysqli_stmt_execute(obj_ptr stmt, htab_ptr params = htab_ptr());

obj_rc mysqli_stmt_get_result(obj_ptr stmt);

val_rc mysqli_stmt_affected_rows(obj_ptr stmt);

bool mysqli_stmt_close(obj_ptr stmt);


//--------------------------------------------------

class Mysqlfn : public IDriver {
protected:
	bool          inTransaction_;


public:

	static base_obj_mgr<Mysqlfn> omg;

	static void register_class(zend_class_entry* idriver_ce);

	static htab_rc allRows(obj_ptr result, zend_long fmode = IDriver::FETCH_ASSOC);
	
	static htab_rc resultObjects(obj_ptr robj);

	static val_return  getResults(obj_ptr result, int mode);

	static htab_rc resultNum(obj_ptr result);



	static str_rc attribute(str_ptr name, str_ptr value);

	
	
	val_rc fetchRow(obj_ptr stmt, zend_long fmode = IDriver::FETCH_ASSOC);

	bool begin() override;

	error_return bind(obj_ptr stmt, htab_ptr params) override;

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

}//wcc namespace

#endif