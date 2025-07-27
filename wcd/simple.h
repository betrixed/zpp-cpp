#ifndef WCD_SIMPLE_H
#define WCD_SIMPLE_H


#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

#ifndef WCD_IDRIVER_H
#include "idriver.h"
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

protected:

	zval_mgr send(bool rval);
	
public:

	static base_obj_mgr<Simple> omg;
	
	virtual void debug_info(htab_write di);

	void construct(zobj_user db, int fetch = IDriver::FETCH_ASSOC);
	void destruct();

	htab_mgr arrayMap(zstr_user keycol, zstr_user valcol, zstr_user table);

	htab_mgr arraySet(zstr_user sql, htab_read params = htab_read());

	zstr_mgr bind(zval_user value);

	int exec(zstr_user sql, htab_read params);

	zstr_mgr firstrow(zstr_user sql, htab_read params);

	zstr_mgr getRows();

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
