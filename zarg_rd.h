#ifndef ZARG_RD_H
#define ZARG_RD_H

/**
 * @file zarg_rd.h
 * @author Michael Rynn <michael.rynn.500@gmail.com>
 * @brief zpp::zarg_rd - Argument reader for PHP functions/method
 * @copyright Copyright (c) 2025 Michael Rynn
 * @license BSD 3-Clause License
 */
#ifndef OBJ_PTR_H
#include "obj_ptr.h"
#endif

#ifndef STR_BUF_H
#include "str_buf.h"
#endif

namespace zpp {

/**
 * @class zarg_rd
 * @brief Argument reader for PHP functions/methods.	
 * @details
 *  Use in PHP functions/methods to read and validate arguments.
 *  Throw zend exceptions on error.
 *  
 */


class zval_slice {
protected:
	zval*	zptr0_;
	size_t  nargs_;
public:
	size_t  size() const {
		return nargs_;
	}
	zval*   ptr() const {
		return zptr0_;
	}
};

class zarg_rd : public zval_slice
{
protected:
	str_buf*          	  errors_;
	bool              	  maybe_;
/*
#if ZEND_DEBUG
	zend_execute_data* 	  ze_;
	zval*                 argptr_;
#endif
*/
	
public:
	zarg_rd(zend_execute_data* ze);
	~zarg_rd();

	str_buf& error();

	zval* need(size_t ix);
	zval* option(size_t ix);

	bool zstring(str_ptr& value, zval* arg);
	bool zstring_null(str_ptr& value, zval* arg);

	bool obj(obj_ptr& value, zval* arg);
	bool obj_null(obj_ptr& value, zval* arg);

	bool obj_ofclass(obj_ptr& value, zval* arg, zend_class_entry* ce);
	bool obj_ofclass_null(obj_ptr& value, zval* arg, zend_class_entry* ce);
	
	bool zarray_null(htab_ptr& value, zval* arg);
	bool zarray(htab_ptr& value, zval* arg);

	bool zlong(zend_long& value, zval* arg);
	bool zlong_null(zend_long& value, zval* arg);

	bool zbool(bool& value, zval* arg);
	
	bool ztype(val_ptr& value, zval* arg, int ptype);

	bool     has_errors() const { return (errors_); }
	str_rc   get_errors();
	bool     throw_errors(const char* fncstr = nullptr);

	size_t size() const {
		return nargs_;
	}

};

}; //namespace zpp

#endif