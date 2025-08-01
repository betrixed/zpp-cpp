#ifndef WCD_OPERATION_CPP
#define WCD_OPERATION_CPP

#ifndef WCD_OPERATION_H
#include "operation.h"
#endif


#ifndef SQL_PART_H
#include "sql_ipart.h"
#endif

namespace wcd {


using namespace zpp;
using namespace wcc;

base_obj_mgr<Operation> Operation::omg;

void 
Operation::debug_info(htab_write di)
{
	di.set(SQSTR.driver, db_);
	di.set(SQSTR.bind_key, bind_);
	di.set(SQSTR.join_tables, joiner_);
}

void 
Operation::construct(zobj_user db)
{
	db_ = db;
	IDriver* dr = zobj_toc<IDriver>(db);

	bind_ = dr->newBindings();

	wipe();

}

void 
Operation::destruct()
{
	joiner_.init();
	db_.init();
}

zobj_mgr 
Operation::addPrime(zstr_user table, zstr_user alias, htab_read cols)
{
	zobj_mgr tc = TColumns::omg.new_zobj();
	TColumns* tcobj = zobj_toc<TColumns>(tc);

	zval_mgr tval(cols);
	tcobj->construct(table, alias, tval);

	JoinTables* jt = zobj_toc<JoinTables>(joiner_);
	jt->setPrime(tc);
	return tc;
}

zval_mgr 
Operation::firstRow(int fetch)
{
	zval_mgr result = getRows(fetch);

	htab_read rows(result.zarray());
	if (rows.size())
	{
		return rows.get(int(0));
	}
	result.set_null();
	return result;
}

zobj_user
Operation::getJoiner()
{
	return joiner_;
}

zval_mgr 
Operation::getRows(int fetch)
{
	zval_mgr result;

	zobj_mgr simple( prepare( fetch ));

	if (simple.ok())
	{
		Simple* s = zobj_toc<Simple>(simple);

		result = s->getRows();
	}
	return result;
}

zobj_mgr
Operation::getSqlParams()
{
	Bindings& bind = *zobj_toc<Bindings>(bind_);
	return bind.getParamList();
}

void 
Operation::limit(zval_user ct, zval_user start)
{
	Bindings& bind = *zobj_toc<Bindings>(bind_);

	bind.limit(ct, start);
}

void 
Operation::orderBy(zval_user column, bool descend)
{
	Bindings& bind = *zobj_toc<Bindings>(bind_);
	bind.orderBy(column, descend);
}

zobj_mgr 
Operation::prepare(int fetch)
{
	if (fetch < 0)
	{
		fetch = IDriver::FETCH_ASSOC;
	}

	zobj_mgr s = Simple::omg.new_zobj();
	Simple* sobj = zobj_toc<Simple>(s);

	Bindings& bind = *zobj_toc<Bindings>(bind_);
	zval_mgr retvals = bind.get(ISql::SQL_RETURN);

	if (retvals.ok())
	{
		sobj->returnsValues(true);
	}
	zobj_mgr pobj_mgr = getSqlParams();
	ParamList* plist = zobj_toc<ParamList>(pobj_mgr);
	bind.wipe();

	zstr_mgr sql = plist->getSql();
	sobj->prepare(sql);

	htab_read values( plist->getValues());

	if (values.size())
	{
		sobj->setValues(values);
	}

	return s;

}

void 
Operation::returns(htab_read list)
{
	Bindings& bind = *zobj_toc<Bindings>(bind_);
	bind.addarray(ISql::SQL_RETURN, list);
}

zval_mgr 
Operation::run()
{
	zobj_mgr s = prepare(IDriver::FETCH_ASSOC);
	Simple* sobj = zobj_toc<Simple>(s);
	return sobj->run();
}

void 
Operation::where(zval_user lattr, zval_user rattr, int op, int blogic)
{
	htab_mgr data;

	htab_write hw(data);

	if(op < 0)
	{
		op = JoinExpr::OP_EQ;
	}
	zstr_user opstr = JoinExpr::opStr(op);
	if (blogic < 0)
	{
		blogic = JoinExpr::B_AND;
	}
	zstr_user bstr = JoinExpr::boolStr(blogic);
	Bindings& bind = *zobj_toc<Bindings>(bind_);
	bind.where(lattr, opstr, rattr, bstr);

}

void 
Operation::wipe()
{
	Bindings& bind = *zobj_toc<Bindings>(bind_);
	bind.wipe();

	zobj_mgr joiner_ = JoinTables::omg.new_zobj();
	zval_mgr a1(joiner_);
	bind.set(ISql::SQL_FROM, a1);
}


}; // namespace wcd;

