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
	di.set(SQSTR.db_ref, dbref_);
	di.set(SQSTR.driver, driver_);
	di.set(SQSTR.join_tables, joiner_);
}

obj_return
Operation::getDb()
{
	obj_return result;
	if (!driver_.ok())
	{
		//zend_printf(	"get Driver");
		driver_ = dbref_.get();

		if (!driver_.ok())
		{
			result.error() << "IDriver weak_ref fail";
		}
		//showobj("Driver", driver_);
	}
	result.value_ = driver_;
	return result;
}

bool 
Operation::bindPtr(error_return& e, Bindings*& ptr)
{
	if (!bind_.ok())
	{
		obj_return obret = getBind();
		if (obret.has_errors())
		{
			e = std::move(obret);
			return false;
		}	
	}
	ptr = zobj_toc<Bindings>(bind_);
	return true;
}

bool 
Operation::dbPtr(error_return& e, IDriver*& ptr)
{
	if (!driver_.ok())
	{
		obj_return obret = getDb();
		if (obret.has_errors())
		{
			e = std::move(obret);
			return false;
		}	
	}
	ptr = zobj_toc<IDriver>(driver_);
	return true;
}

obj_return
Operation::getBind()
{
	obj_return result;

	if (!bind_.ok())
	{
		IDriver* dv = nullptr;
		if (!dbPtr(result,dv))
		{
			return result;
		}
		bind_ = dv->newBindings();
	}
	result.value_ = bind_;
	return result;
}

void 
Operation::construct(const weak_ref& db)
{
	dbref_ = db;
}

void 
Operation::destruct()
{
	bind_.init();
	joiner_.init();
	driver_.init();
	dbref_.init();
	
}

obj_return 
Operation::addPrime(str_ptr table, str_ptr alias, htab_ptr cols)
{
	obj_return result;

	obj_rc tc = TColumns::omg.new_zobj();
	TColumns* tcobj = zobj_toc<TColumns>(tc);

	val_rc tval(cols);
	tcobj->construct(table, alias, tval);

	//showobj("tcobj", tc);

	obj_return jt_ret = getJoiner();
	if (jt_ret.has_errors())
	{
		result = std::move(jt_ret);
		return result;
	}	
	JoinTables* jt = zobj_toc<JoinTables>(jt_ret.value_);
	jt->setPrime(tc);
	result.value_ = tc;
	return result;
}

val_return
Operation::firstRow(int fetch)
{
	val_return result = getRows(fetch);
	if (result.has_errors())
	{
		return result;
	}
	val_rc& data = result.value_;
	htab_ptr rows(data.zarray());
	if (rows.size())
	{
		result.value_ = rows.get(int(0));
		return result;
	}
	result.value_.set_null();
	return result;
}

obj_return
Operation::getJoiner()
{
	obj_return result;

	if (!joiner_.ok())
	{
		joiner_ = JoinTables::omg.new_zobj();
		Bindings* bind = nullptr;
		if (!bindPtr(result, bind))
		{
			return result;
		}
		val_rc arg(joiner_);
		bind->set(ISql::SQL_FROM, arg);
	}
	result.value_ = joiner_;
	return result;
}

val_return 
Operation::getRows(int fetch)
{
	val_return result;

	obj_return simple_ret = prepare( fetch );

	if (simple_ret.has_errors())
	{
		result = std::move(simple_ret);
		return result;
	}

	Simple* s = zobj_toc<Simple>(simple_ret.value_);

	return s->getRows();
}

obj_return
Operation::getSqlParams()
{
	obj_return result;
	Bindings* bind = nullptr;

	if (!bindPtr(result, bind))
	{
		return result;
	}

	result = bind->getParamList();
	if (result.has_errors())
	{
		return result;
	}
	this->wipe();
	return result;
}

/* this throws away the ParamList object
*/
str_return 
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

error_return 
Operation::limit(val_ptr ct, val_ptr start)
{
	error_return err;
	Bindings* bind = nullptr;
	
	if (!bindPtr(err, bind))
	{
		return err;
	}
	bind->limit(ct, start);
	return err;
}

