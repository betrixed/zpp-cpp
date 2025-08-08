#ifndef WCD_DELETE_H
#define WCD_DELETE_H


#ifndef WCD_OPERATION_H
#include "operation.h"
#endif


namespace wcd {

	using namespace zpp;
	using namespace wcc;


class Delete : public Operation {
public:

	static base_obj_mgr<Delete> omg;

	virtual obj_rc getSqlParams();

};

}; //namespace wcd

#endif

