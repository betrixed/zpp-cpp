#ifndef WCD_INSERT_CPP
#define WCD_INSERT_CPP

#ifndef WCD_INSERT_H
#include "delete.h"
#endif

#ifndef SQL_PART_H
#include "sql_ipart.h"
#endif

namespace wcd {


using namespace zpp;
using namespace wcc;

base_obj_mgr<Insert> Insert::omg;



obj_rc 
Insert::getSqlParams()
{
	Bindings& bind = this->bindings();
	obj_ptr isql_ptr = bind.isql();

	ISql* isql = zobj_toc<ISql>(isql_ptr);

	val_ptr idata = bind.get(ISql::SQL_INSERT);

	if (idata.empty())
	{
		JoinTables& jt = this->joiner();
		obj_rc  ptable = jt.getPrime();
		if (ptable.ok())
		{
			IColumns* icols = zobj_toc<IColumns>(ptable);
			htab_ptr names = icols->getColNames();

			htab_rc coldata_rc;
			htab_rw coldata(coldata_rc);

			htab_walk wk;
			auto      name_ptr = wk.key();
			val_rc 	  false_val;

			false_val.set_bool(false);

			for(wk.start(names); wk.ok(); wk.next())
			{
				coldata.set(name_ptr, false_val);
			}
			bind.addarray(ISql::SQL_INSERT, coldata_rc);
		}
		else {
			zend_throw_error(zend_ce_error,"Insert: values not set");
			return obj_rc();
		}
	}
	obj_rc plist_rc = isql->insert(bind);

	bind.wipe();

	return plist_rc;
}

}; // namespace wcd

using namespace wcd;
using namespace zpp;


ZEND_METHOD(Wcd_Sql_Insert, getSqlParams)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Insert* cobj = zval_toc<Insert>(ZEND_THIS);

	obj_rc result = cobj->getSqlParams();

	result.move_zv(return_value);
}


#endif