error_return 
Operation::orderBy(val_ptr column, bool descend)
{
	error_return err;
	Bindings* bind = nullptr;
	if (!bindPtr(err, bind))
	{
		return err;
	}
	bind->orderBy(column, descend);
	return err;
}

obj_return 
Operation::prepare(int fetch)
{
	obj_return result;

	if (fetch < 0)
	{
		fetch = IDriver::FETCH_ASSOC;
	}

	obj_rc s = Simple::omg.new_zobj();

	Simple* sobj = zobj_toc<Simple>(s);

	IDriver* dv = nullptr;
	if (!dbPtr(result, dv))
	{
		return result;
	}

	sobj->construct(dbref_, fetch);
	Bindings* bind = nullptr;
	if (!bindPtr(result, bind))
	{
		return result;
	}

	val_rc retvals = bind->get(ISql::SQL_RETURN);

	if (retvals.ok())
	{
		sobj->returnsValues(true);
	}

	obj_ptr self(vobj());

	obj_rc pobj_mgr = self.call(SQSTR.get_sql_params);

	if (pobj_mgr.ok())
	{
		ParamList* plist = zobj_toc<ParamList>(pobj_mgr);

		this->wipe();

		str_rc sql = plist->getSql();
		//showstr("sql ", sql);

		bool_return check = sobj->prepare(sql);

		if (check.has_errors())
		{
			result = std::move(check);
			return result;
		}

		htab_ptr values( plist->getValues());

		if (values.size())
		{
			sobj->setValues(values);
		}
	}
	result.value_ = s;
	return result;
}

error_return 
Operation::returns(htab_ptr list)
{
	error_return err;

	obj_return bind_ret = getBind();
	if (bind_ret.has_errors())
	{
		err = std::move(bind_ret);
		return err;
	}
	Bindings* bind = zobj_toc<Bindings>(bind_ret.value_);
	bind->addarray(ISql::SQL_RETURN, list);
	return err;
}

val_return
Operation::run()
{
	val_return result;

	obj_return sret = prepare(IDriver::FETCH_ASSOC);

	if (sret.has_errors())
	{
		result = std::move(sret);
		return result;
	}

	Simple* sobj = zobj_toc<Simple>(sret.value_);
	return sobj->run();
}

error_return 
Operation::where(val_ptr lattr, val_ptr rattr, int op, int blogic)
{
	error_return err;

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
	obj_return bind_ret = getBind();
	if (bind_ret.has_errors())
	{
		err = std::move(bind_ret);
		return err;
	}
	Bindings* bind = zobj_toc<Bindings>(bind_ret.value_);
	bind->where(lattr, opstr, rattr, bstr);
	return err;
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

	weak_ref driver;

	args.weakref(driver, args.need(0));

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

	args.zstring(table, args.need(0));
	args.zstring_null(alias, args.option(1));
	args.zarray_null(cols, args.option(2));


	if (!args.throw_errors())
	{
		Operation* cobj = zval_toc<Operation>(ZEND_THIS);

		obj_return result = cobj->addPrime(table, alias, cols);
		result.throw_errors();
		result.value_.move_zv(return_value);
	}
}

ZEND_METHOD(Wcd_Sql_Operation, firstRow)
{
	zarg_rd args(execute_data);

	str_ptr table;
	str_ptr alias;
	htab_ptr cols;

	args.zstring(table, args.need(0));
	args.zstring_null(alias, args.option(1));
	args.zarray_null(cols, args.option(2));


	if (!args.throw_errors())
	{
		Operation* cobj = zval_toc<Operation>(ZEND_THIS);

		obj_return result = cobj->addPrime(table, alias, cols);
		result.throw_errors();
		result.value_.move_zv(return_value);
	}
}

