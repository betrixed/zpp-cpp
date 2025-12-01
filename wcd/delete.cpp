#ifndef WCD_DELETE_CPP
#define WCD_DELETE_CPP

#ifndef WCD_DELETE_H
#include "delete.h"
#endif

#ifndef SQL_PART_H
#include "sql_ipart.h"
#endif

namespace wcd {


using namespace zpp;
using namespace wcc;

base_obj_mgr<Delete> Delete::omg;


obj_return 
Delete::getSqlParams()
{
	Bindings& bind = this->bindings();

	obj_rc isql_mgr = bind.isql();

	ISql* isql = zobj_toc<ISql>(isql_mgr);

	obj_rc plist_rc = isql->deleteSql(bind);

	bind.wipe();

	return plist_rc;
}

}; // namespace wcd

using namespace wcd;
using namespace zpp;

ZEND_METHOD(Wcd_Sql_Delete, getSqlParams)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Delete* cobj = zval_toc<Delete>(ZEND_THIS);

	obj_rc result = cobj->getSqlParams();

	result.move_zv(return_value);
}


#endif
