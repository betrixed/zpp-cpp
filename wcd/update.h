#ifndef WCD_UPDATE_H
#define WCD_UPDATE_H


#ifndef WCD_OPERATION_H
#include "operation.h"
#endif


namespace wcd {

	using namespace zpp;
	using namespace wcc;


class Update : public Operation {
public:

	static base_obj_mgr<Update> omg;

	virtual obj_rc getSqlParams();

	void set(str_ptr column, val_ptr value);
};

}; //namespace wcd

#endif