ZEND_METHOD(Wcd_Sql_Operation, getBind)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Operation* cobj = zval_toc<Operation>(ZEND_THIS);

	obj_return result = cobj->getBind();
	result.throw_errors();
	result.value_.copy_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Operation, getDb)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Operation* cobj = zval_toc<Operation>(ZEND_THIS);

	obj_return result = cobj->getDb();
	result.throw_errors();
	result.value_.copy_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Operation, getJoiner)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Operation* cobj = zval_toc<Operation>(ZEND_THIS);

	obj_return result = cobj->getJoiner();
	result.throw_errors();
	result.value_.copy_zv(return_value);
}


ZEND_METHOD(Wcd_Sql_Operation, getRows)
{

	zarg_rd args(execute_data);

	zend_long fetch = -1;

	args.zlong(fetch, args.option(0));

	if (fetch < 0)
	{
		fetch = IDriver::FETCH_ASSOC;
	}
	val_return result;

	if (!args.throw_errors())
	{
		Operation* cobj = zval_toc<Operation>(ZEND_THIS);
		result = cobj->getRows(fetch);
		result.throw_errors();
		result.value_.move_zv(return_value);
	}
}

ZEND_METHOD(Wcd_Sql_Operation, getSqlParams)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Operation* cobj = zval_toc<Operation>(ZEND_THIS);

	obj_return result = cobj->getSqlParams();
	result.throw_errors();
	result.value_.move_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Operation, getSql)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Operation* cobj = zval_toc<Operation>(ZEND_THIS);

	str_return result = cobj->getSql();
	result.throw_errors();
	result.value_.move_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Operation, limit)
{
	zarg_rd args(execute_data);

	val_ptr maxct(args.need(0));
	val_ptr start(args.option(1));

	if (!args.throw_errors())
	{
		Operation* cobj = zval_toc<Operation>(ZEND_THIS);

		error_return result = cobj->limit(maxct, start);
		result.throw_errors();
	}

}

ZEND_METHOD(Wcd_Sql_Operation, orderBy)
{
	zarg_rd args(execute_data);

	val_ptr column(args.need(0));
	bool      descend;

	if (!args.zbool(descend,args.option(1)))
	{
		descend = false;
	}

	if (!args.throw_errors())
	{
		Operation* cobj = zval_toc<Operation>(ZEND_THIS);

		error_return result = cobj->orderBy(column, descend);
		result.throw_errors();
	}
}

ZEND_METHOD(Wcd_Sql_Operation, prepare)
{
	zarg_rd args(execute_data);

	zend_long fetch = -1;

	args.zlong(fetch, args.option(0));

	if (fetch < 0)
	{
		fetch = IDriver::FETCH_ASSOC;
	}
	obj_return result;

	if (!args.throw_errors())
	{
		Operation* cobj = zval_toc<Operation>(ZEND_THIS);
		result = cobj->prepare(fetch);
		result.throw_errors();
		result.value_.move_zv(return_value);
	}
}

ZEND_METHOD(Wcd_Sql_Operation, returns)
{
	zarg_rd args(execute_data);

	htab_ptr rvalues;

	args.zarray(rvalues, args.need(0));

	if (!args.throw_errors())
	{
		Operation* cobj = zval_toc<Operation>(ZEND_THIS);
		error_return result = cobj->returns(rvalues);
		result.throw_errors();
	}
}

ZEND_METHOD(Wcd_Sql_Operation, run)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Operation* cobj = zval_toc<Operation>(ZEND_THIS);

	val_return result = cobj->run();
	result.throw_errors();
	result.value_.move_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Operation, where)
{
	zarg_rd args(execute_data);

	val_ptr lattr(args.need(0));
	val_ptr rattr(args.option(1));

	zend_long op = -1;
	zend_long blogic = -1;

	args.zlong(op, args.option(2));
	args.zlong(blogic, args.option(3));

	if (!args.throw_errors())
	{
		Operation* cobj = zval_toc<Operation>(ZEND_THIS);
		error_return result = cobj->where(lattr, rattr, op, blogic);
		result.throw_errors();
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