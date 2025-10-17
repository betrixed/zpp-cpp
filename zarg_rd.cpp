#ifndef ZARG_RD_CPP
#define ZARG_RD_CPP

/**
 * @file zarg_rd.cpp
 * @author Michael Rynn <michael.rynn.500@gmail.com>
 * @brief Read arguments from zend_execute_data
 * @copyright Copyright (c) 2025 Michael Rynn
 * @license MIT License
 * 
 */
#ifndef ZARG_RD_H
#include "zarg_rd.h"
#endif

namespace zpp {
str_buf& 
zarg_rd::error()
{
	if (!errors_)
	{
		errors_ = new str_buf();
	}
	return *errors_;
}

zarg_rd::~zarg_rd()
{
	if (errors_) 
	{
		delete errors_;
	}
}

zarg_rd::zarg_rd(zend_execute_data* ze) : errors_(nullptr)
{
	zptr0_ = (zval*)(ZEND_CALL_VAR_NUM(ze, 0));
	nargs_ = ZEND_CALL_NUM_ARGS(ze);
	option_ = 0;
/*
	#if ZEND_DEBUG
		ze_ = ze;
	#endif
*/
}

zval*
zarg_rd::option(size_t ix)
{
	option_ = 1;
	zval* zptr;

	if (ix >= nargs_)
	{
		zptr = (zval*) nullptr;
	}
	else {
		zptr = zptr0_ + ix;
	}
	return zptr;
}

zval* 
zarg_rd::need(size_t ix)
{
	option_ = 0;
	zval* zptr;

	if (ix >= nargs_)
	{
		error() << "; Bad argument index " << ix;
		zptr = (zval*) nullptr;
	}
	else {
		zptr = zptr0_ + ix;
	}
	return zptr;
}

bool 
zarg_rd::zstring(str_ptr& value, zval* arg)
{

	val_ptr test(arg);
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
zarg_rd::zstring_null(str_ptr& value, zval* arg)
{
	val_ptr test(arg);
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
zarg_rd::zarray_null(htab_ptr& value, zval* arg)
{
	val_ptr test(arg);
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
zarg_rd::zarray(htab_ptr& value, zval* arg)
{
	val_ptr test(arg);
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
zarg_rd::obj_ofclass_null(obj_ptr& value, zval* arg, zend_class_entry* ce)
{
	value = obj_ptr(arg);
	if (value.instanceof(ce) || value.isNull()) {
		return true;
	}
	str_ptr name(ce->name);
	if (!option_)
	{
		error() << "; Expect NULL or object of class " << name;
	}
	return false;
}

bool 
zarg_rd::obj(obj_ptr& value, zval* arg)
{
	value = obj_ptr(arg);
	bool result = value.ok();
	if (!option_ && !result)
	{
		error() << "; Expected object";
	}
	return result;
}

bool 
zarg_rd::obj_null(obj_ptr& value, zval* arg)
{
	val_ptr test(arg);
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
zarg_rd::obj_ofclass(obj_ptr& value, zval* arg, zend_class_entry* ce)
{
	value = obj_ptr(arg);
	if (value.instanceof(ce)) {
		return true;
	}
	str_ptr name(ce->name);
	if (!option_)
	{
		error() << "; Expect object of class " << name;
	}
	return false;
}

bool 
zarg_rd::zlong_null(zend_long& value, zval* arg)
{
	val_ptr test(arg);
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
zarg_rd::zbool(bool& value, zval* arg)
{
	val_ptr test(arg);
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
zarg_rd::zlong(zend_long& value, zval* arg)
{
	val_ptr test(arg);
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


str_rc 
zarg_rd::get_errors()
{
	str_rc result;

	if (errors_)
	{	
		result =  errors_->zstr();
		delete errors_;
		errors_ = nullptr;
	}
	return result;
}

bool 
zarg_rd::throw_errors(const char* fncstr)
{
	if (errors_)
	{
		*errors_ << " *** ZARG_RD: ERROR in " << fncstr;
		str_rc s = errors_->zstr();
		zend_throw_error(zend_ce_error,"%s", s.data());
		delete errors_;
		errors_ = nullptr;
		return true;
	}
	return false;
}

}; //namespace
#endif