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

error_return 
Update::set(str_ptr column, val_ptr value)
{
	error_return result;

	htab_rc args_rc;

	htab_rw args(args_rc);

	args.set(SQSTR.column, column);
	args.set(SQSTR.valuekey, value);

	obj_return bret = getBind();
	if (bret.has_errors())
	{
		result = std::move(bret);
		return result;
	}

	Bindings* bind = zobj_toc<Bindings>(bret.value_);

	bind->addarray(ISql::SQL_UPDATE, args_rc);

	return result;
}

obj_return 
Update::getSqlParams()
{
	obj_return result;

	obj_return bret = getBind();
	if (bret.has_errors())
	{
		result = std::move(bret);
		return result;
	}

	Bindings* bind = zobj_toc<Bindings>(bret.value_);

	obj_ptr isql_ptr = bind->isql();

	ISql* isql = zobj_toc<ISql>(isql_ptr);

	result = isql->update(*bind);
	if (result.has_errors())
	{
		return result;
	}
	bind->wipe();
	return result;
}

}; // namespace wcd

using namespace wcd;
using namespace zpp;

ZEND_METHOD(Wcd_Sql_Update, set)
{
	zarg_rd args(execute_data);

	str_ptr  column;
	val_ptr  value;

	args.zstring(column, args.need(0));
	value = args.need(1);

	if (!args.throw_errors())
	{
		Update* cobj = zval_toc<Update>(ZEND_THIS);
		error_return result = cobj->set(column,value);
		result.throw_errors();
	}
}

ZEND_METHOD(Wcd_Sql_Update, getSqlParams)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Update* cobj = zval_toc<Update>(ZEND_THIS);

	obj_return result = cobj->getSqlParams();
	result.throw_errors();
	result.value_.move_zv(return_value);
}

#endif