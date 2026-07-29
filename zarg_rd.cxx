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

#ifndef SHOW_ZPP_H
#include "show_zpp.h"
#endif

#ifndef ZEND_WEAKREFS_H
extern "C" {
    #include <zend_weakrefs.h>
}
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


void //static 
zarg_rd::list_args(str_buf& args, zval* p0, size_t ct)
{
	dump_info  din(args); // connect buffer
	din.di_show_slice(p0, ct);
}

bool //static 
zarg_rd::more_args(zend_execute_data* ze, const char* fn, size_t maxa)
{
	size_t  nargs = ZEND_CALL_NUM_ARGS(ze);
	if (nargs > maxa) {
		str_buf args;
		args << fn << " More than " << maxa << " arguments: " << nargs << ':' << endl;
		zval* p0 = (zval*)(ZEND_CALL_VAR_NUM(ze, 0));
		list_args(args, p0, nargs);
		str_rc msg = args.zstr();
		zend_throw_error(zend_ce_error,"%s", msg.data());
		return false;
	}
	return true;
}

bool //static 
zarg_rd::zero_args(zend_execute_data* ze, const char* fn)
{
	return zarg_rd::more_args(ze, fn, 0);
}

zarg_rd::zarg_rd(zend_execute_data* ze) : errors_(nullptr)
{
	zptr0_ = (zval*)(ZEND_CALL_VAR_NUM(ze, 0));
	nargs_ = ZEND_CALL_NUM_ARGS(ze);
	maybe_ = false;
/*
	#if ZEND_DEBUG
		ze_ = ze;
	#endif
*/
}

zval*
zarg_rd::option(size_t ix)
{
	maybe_ = 1;
	zval* zptr;
	ix_ = ix;

	if (ix >= nargs_)
	{
		zptr = (zval*) nullptr;
	}
	else {
		zptr = zptr0_ + ix;
	}
	return zptr;
}

bool 
zarg_rd::ztype(val_ptr& value, zval* arg, int ptype)
{
	value = arg;
	int rtype = value.ref_type();
	if (rtype != ptype)
	{
		error() << "; Expected TYPE " << ptype << ",  got ";
		wrong(arg);
		return false;
	}
	return true;
}

