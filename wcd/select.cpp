#ifndef WCD_SELECT_CPP
#define WCD_SELECT_CPP

#ifndef WCD_SELECT_H
#include "select.h"
#endif

#ifndef SQL_PART_H
#include "sql_ipart.h"
#endif

namespace wcd {

using namespace wcc;
using namespace zpp;

base_obj_mgr<Select> Select::omg;


zobj_mgr 
Select::getSqlParams()
{
	Bindings& bind = this->bindings();
	if (autoAlias_) {
		bind.aliasSelect();
	}
	zobj_mgr isql_mgr = bind.isql();
	ISql* isq = zobj_toc<ISql>(isql_mgr);

	zobj_mgr plist_mgr = isq->select(bind);
	bind.wipe();
	return plist_mgr;
}

void 
Select::construct(zobj_user db, bool autoAlias)
{
	Operation::construct(db);
	autoAlias_ = autoAlias;
	zobj_user self(vobj());

	icols_ = IColumns::omg.new_zobj();
	icol().construct(self);

}

void 
Select::destruct()
{
	icols_.init();
	Operation::destruct();
}

void 
Select::add(htab_read cols)
{
	icol().add(cols);
}

zobj_mgr 
Select::addJoin(zobj_user ltable, zobj_user rtable, int jtype)
{
	zobj_mgr ji_mgr = JoinInfo::omg.new_zobj();
	JoinInfo*  ji = zobj_toc<JoinInfo>(ji_mgr);

	ji->construct(ltable, rtable, jtype);

	JoinTables& jt = this->joiner();
	
	jt.addJoin(ji_mgr);
	return ji_mgr;
}

zobj_mgr 
Select::addTable(zstr_user table, zstr_user alias, htab_read cols)
{
	zobj_mgr tc_mgr = TColumns::omg.new_zobj();
	TColumns* tc = zobj_toc<TColumns>(tc_mgr);

	zval_mgr cols_mgr(cols);
	tc->construct(table, alias, cols_mgr);

	JoinTables& jt = this->joiner();
	jt.addTable(tc);

	return tc_mgr;
}

zval_mgr 
Select::getRenamed()
{
	zval_mgr results = this->getRows();

	zval_mgr rename = bindings().get(ISql::SQL_RENAME);

	if (results.isArray() && rename.isArray())
	{
		htab_mgr objset;
		htab_write hw(objset);

		htab_read rows(results);
		htab_read rtab(rename);
		htab_walk wk;

		for(wk.start(rows); wk.ok(); wk.next())
		{
			zobj_mgr split = JoinTables::rowSplit(rows, rtab);
			hw.push_back(split);
		}
		results = objset;
	}
	return results;
}

zobj_user
Select::iCols()
{
	return icols_;
}

void 
Select::setAlias(zstr_user alias)
{
	icol().setAlias(alias);
}


}; // namespace

using namespace wcd;

ZEND_METHOD(Wcd_Sql_Select, __construct)
{
	zarg_exec args(execute_data);

	zobj_user db;
	bool      auto_alias = false;

	args.obj_ofclass(db, args.need(1), IDriver::omg.class_entry_);

	args.zbool(auto_alias, args.option(2));

	if (!args.throw_errors())
	{
		Select* sobj = zval_toc<Select>(ZEND_THIS);

		sobj->construct(db, auto_alias);
	}

}

ZEND_METHOD(Wcd_Sql_Select, __destruct)
{
	ZEND_PARSE_PARAMETERS_NONE();
	Select* sobj = zval_toc<Select>(ZEND_THIS);
	sobj->destruct();

}

ZEND_METHOD(Wcd_Sql_Select, add)
{
	zarg_exec args(execute_data);

	htab_read cols;

	args.zarray(cols, args.need(1));

	if (!args.throw_errors())
	{
		Select* sobj = zval_toc<Select>(ZEND_THIS);

		sobj->add(cols);
	}
}

ZEND_METHOD(Wcd_Sql_Select, addJoin)
{
	zarg_exec args(execute_data);

	zobj_user ltable;
	zobj_user rtable;
	zend_long jtype = JoinInfo::J_INNER;
	zobj_mgr result;

	args.obj_ofclass(ltable, args.need(1), zclass_sql_icolumns);
	args.obj_ofclass_null(rtable, args.option(2), zclass_sql_icolumns);
	args.zlong(jtype, args.option(3));
	

	if (!args.throw_errors())
	{
		Select* sobj = zval_toc<Select>(ZEND_THIS);

		result = sobj->addJoin(ltable,rtable,jtype);
	}	

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Select, addTable)
{
	zarg_exec args(execute_data);

	zstr_user tname;
	zstr_user talias;
	htab_read cols;

	zobj_mgr result;

	args.zstring(tname, args.need(1));
	args.zstring_null(talias, args.option(2));
	args.zarray_null(cols, args.option(3));
	

	if (!args.throw_errors())
	{
		Select* sobj = zval_toc<Select>(ZEND_THIS);

		result = sobj->addTable(tname,talias,cols);
	}	

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Select, getRenamed)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Select* sobj = zval_toc<Select>(ZEND_THIS);

	htab_mgr result = sobj->getRenamed();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Select, getSqlParams)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Select* sobj = zval_toc<Select>(ZEND_THIS);

	zobj_mgr result = sobj->getSqlParams();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Select, icols)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Select* sobj = zval_toc<Select>(ZEND_THIS);

	zobj_user result = sobj->iCols();

	result.return_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Select, setAlias)
{
	zarg_exec args(execute_data);

	zstr_user alias;

	args.zstring(alias, args.need(1));

	if (!args.throw_errors())
	{
		Select* sobj = zval_toc<Select>(ZEND_THIS);

		sobj->setAlias(alias);
	}	

}

PHP_MINIT_FUNCTION(Wcd_Select_reg)
{
	Select::omg.classEntry(register_class_Wcd_Sql_Select(Operation::omg.class_entry_));
	return SUCCESS;
}

#endif

