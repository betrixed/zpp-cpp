#ifndef WCD_UPDATE_CPP
#define WCD_UPDATE_CPP

#ifndef WCD_UPDATE_H
#include "update.h"
#endif

#ifndef SQL_PART_H
#include "sql_ipart.h"
#endif

namespace wcd {

using namespace zpp;
using namespace wcc;

base_obj_mgr<Update> Update::omg;

void 
Update::set(str_ptr column, val_ptr value)
{
	htab_rc args_rc;

	htab_rw args(args_rc);

	args.set(SQSTR.column, column);
	args.set(SQSTR.valuekey, value);

	Bindings& bind = this->bindings();
	bind.add(ISql::SQL_UPDATE, args_rc);
}

obj_rc 
Update::getSqlParams()
{
	Bindings& bind = this->bindings();

	obj_rc isql_mgr = bind.iSql();

	ISql* isql = zobj_toc<ISql>(isql_mgr);

	obj_rc plist_rc = isql->update(bind);

	bind.wipe();

	return plist_rc();
}

}; // namespace wcd

using namespace wcd;
using namespace zpp;

ZEND_METHOD(Wcd_Sql_Update, set)
{
	zarg_rd args(execute_data);

	str_ptr  column;
	val_ptr  value;

	args.zstring(column, args.need(1));
	value = args.need(2);

	if (!args.throw_errors())
	{
		Update* cobj = zval_toc<Update>(ZEND_THIS);
		cobj->set(column,value);
	}
}

ZEND_METHOD(Wcd_Sql_Update, getSqlParams)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Update* cobj = zval_toc<Update>(ZEND_THIS);

	obj_rc result = cobj->getSqlParams();

	result.move_zv(return_value);
}

#endif