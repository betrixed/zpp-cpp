#ifndef WCD_SQL_RAW_H
#define WCD_SQL_RAW_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif


namespace wcd {

	using namespace zpp;
	using namespace wcc;

class Raw : public base_d
{
public:
	static base_obj_mgr<Raw> omg;


	void construct(str_ptr sql, obj_ptr driver);

	val_return execute();

	obj_rc getConnection();

	str_rc getSql();

	void setConnection(obj_ptr driver);
protected:

	str_rc sql_;
	obj_rc db_;
};



}; //namespace wcd

#endif