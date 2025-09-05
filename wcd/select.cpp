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


obj_rc 
Select::getSqlParams()
{
	Bindings& bind = this->bindings();
	if (autoAlias_) {
		bind.aliasSelect();
	}
	obj_rc isql_mgr = bind.isql();
	ISql* isq = zobj_toc<ISql>(isql_mgr);

	obj_rc plist_mgr = isq->select(bind);
	bind.wipe();

	//showobj("getSQLParams", vobj());
	ParamList* plist = zobj_toc<ParamList>(plist_mgr);

	str_rc sql = plist->getSql();
	//showstr("sql: ", sql);
	return plist_mgr;
}

void 
Select::construct(obj_ptr db, bool autoAlias)
{
	Operation::construct(db);

	autoAlias_ = autoAlias;
	//showobj("Select::construct", vobj());
}	

 htab_rc // static
 Select::getAlive()
 {
 	htab_rc values;

 	htab_rw hw(values);

 	base_dlink<Select>& dref = omg.obj_list_;

 	auto n = dref.next_;
 	while(n)
 	{
 		hw.push_back(n->obj_);
 		n = n->next_;
 	}
 	return values;
 }

void
Select::aggregate(str_ptr aggfn, str_ptr alias, htab_ptr aggargs)
{
	htab_rc data;

	htab_rw hw(data);

	hw.set(SQSTR.function, aggfn);
	hw.set(SQSTR.alias, alias);
	
	if (!aggargs.size())
	{
		hw.set(SQSTR.columns, SQSTR.asterisk);
	}
	else {
		hw.set(SQSTR.columns, aggargs);
	}

	Bindings& bind = this->bindings();
	bind.set(ISql::SQL_AGGREGATE, data);
}

void 
Select::destruct()
{
	//showobj("~Select", vobj());
	this->wipe();
}

void
Select::wipe()
{
	//showobj("Select::wipe", vobj());
	icols_.init();
	Operation::wipe();
}

void 
Select::add(htab_ptr cols)
{
	icol().add(cols);
}

obj_rc 
Select::addJoin(obj_ptr ltable, obj_ptr rtable, int jtype)
{
	obj_rc ji_mgr = JoinInfo::omg.new_zobj();
	JoinInfo*  ji = zobj_toc<JoinInfo>(ji_mgr);

	ji->construct(ltable, rtable, jtype);

	JoinTables& jt = this->joiner();
	
	jt.addJoin(ji_mgr);
	return ji_mgr;
}

obj_rc 
Select::addTable(str_ptr table, str_ptr alias, htab_ptr cols)
{
	obj_rc tc_mgr = TColumns::omg.new_zobj();
	TColumns* tc = zobj_toc<TColumns>(tc_mgr);

	val_rc cols_mgr(cols);
	tc->construct(table, alias, cols_mgr);

	JoinTables& jt = this->joiner();
	jt.addTable(tc);

	return tc_mgr;
}

val_rc 
Select::getRenamed()
{
	val_rc results = this->getRows();

	val_rc rename = bindings().get(ISql::SQL_RENAME);

	if (results.isArray() && rename.isArray())
	{
		htab_rc objset;
		htab_rw hw(objset);

		htab_ptr rows(results);
		htab_ptr rtab(rename);
		htab_walk wk;

		for(wk.start(rows); wk.ok(); wk.next())
		{
			obj_rc split = JoinTables::rowSplit(rows, rtab);
			hw.push_back(split);
		}
		results = objset;
	}
	return results;
}

obj_ptr
Select::iCols()
{
	if (!icols_.ok())
	{
		icols_ = IColumns::omg.new_zobj();
		IColumns* ip = zobj_toc<IColumns>(icols_);
		obj_ptr self(vobj());
		ip->construct(self);
	}
	return icols_;
}

void 
Select::setAlias(str_ptr alias)
{
	icol().setAlias(alias);
}


}; // namespace

using namespace wcd;

ZEND_METHOD(Wcd_Sql_Select, __construct)
{
	zarg_rd args(execute_data);
	obj_ptr   db;
	bool      auto_alias = false;

	args.obj_ofclass(db, args.need(1), IDriver::omg.class_entry_);

	if (!args.zbool(auto_alias, args.option(2)))
	{
		/*#if ZEND_DEBUG
		zend_printf("Arg 2 using default\n");
		#endif
		*/
	};

	if (!args.throw_errors())
	{
		Select* sobj = zval_toc<Select>(ZEND_THIS);

		sobj->construct(db, auto_alias);
		//showobj("DB object", db);
	}

	/*zval*     dbobj;
	bool      auto_alias = false;
	zend_class_entry* ce = IDriver::omg.class_entry_;

	ZEND_PARSE_PARAMETERS_START(1,2)
	Z_PARAM_OBJECT_OF_CLASS(dbobj, ce)
	Z_PARAM_OPTIONAL
	Z_PARAM_BOOL(auto_alias)
	ZEND_PARSE_PARAMETERS_END();

	Select* sobj = zval_toc<Select>(ZEND_THIS);
	sobj->construct(dbobj, auto_alias);
	*/
}

ZEND_METHOD(Wcd_Sql_Select, __destruct)
{
	ZEND_PARSE_PARAMETERS_NONE();
	Select* sobj = zval_toc<Select>(ZEND_THIS);
	sobj->destruct();

}

ZEND_METHOD(Wcd_Sql_Select, getAlive)
{
	ZEND_PARSE_PARAMETERS_NONE();

	htab_rc data = Select::getAlive();
	
	data.move_zv(return_value);

}

ZEND_METHOD(Wcd_Sql_Select, add)
{
	zarg_rd args(execute_data);

	htab_ptr cols;

	args.zarray(cols, args.need(1));

	if (!args.throw_errors())
	{
		Select* sobj = zval_toc<Select>(ZEND_THIS);

		sobj->add(cols);
	}
}


//public function aggregate(string $fn, string $as, ?array $args) : void {}
ZEND_METHOD(Wcd_Sql_Select, aggregate)
{
	zarg_rd args(execute_data);

	str_ptr aggfn;
	str_ptr alias;

	htab_ptr aggcols;

	args.zstring(aggfn, args.need(1));
	args.zstring(alias, args.need(2));

	args.zarray_null(aggcols, args.option(3));

	if (!args.throw_errors())
	{
		Select* sobj = zval_toc<Select>(ZEND_THIS);

		sobj->aggregate(aggfn, alias, aggcols);
	}
}

ZEND_METHOD(Wcd_Sql_Select, addJoin)
{
	zarg_rd args(execute_data);

	obj_ptr ltable;
	obj_ptr rtable;
	zend_long jtype = JoinInfo::J_INNER;
	obj_rc result;

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
	zarg_rd args(execute_data);

	str_ptr tname;
	str_ptr talias;
	htab_ptr cols;

	obj_rc result;

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

	htab_rc result = sobj->getRenamed();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Select, getSqlParams)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Select* sobj = zval_toc<Select>(ZEND_THIS);

	obj_rc result = sobj->getSqlParams();


	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Select, icols)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Select* sobj = zval_toc<Select>(ZEND_THIS);

	obj_ptr result = sobj->iCols();

	result.return_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Select, wipe)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Select* sobj = zval_toc<Select>(ZEND_THIS);

	sobj->wipe();
}

ZEND_METHOD(Wcd_Sql_Select, setAlias)
{
	zarg_rd args(execute_data);

	str_ptr alias;

	args.zstring(alias, args.need(1));

	if (!args.throw_errors())
	{
		Select* sobj = zval_toc<Select>(ZEND_THIS);

		sobj->setAlias(alias);
	}	

}


#endif

