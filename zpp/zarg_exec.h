#ifndef ZARG_EXEC_H
#define ZARG_EXEC_H

#ifndef ZOBJ_USER_H
#include "zobj_user.h"
#endif

#ifndef ZSTR_BUFFER_H
#include "zstr_buffer.h"
#endif

namespace zpp {


class zarg_exec 
{
protected:
	zstr_buffer*          errors_;
	zval*				  zptr0_;
	uint32_t              nargs_;
	uint32_t              option_;

public:
	zarg_exec(zend_execute_data* ze);
	~zarg_exec();

	zstr_buffer& error();

	zval* need(size_t ix);
	zval* option(size_t ix);

	bool zstring(zstr_user& value, zval* arg);
	bool zstring_null(zstr_user& value, zval* arg);

	bool obj(zobj_user& value, zval* arg);
	bool obj_null(zobj_user& value, zval* arg);

	bool obj_ofclass(zobj_user& value, zval* arg, zend_class_entry* ce);
	bool obj_ofclass_null(zobj_user& value, zval* arg, zend_class_entry* ce);
	
	bool zarray_null(htab_read& value, zval* arg);
	bool zarray(htab_read& value, zval* arg);

	bool zlong(zend_long& value, zval* arg);
	bool zlong_null(zend_long& value, zval* arg);

	bool zbool(bool& value, zval* arg);
	
	bool     has_errors() const { return (errors_); }
	zstr_mgr get_errors();
	bool     throw_errors();

	size_t size() const {
		return nargs_;
	}
};

}; //namespace zpp

#endif