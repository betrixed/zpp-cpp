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


	void construct(zstr_user sql, zobj_user driver);

	zval_mgr execute();

	zobj_mgr getConnection();

	zstr_mgr getSql();

	void setConnection(zobj_user driver);
protected:

	zstr_mgr sql_;
	zobj_mgr db_;
};



}; //namespace wcd

#endif