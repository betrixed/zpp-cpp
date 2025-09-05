#ifndef WCD_OPERATION_CPP
#define WCD_OPERATION_CPP

#ifndef WCD_OPERATION_H
#include "operation.h"
#endif

#ifndef WCD_SELECT_H
#include "select.h"
#endif

#ifndef WCD_DELETE_H
#include "delete.h"
#endif

#ifndef WCD_UPDATE_H
#include "update.h"
#endif

#ifndef WCD_INSERT_H
#include "insert.h"
#endif

#ifndef SQL_PART_H
#include "sql_ipart.h"
#endif

namespace wcd {


using namespace zpp;
using namespace wcc;

base_obj_mgr<Operation> Operation::omg;

void 
Operation::debug_info(htab_rw di)
{
	base_d::debug_info(di);
	di.set(SQSTR.db_name, db_name_);
	di.set(SQSTR.driver, driver_);
	di.set(SQSTR.join_tables, joiner_);
}

obj_ptr
Operation::getDb()
{
	if (!driver_.ok())
	{
		//zend_printf("get Driver");
		driver_ = IServer::connect(db_name_);
		//showobj("Driver", driver_);
	}
	return driver_;
}

obj_ptr
Operation::getBind()
{
	if (!bind_.ok())
	{
		IDriver& dv = driver();
		bind_ = dv.newBindings();
	}
	return bind_;
}

void 
Operation::construct(obj_ptr db)
{
	IDriver* dv = zobj_toc<IDriver>(db);

	db_name_ = dv->getName();
}

void 
Operation::destruct()
{
	bind_.init();
	joiner_.init();
	driver_.init();
	db_name_.init();
	
}

obj_rc 
Operation::addPrime(str_ptr table, str_ptr alias, htab_ptr cols)
{
	obj_rc tc = TColumns::omg.new_zobj();
	TColumns* tcobj = zobj_toc<TColumns>(tc);

	val_rc tval(cols);
	tcobj->construct(table, alias, tval);

	//showobj("tcobj", tc);

	JoinTables& jt = joiner();
	jt.setPrime(tc);
	return tc;
}

val_rc 
Operation::firstRow(int fetch)
{
	val_rc result = getRows(fetch);

	htab_ptr rows(result.zarray());
	if (rows.size())
	{
		return rows.get(int(0));
	}
	result.set_null();
	return result;
}

obj_ptr
Operation::getJoiner()
{
	if (!joiner_.ok())
	{
		joiner_ = JoinTables::omg.new_zobj();
		Bindings& bind = bindings();

		val_rc arg(joiner_);
		bind.set(ISql::SQL_FROM, arg);
	}
	return joiner_;
}

val_rc 
Operation::getRows(int fetch)
{
	val_rc result;

	obj_rc simple = prepare( fetch );

	if (simple.ok())
	{
		Simple* s = zobj_toc<Simple>(simple);

		result = s->getRows();
	}
	return result;
}

obj_rc
Operation::getSqlParams()
{
	Bindings& bind = bindings();
	obj_rc plist = bind.getParamList();
	this->wipe();
	return plist;
}

/* this throws away the ParamList object
*/
str_rc 
Operation::getSql()
{
	str_rc sql;

	obj_ptr self(vobj());
	// call via php method name
	obj_rc pobj = self.call(SQSTR.get_sql_params);
	if (pobj.ok())
	{
		ParamList* plist = zobj_toc<ParamList>(pobj);
		sql = plist->getSql();
	}
	return sql;
}

void 
Operation::limit(val_ptr ct, val_ptr start)
{
	Bindings& bind = bindings();
	bind.limit(ct, start);
}

void 
Operation::orderBy(val_ptr column, bool descend)
{
	Bindings& bind = bindings();
	bind.orderBy(column, descend);
}

obj_rc 
Operation::prepare(int fetch)
{
	if (fetch < 0)
	{
		fetch = IDriver::FETCH_ASSOC;
	}

	obj_rc s = Simple::omg.new_zobj();

	Simple* sobj = zobj_toc<Simple>(s);
	sobj->construct(getDb(), fetch);

	//showobj("Simple", s);

	Bindings& bind = bindings();
	val_rc retvals = bind.get(ISql::SQL_RETURN);

	if (retvals.ok())
	{
		sobj->returnsValues(true);
	}

	obj_ptr self(vobj());

	obj_rc pobj_mgr = self.call(SQSTR.get_sql_params);
	ParamList* plist = zobj_toc<ParamList>(pobj_mgr);

	this->wipe();

	str_rc sql = plist->getSql();
	//showstr("prepare", sql);

	sobj->prepare(sql);

	htab_ptr values( plist->getValues());

	if (values.size())
	{
		sobj->setValues(values);
	}

	return s;

}

void 
Operation::returns(htab_ptr list)
{
	Bindings& bind = bindings();
	bind.addarray(ISql::SQL_RETURN, list);
}

val_rc 
Operation::run()
{
	obj_rc s = prepare(IDriver::FETCH_ASSOC);
	Simple* sobj = zobj_toc<Simple>(s);
	return sobj->run();
}

void 
Operation::where(val_ptr lattr, val_ptr rattr, int op, int blogic)
{
	htab_rc data;

	htab_rw hw(data);

	if(op < 0)
	{
		op = JoinExpr::OP_EQ;
	}
	str_ptr opstr = JoinExpr::opStr(op);
	if (blogic < 0)
	{
		blogic = JoinExpr::B_AND;
	}
	str_ptr bstr = JoinExpr::boolStr(blogic);
	Bindings& bind = bindings();
	bind.where(lattr, opstr, rattr, bstr);

}

void 
Operation::wipe()
{
	if (bind_.ok())
	{
		Bindings& bind = *zobj_toc<Bindings>(bind_);
		bind.wipe();
		bind_.init();
	}
	//showobj("Joiner init", joiner_);
	joiner_.init();
	driver_.init();
}


}; // namespace wcd;

