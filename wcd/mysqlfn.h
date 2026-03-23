#ifndef MYSQLFN_WCD_H
#define MYSQLFN_WCD_H


#ifndef WCD_IDRIVER_H
#include "idriver.h"
#endif

namespace wcd {

using namespace zpp;

class MsiWrap;

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
//--------------------------------------------------

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



#endif