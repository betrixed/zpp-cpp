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
	htab_rc values_;
	val_rc stmt_;
	str_rc sql_;
	obj_rc db_;

protected:

	val_rc send(bool rval);
	
public:

	static base_obj_mgr<Simple> omg;
	
	virtual void debug_info(htab_wr di);

	void construct(obj_ptr db, int fetch = IDriver::FETCH_ASSOC);
	void destruct();

	htab_rc arrayMap(str_ptr keycol, str_ptr valcol, str_ptr table);

	htab_rc arraySet(str_ptr sql, htab_rd params = htab_rd());

	str_rc bind(val_ptr value);

	val_rc exec(str_ptr sql, htab_rd params);

	val_rc firstrow(str_ptr sql, htab_rd params);

	val_rc getRows();

	str_rc getSchemaName();

	val_rc insert(htab_rd values);

	bool prepare(str_ptr sql);

	str_rc quoteName(str_ptr name);

	void returnsValues(bool rval);

	val_rc run();

	void setValues(htab_rd values);

	val_rc update(htab_rd values);
};

}; //namespace wcd

//simple.h
#endif