zval* 
zarg_rd::need(size_t ix)
{
	maybe_ = false;
	zval* zptr;
	ix_ = ix;

	if (ix >= nargs_)
	{
		error() << "; Bad argument index " << ix_;
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
	if (!maybe_)
	{
		error() << "; Expect string";
	}
	return false;
}

str_ptr 
zarg_rd::str(zval *arg)
{
	str_ptr result;

	if (!arg && maybe_)
	{
		return result;
	}
	result = arg;
	if (result.ok())
	{
		return result;
	}
	error() << " # Not a string ";
	wrong(arg);

	return result;
}

htab_ptr 
zarg_rd::htab(zval *arg)
{
	htab_ptr result;
	if (!arg && maybe_)
	{
		result = htab_ptr::empty_array();
		//showarray("arg", result);
		return result;
	}
	val_ptr test(arg);

	switch(test.ztype())
	{
	case IS_ARRAY:
		result = test.zarray();
		break;
	default:
		if (maybe_) {
			result = htab_ptr::empty_array();
		}
		else {
			error() << "Array value expected";
		}
		break;
	}
	//showarray("arg", result);
	return result;
}

htab_ptr
zarg_rd::htab_or_null(zval *arg)
{
	htab_ptr result;
	if (!arg && maybe_)
	{
		//showarray("arg", result);
		return result;
	}
	val_ptr test(arg);

	switch(test.ztype())
	{
	case IS_NULL:
		 break;
	case IS_ARRAY:
		result = test.zarray();
		break;
	default:
		if (!maybe_) {
			error() << "Array value or Null expected";
		}
		break;
	}
	//showarray("arg", result);
	return result;
}

str_ptr 
zarg_rd::str_or_null(zval *arg)
{
	str_ptr result;

	if (!arg && maybe_)
	{
		return result;
	}
	result = arg;
	if (result.ok() || result.isNull())
	{
		return result;
	}
	error() << "# Not a string OR null ";
	wrong(arg);
	return result;
}

str_ptr
zarg_rd::str_or_default(zval *arg, const str_intern& strdef)
{
	str_ptr result;
	if (!arg && maybe_)
	{
		result = strdef;
		return result;
	}
	val_ptr test(arg);
	result = test.zstr();

	if (result.ok())
	{
		return result;
	}
	wrong(arg);
	return result;
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
	if (!maybe_)
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
	if (!maybe_)
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
	if (!maybe_)
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
	if (!maybe_)
	{
		error() << "; Expect NULL or object of class " << name;
	}
	return false;
}

val_ptr
zarg_rd::string_or_array(zval* arg)
{
	val_ptr result(arg);

	if (result.isString())
	{
		return result;
	}
	if (result.isArray())
	{
		return result;
	}
	if (!maybe_)
	{
		error() << "String or Array required";
	}
	return result;
}

obj_ptr 
zarg_rd::obj(zval* arg)
{
	obj_ptr result;

	if (!arg && maybe_)
	{
		return result;
	}
	result = arg;
	if (result.ok())
	{
		return result;
	}
	error() << "# Not object: ";
	wrong(arg);

	return result;
}

obj_ptr 
zarg_rd::obj_ornull(zval* arg)
{
	obj_ptr result;

	if (!arg && maybe_)
	{
		return result;
	}
	result = arg;

	if (result.ok())
	{
		return result;
	}

	val_ptr test(arg);
	if (test.isNull())
	{
		return result;
	}

	error() << "# Not object or null: ";
	wrong(arg);

	return result;
}

obj_ptr 
zarg_rd::objclass_ornull(zval* arg, zend_class_entry* ce)
{
	obj_ptr result;

	if (!arg && maybe_)
	{
		return result;
	}

	result = arg;

	if (result.ok() && result.instanceof(ce))
	{
		return result;
	}
	error() << "# Not null or object of class ";
	wrong_notclass(arg, ce);
	return result;
}

obj_ptr 
zarg_rd::obj_class(zval* arg, zend_class_entry* ce)
{
	obj_ptr result;

	if (!arg && maybe_)
	{
		return result;
	}

	result = arg;

	if (result.ok() && result.instanceof(ce))
	{
		return result;
	}
	error() << "# Not object of class ";
	wrong_notclass(arg, ce);
	return result;
}

bool 
zarg_rd::obj(obj_ptr& value, zval* arg)
{
	value = obj_ptr(arg);
	bool result = value.ok();
	if (!maybe_ && !result)
	{
		error() << "; Expected object";
	}
	return result;
}

bool 
zarg_rd::weakref(weak_ref& value, zval* arg)
{
	value = obj_ptr(arg);
	bool result = value.ok();
	if (result)
	{
		if (value.class_entry() != zend_ce_weakref)
		{
			error() << "; Expected WeakReference class";
			result = false;
		}
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
		if (!maybe_) {
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
	if (!maybe_)
	{
		error() << "; Expect object of class " << name;
	}
	return false;
}

bool 
zarg_rd::zlong_null(zend_long& value, zval* arg, zend_long ifnull)
{
	val_ptr test(arg);
	int itype = test.ref_type();

	switch(itype) {
		case IS_LONG:
			value = test.zlong();
			break;
		case IS_NULL:
			value = ifnull;
			break;
		default:
                                                if (!maybe_) {
			error() << "; Expected integer value or NULL";
			return false;
                                                }
	}
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
	if (!maybe_)
	{
		error() << "; Expected bool value";
                                      return false;
	}

	return false;
}


bool 
zarg_rd::zlong(zend_long& value, zval* arg)
{
	val_ptr test(arg);
	if (!test.isLong()) 
	{
		if (!maybe_)
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
		*errors_ << " : zard_rd::threw_errors in " << fncstr << "<br>\n";
		list_args(*errors_, zptr0_, nargs_);

		str_rc s = errors_->zstr();


		zend_throw_error(zend_ce_error,"%s", s.data());
		delete errors_;
		errors_ = nullptr;
		return true;
	}
	return false;
}

void 
zarg_rd::wrong(zval* arg)
{
	dump_info di(error());
	*errors_ << endl;
}

void 
zarg_rd::wrong_notclass(zval* arg, zend_class_entry* ce)
{
	dump_info di(error());
	*errors_ << " Not class " << ce->name << endl;
}

}; //namespace
#endif