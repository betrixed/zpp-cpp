#ifndef WCD_SELECT_H
#define WCD_SELECT_H

#ifndef WCD_OPERATION_H
#include "operation.h"
#endif




namespace wcd {

	using namespace zpp;
	using namespace wcc;


class Select : public Operation {
protected:
	bool autoAlias_;

	obj_rc icols_;

	IColumns& icol() {
		return *zobj_toc<IColumns>(icols_);
	}
public:

	static base_obj_mgr<Select> omg;

	virtual obj_rc getSqlParams();

	void construct(obj_ptr db, bool autoAlias = false);

	void destruct();

	void aggregate(str_ptr fn, str_ptr alias, htab_ptr args);

	void add(htab_ptr cols);

	obj_rc addJoin(obj_ptr ltable, obj_ptr rtable, int jtype);

	obj_rc addTable(str_ptr table, str_ptr alias, htab_ptr cols);

	val_rc getRenamed();

	obj_ptr iCols();

	void setAlias(str_ptr alias);

};

}; //namespace wcd

#endif