using namespace wcd;

ZEND_METHOD(Wcd_Sql_Operation, __construct)
{
	zarg_rd args(execute_data);

	obj_ptr driver;

	args.obj_ofclass(driver, args.need(1), IDriver::omg.class_entry_);

	if (!args.throw_errors())
	{
		Operation* cobj = zval_toc<Operation>(ZEND_THIS);

		cobj->construct(driver);
	}
}


ZEND_METHOD(Wcd_Sql_Operation, __destruct)
{
	ZEND_PARSE_PARAMETERS_NONE();
	Operation* cobj = zval_toc<Operation>(ZEND_THIS);
	cobj->destruct();
}

ZEND_METHOD(Wcd_Sql_Operation, addPrime)
{
	zarg_rd args(execute_data);

	str_ptr table;
	str_ptr alias;
	htab_ptr cols;

	args.zstring(table, args.need(1));
	args.zstring_null(alias, args.option(2));
	args.zarray_null(cols, args.option(3));


	if (!args.throw_errors())
	{
		Operation* cobj = zval_toc<Operation>(ZEND_THIS);

		obj_rc result(cobj->addPrime(table, alias, cols));

		result.move_zv(return_value);
	}
}

ZEND_METHOD(Wcd_Sql_Operation, firstRow)
{
	zarg_rd args(execute_data);

	str_ptr table;
	str_ptr alias;
	htab_ptr cols;

	args.zstring(table, args.need(1));
	args.zstring_null(alias, args.option(2));
	args.zarray_null(cols, args.option(3));


	if (!args.throw_errors())
	{
		Operation* cobj = zval_toc<Operation>(ZEND_THIS);

		obj_rc result(cobj->addPrime(table, alias, cols));

		result.move_zv(return_value);
	}
}

ZEND_METHOD(Wcd_Sql_Operation, getBind)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Operation* cobj = zval_toc<Operation>(ZEND_THIS);

	obj_ptr result = cobj->getBind();

	result.return_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Operation, getDb)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Operation* cobj = zval_toc<Operation>(ZEND_THIS);

	obj_ptr result = cobj->getDb();

	result.return_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Operation, getJoiner)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Operation* cobj = zval_toc<Operation>(ZEND_THIS);

	obj_ptr result = cobj->getJoiner();

	result.return_zv(return_value);
}


