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

	zobj_mgr icols_;

	IColumns& icol() {
		return *zobj_toc<IColumns>(icols_);
	}
public:

	static base_obj_mgr<Select> omg;

	virtual zobj_mgr getSqlParams();

	void construct(zobj_user db, bool autoAlias = false);

	void destruct();

	void add(htab_read cols);

	zobj_mgr addJoin(zobj_user ltable, zobj_user rtable, int jtype);

	zobj_mgr addTable(zstr_user table, zstr_user alias, htab_read cols);

	zval_mgr getRenamed();

	zobj_user iCols();

	void setAlias(zstr_user alias);

};

}; //namespace wcd

#endif