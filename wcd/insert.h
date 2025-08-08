#ifndef WCD_INSERT_H
#define WCD_INSERT_H


#ifndef WCD_OPERATION_H
#include "operation.h"
#endif


namespace wcd {

	using namespace zpp;
	using namespace wcc;


class Insert : public Operation {
public:

	static base_obj_mgr<Insert> omg;

	virtual obj_rc getSqlParams();

};

}; //namespace wcd

#endif

