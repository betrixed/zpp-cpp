#ifndef WCD_SIMPLE_H
#define WCD_SIMPLE_H


#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif



namespace wcd {

	using namespace zpp;
	using namespace wcc;


class Simple : public base_d {
protected:
	bool autoclose_;
	bool retval_;
	int  fetch_;
	htab_mgr values;
	zval_mgr stmt_;
	zstr_mgr sql_;
	zobj_mgr db_;
	
public:

	void construct(zobj_user db);
	void destruct();

	zval_mgr arrayMap(zstr_user keycol, zstr_user valcol, zstr_user table);

	htab_mgr arraySet(zstr_user sql, htab_read params);

	zstr_mgr bind(zval_user value);

	int exec(zstr_user sql, htab_read params);

	htab_mgr firstrow(zstr_user sql, htab_read params);

	htab_mgr getRows();

	zstr_mgr getSchemaName();

	zval_mgr insert(htab_read values);

	zval_mgr prepare(zstr_user sql);

	zstr_mgr quoteName(zstr_user name);

	void returnsValues(bool rval);

	zval_mgr run();

	void setValues(htab_read values);

	zval_mgr update(htab_read values);
};

}; //namespace wcd

//simple.h
#endif
