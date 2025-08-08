#ifndef ZARG_RD_H
#define ZARG_RD_H

#ifndef OBJ_PTR_H
#include "obj_ptr.h"
#endif

#ifndef STR_BUF_H
#include "str_buf.h"
#endif

namespace zpp {


class zarg_rd 
{
protected:
	str_buf*          errors_;
	zval*				  zptr0_;
	uint32_t              nargs_;
	uint32_t              option_;

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
	
	bool zarray_null(htab_rd& value, zval* arg);
	bool zarray(htab_rd& value, zval* arg);

	bool zlong(zend_long& value, zval* arg);
	bool zlong_null(zend_long& value, zval* arg);

	bool zbool(bool& value, zval* arg);
	
	bool     has_errors() const { return (errors_); }
	str_rc get_errors();
	bool     throw_errors();

	size_t size() const {
		return nargs_;
	}
};

}; //namespace zpp

#endif