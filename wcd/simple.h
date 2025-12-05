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
	val_rc stmt_;
	htab_rc values_;
	//str_rc sql_;
	obj_rc db_;
	bool autoclose_;
	bool retval_;
	int  fetch_;

protected:

	val_return send(bool rval);
	
public:

	static base_obj_mgr<Simple> omg;
	
	virtual void debug_info(htab_rw di);

	void construct(obj_ptr db, int fetch = IDriver::FETCH_ASSOC);
	void destruct();

	htab_return arrayMap(str_ptr keycol, str_ptr valcol, str_ptr table);

	htab_return arraySet(str_ptr sql, htab_ptr params = htab_ptr());

	str_rc bind(val_ptr value);

	val_return exec(str_ptr sql, htab_ptr params);

	val_return firstrow(str_ptr sql, htab_ptr params);

	val_return getRows();

	str_rc getSchemaName();

	val_return insert(htab_ptr values);

	bool_return prepare(str_ptr sql);

	str_rc quoteName(str_ptr name);

	void returnsValues(bool rval);

	val_return run();

	void setValues(htab_ptr values);

	val_return  update(htab_ptr values);
};

}; //namespace wcd

//simple.h
#endif
