#ifndef ZARG_EXEC_CPP
#define ZARG_EXEC_CPP

#ifndef ZARG_EXEC_H
#include "zarg_exec.h"
#endif

namespace zpp {
zstr_buffer& 
zarg_exec::error()
{
	if (!errors_)
	{
		errors_ = new zstr_buffer();
	}
	return *errors_;
}

zarg_exec::~zarg_exec()
{
	if (errors_) 
	{
		delete errors_;
	}
}

zarg_exec::zarg_exec(zend_execute_data* ze) : errors_(nullptr)
{
	zptr0_ = (zval*)(ZEND_CALL_VAR_NUM(ze, 0));
	nargs_ = ZEND_CALL_NUM_ARGS(ze);
	option_ = 0;
}

zval*
zarg_exec::option(size_t ix)
{
	option_ = 1;
	if ((ix < 1) || (ix > nargs_))
	{
		return (zval*)nullptr;
	}
	return zptr0_ + (ix-1);
}

zval* 
zarg_exec::need(size_t ix)
{
	option_ = 0;
	if ((ix < 1) || (ix > nargs_))
	{
		error() << "; Bad argument index " << ix;
		return (zval*)nullptr;
	}
	return zptr0_ + (ix-1);
}

bool 
zarg_exec::zstring(zstr_user& value, zval* arg)
{

	zval_user test(arg);
	value = test.zstr();
	if (value.ok())
	{
		return true;
	}
	if (!option_)
	{
		error() << "; Expect string";
	}
	return false;
}

bool 
zarg_exec::zstring_null(zstr_user& value, zval* arg)
{
	zval_user test(arg);
	value = test.zstr();
	if (value.ok() || test.isNull())
	{
		value = arg;
		return true;
	}
	if (!option_)
	{
		error() << "; Expect string or NULL";
	}
	return false;
}

bool 
zarg_exec::zarray_null(htab_read& value, zval* arg)
{
	zval_user test(arg);
	int itype = test.ref_type();
	if (itype == IS_ARRAY || itype == IS_NULL)
	{
		value = test.zarray();
		return true;
	}
	if (!option_)
	{
		error() << "; Expect Array or NULL";
	}
	return false;
}


bool 
zarg_exec::zarray(htab_read& value, zval* arg)
{
	zval_user test(arg);
	int itype = test.ref_type();

	if (itype == IS_ARRAY)
	{
		value = test.zarray();
		return true;
	}
	if (!option_)
	{
		error() << "; Expect Array or NULL";
	}
	return false;
}

bool
zarg_exec::obj_ofclass_null(zobj_user& value, zval* arg, zend_class_entry* ce)
{
	value = zobj_user(arg);
	if (value.instanceof(ce) || value.isNull()) {
		return true;
	}
	zstr_user name(ce->name);
	if (!option_)
	{
		error() << "; Expect NULL or object of class " << name;
	}
	return false;
}

bool 
zarg_exec::obj(zobj_user& value, zval* arg)
{
	value = zobj_user(arg);
	bool result = value.ok();
	if (!option_ && !result)
	{
		error() << "; Expected object";
	}
	return result;
}

bool 
zarg_exec::obj_null(zobj_user& value, zval* arg)
{
	zval_user test(arg);
	int itype = test.ref_type();
	if (itype != IS_OBJECT && itype != IS_NULL)
	{
		if (!option_) {
			error() << "; Expected Object or NULL";
		}
		return false;
	}
	return true;
}

bool
zarg_exec::obj_ofclass(zobj_user& value, zval* arg, zend_class_entry* ce)
{
	value = zobj_user(arg);
	if (value.instanceof(ce)) {
		return true;
	}
	zstr_user name(ce->name);
	if (!option_)
	{
		error() << "; Expect object of class " << name;
	}
	return false;
}

bool 
zarg_exec::zlong_null(zend_long& value, zval* arg)
{
	zval_user test(arg);
	int itype = test.ref_type();
	if (itype != IS_LONG && itype != IS_NULL) 
	{
		if (!option_) {
			error() << "; Expected integer value or NULL";
		}
		
		return false;
	}
	value = test.zlong();
	return true;
}

bool 
zarg_exec::zbool(bool& value, zval* arg)
{
	zval_user test(arg);
	int itype = test.ref_type();
	if (itype == IS_TRUE || itype == IS_FALSE)
	{
		value = test.zbool();
		return true;
	}
	if (!option_)
	{
		error() << "; Expected bool value";
	}
	return false;
}


bool 
zarg_exec::zlong(zend_long& value, zval* arg)
{
	zval_user test(arg);
	if (!test.isLong()) 
	{
		if (!option_)
		{
			error() << "; Expected integer value";
		}
		return false;
	}
	value = test.zlong();
	return true;
}


zstr_mgr 
zarg_exec::get_errors()
{
	zstr_mgr result;

	if (errors_)
	{	
		result =  errors_->zstr();
		delete errors_;
		errors_ = nullptr;
	}
	return result;
}

bool 
zarg_exec::throw_errors()
{
	if (errors_)
	{
		zstr_mgr s = errors_->zstr();
		zend_throw_error(zend_ce_error,"Errors %s: ", s.data());
		delete errors_;
		errors_ = nullptr;
		return true;
	}
	return false;
}

}; //namespace
#endif