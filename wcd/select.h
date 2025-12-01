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
		if (!icols_.ok())
		{
			this->iCols();
		}
		return *zobj_toc<IColumns>(icols_);
	}

	base_dlink<Select> dlink_;

public:

	virtual void debug_info(htab_rw di);
	
	static htab_rc getAlive();

	static base_obj_mgr<Select> omg;

	Select() : dlink_(this) 
	{
		dlink_.linkup(omg.obj_list_);
	}

	virtual ~Select() {
		dlink_.unlink();
	}
	virtual obj_return getSqlParams();

	void construct(obj_ptr db, bool autoAlias = false);

	void destruct();

	void aggregate(str_ptr fn, str_ptr alias, htab_ptr args);

	void add(htab_ptr cols);

	obj_rc addJoin(obj_ptr ltable, obj_ptr rtable, int jtype);

	obj_rc addTable(str_ptr table, str_ptr alias, htab_ptr cols);

	val_rc getRenamed();

	obj_ptr iCols();

	void wipe() override;

	void setAlias(str_ptr alias);

};

}; //namespace wcd

#endif