ZEND_METHOD(Wcd_Sql_Operation, getRows)
{

	zarg_rd args(execute_data);

	zend_long fetch = -1;

	args.zlong(fetch, args.option(1));

	if (fetch < 0)
	{
		fetch = IDriver::FETCH_ASSOC;
	}
	val_rc result;

	if (!args.throw_errors())
	{
		Operation* cobj = zval_toc<Operation>(ZEND_THIS);
		result = cobj->getRows(fetch);
		result.move_zv(return_value);
	}
}

ZEND_METHOD(Wcd_Sql_Operation, getSqlParams)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Operation* cobj = zval_toc<Operation>(ZEND_THIS);

	obj_rc result = cobj->getSqlParams();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Operation, getSql)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Operation* cobj = zval_toc<Operation>(ZEND_THIS);

	str_rc result = cobj->getSql();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Operation, limit)
{
	zarg_rd args(execute_data);

	val_ptr maxct(args.need(1));
	val_ptr start(args.option(2));

	Operation* cobj = zval_toc<Operation>(ZEND_THIS);

	cobj->limit(maxct, start);

}

ZEND_METHOD(Wcd_Sql_Operation, orderBy)
{
	zarg_rd args(execute_data);

	val_ptr column(args.need(1));
	bool      descend;

	if (!args.zbool(descend,args.option(2)))
	{
		descend = false;
	}

	if (!args.throw_errors())
	{
		Operation* cobj = zval_toc<Operation>(ZEND_THIS);

		cobj->orderBy(column, descend);
	}
}

ZEND_METHOD(Wcd_Sql_Operation, prepare)
{
	zarg_rd args(execute_data);

	zend_long fetch = -1;

	args.zlong(fetch, args.option(1));

	if (fetch < 0)
	{
		fetch = IDriver::FETCH_ASSOC;
	}
	obj_rc result;

	if (!args.throw_errors())
	{
		Operation* cobj = zval_toc<Operation>(ZEND_THIS);
		result = cobj->prepare(fetch);
		result.move_zv(return_value);
	}
}

ZEND_METHOD(Wcd_Sql_Operation, returns)
{
	zarg_rd args(execute_data);

	htab_ptr rvalues;

	args.zarray(rvalues, args.need(1));

	if (!args.throw_errors())
	{
		Operation* cobj = zval_toc<Operation>(ZEND_THIS);
		cobj->returns(rvalues);
	}
}

ZEND_METHOD(Wcd_Sql_Operation, run)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Operation* cobj = zval_toc<Operation>(ZEND_THIS);

	val_rc result(cobj->run());

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Operation, where)
{
	zarg_rd args(execute_data);

	val_ptr lattr(args.need(1));
	val_ptr rattr(args.option(2));

	zend_long op = -1;
	zend_long blogic = -1;

	args.zlong(op, args.option(3));
	args.zlong(blogic, args.option(4));

	if (!args.throw_errors())
	{
		Operation* cobj = zval_toc<Operation>(ZEND_THIS);
		cobj->where(lattr, rattr, op, blogic);
	}
}


ZEND_METHOD(Wcd_Sql_Operation, wipe)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Operation* cobj = zval_toc<Operation>(ZEND_THIS);

	cobj->wipe();
}

PHP_MINIT_FUNCTION(Wcd_Operation_reg)
{
	zend_class_entry* temp;

	temp = register_class_Wcd_Sql_Operation();

	Operation::omg.classEntry(temp);

	Select::omg.classEntry(register_class_Wcd_Sql_Select(temp));
	Delete::omg.classEntry(register_class_Wcd_Sql_Delete(temp));
	Insert::omg.classEntry(register_class_Wcd_Sql_Insert(temp));
	Update::omg.classEntry(register_class_Wcd_Sql_Update(temp));

	return SUCCESS;
}

#endif