using namespace wcd;

ZEND_METHOD(Wcd_Sql_Operation, __construct)
{
	zarg_exec args(execute_data);

	zobj_user driver;

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
	zarg_exec args(execute_data);

	zstr_user table;
	zstr_user alias;
	htab_read cols;

	args.zstring(table, args.need(1));
	args.zstring_null(alias, args.option(2));
	args.zarray_null(cols, args.option(3));


	if (!args.throw_errors())
	{
		Operation* cobj = zval_toc<Operation>(ZEND_THIS);

		zobj_mgr result(cobj->addPrime(table, alias, cols));

		result.move_zv(return_value);
	}
}

ZEND_METHOD(Wcd_Sql_Operation, firstRow)
{
	zarg_exec args(execute_data);

	zstr_user table;
	zstr_user alias;
	htab_read cols;

	args.zstring(table, args.need(1));
	args.zstring_null(alias, args.option(2));
	args.zarray_null(cols, args.option(3));


	if (!args.throw_errors())
	{
		Operation* cobj = zval_toc<Operation>(ZEND_THIS);

		zobj_mgr result(cobj->addPrime(table, alias, cols));

		result.move_zv(return_value);
	}
}

ZEND_METHOD(Wcd_Sql_Operation, getJoiner)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Operation* cobj = zval_toc<Operation>(ZEND_THIS);

	zobj_user result = cobj->getJoiner();

	result.return_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Operation, getParams)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Operation* cobj = zval_toc<Operation>(ZEND_THIS);
	
	htab_read result = cobj->getParams();

	result.return_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Operation, getRows)
{

	zarg_exec args(execute_data);

	zend_long fetch = -1;

	args.zlong(fetch, args.option(1));

	if (fetch < 0)
	{
		fetch = IDriver::FETCH_ASSOC;
	}
	zval_mgr result;

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

	zobj_mgr result = cobj->getSqlParams();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Operation, limit)
{
	zarg_exec args(execute_data);

	zval_user maxct(args.need(1));
	zval_user start(args.option(2));

	Operation* cobj = zval_toc<Operation>(ZEND_THIS);

	cobj->limit(maxct, start);

}

ZEND_METHOD(Wcd_Sql_Operation, orderBy)
{
	zarg_exec args(execute_data);

	zval_user column(args.need(1));
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
	zarg_exec args(execute_data);

	zend_long fetch = -1;

	args.zlong(fetch, args.option(1));

	if (fetch < 0)
	{
		fetch = IDriver::FETCH_ASSOC;
	}
	zobj_mgr result;

	if (!args.throw_errors())
	{
		Operation* cobj = zval_toc<Operation>(ZEND_THIS);
		result = cobj->prepare(fetch);
		result.move_zv(return_value);
	}
}

ZEND_METHOD(Wcd_Sql_Operation, returns)
{
	zarg_exec args(execute_data);

	htab_read rvalues;

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

	zval_mgr result(cobj->run());

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Operation, where)
{
	zarg_exec args(execute_data);

	zval_user lattr(args.need(1));
	zval_user rattr(args.option(2));

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
	Operation::omg.classEntry(register_class_Wcd_Sql_Operation());
	return SUCCESS;
}

#endif