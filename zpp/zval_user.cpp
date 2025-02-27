 /*  
  *  PHP extension C++ classes - zpp 
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  */

#ifndef ZVAL_USER_CPP
#define ZVAL_USER_CPP

#ifndef ZSTR_USER_H
#include "zstr_user.h"
#endif

namespace zpp {
	
zval_user zval_user::referent()
{
	if (p_) {
		zval* x = p_;
		ZVAL_DEREF(x);
		return zval_user(x);
	}
	return zval_user(p_);	
}


int  
zval_user::ref_type() const
{
    if (!p_) {
        return IS_NULL;
    }
    zval* zv = p_;
    ZVAL_DEREF(zv);
    return Z_TYPE_P(zv);
}

zend_string* 
zval_user::className() const
{
	if (!p_)
		return nullptr;

	zval* zv = (zval*) p_;

	ZVAL_DEREF(zv);
	if (Z_TYPE_P(zv) == IS_OBJECT)
	{
		zend_class_entry *ce = Z_OBJCE_P(zv);
		return ce->name;	
	}
	//TODO: Throw exception?
	return nullptr;
}

zend_object* 
zval_user::zobject() const
{
	if (!p_) {
		return nullptr;
	}
	if (Z_TYPE_P(p_) != IS_OBJECT) {
		return nullptr;
	}
	return Z_OBJ_P(p_);
}

size_t zval_user::size() const
{
	if (!p_) 
	{
		return 0;
	}
	auto ztype = Z_TYPE_P(p_);
	switch(ztype)
	{
	case IS_ARRAY:
		return zend_array_count(Z_ARRVAL_P(p_));
	case IS_STRING:
		return ZSTR_LEN(Z_STR(*p_));
	default:
		return 0;
	}
}

/*
* Return managed string
*/
zstr_mgr
zval_user::to_zstr() const 
{
	zstr_mgr result;

	if (!p_)
	{
		return result;
	}

	zval* zv = (zval*) p_;
	ZVAL_DEREF(zv);

	if (Z_TYPE_P(zv) != IS_STRING) {
		// return a string representation
		result.adopt(zval_get_string(zv));
	}
	else {
		result = zv;
	}
	return result;
}

zend_string*  
zval_user::zstr() const
{	
	if (!p_)
		return nullptr;
	zval* zv = p_;
	ZVAL_DEREF(zv);
	if (Z_TYPE_P(zv) != IS_STRING)
	{
		return nullptr;
	}
	return Z_STR_P(zv);
}

std::string 
zval_user::cstr() const
{
	zstr_mgr temp = to_zstr();
	return zstr_user(temp).cstr();	
}

std::string_view 
zval_user::vstr() const
{
	zstr_user temp(this->zstr());
	return temp.vstr();
}

bool 
zval_user::isCallable()  const
{
	if (!p_)
		return false;
	zval* zv = p_;
	ZVAL_DEREF(zv);
	return (zend_is_callable(zv, 0, nullptr));
}

bool 
zval_user::getStringData(zend_string** retstr) const 
{
	zend_string* s = this->zstr();

	if (retstr)
	{
		*retstr = s;
	}
	return ( (s) &&  (ZSTR_LEN(s) > 0));
}

double 
zval_user::zdouble() const
{
	if (!p_) {
		return 0.0; 
	}
	zval* zv = (zval*) p_;
	ZVAL_DEREF(zv);

	if (Z_TYPE_P(zv) != IS_DOUBLE) {
		return zval_get_double_func(zv);
	}
	return Z_DVAL_P(zv);
}

void* 
zval_user::voidptr() const 
{
	if (!p_) 
	{
		return nullptr; 
	}
	zval* zv = (zval*) p_;
	ZVAL_DEREF(zv);
	if ( Z_TYPE_P(zv) != IS_PTR) {
		return nullptr;
	}
	return Z_PTR_P(zv);
}

bool
zval_user::zbool() const {
	if (!p_)
	{
		return false;
	}
	zval* zv = (zval*) p_;
	ZVAL_DEREF(zv);
	int ztype = Z_TYPE_P(zv);
	switch(ztype) {
		case IS_TRUE: return true;
		case IS_FALSE: return false;
		default: {
			// become ridiculous
			zval_mgr temp(zv);
			convert_to_boolean(temp);
			return zval_user(temp).zbool();
		}
	}
}

zend_long 
zval_user::zlong() const
{
	if (!p_)
	{
		return 0;
	}
	zval* zv = (zval*)p_;
	ZVAL_DEREF(zv);

	int ztype = Z_TYPE_P(zv);
	switch(ztype) {
		case IS_LONG: return Z_LVAL_P(zv);
		case IS_TRUE: return 1;
		case IS_FALSE: return 0;
		default: return zval_get_long_func(zv, false);
	}
}

HashTable* 
zval_user::zarray() const
{
	if (!p_)
		return nullptr;
	zval* zv = (zval*) p_;
	ZVAL_DEREF(zv);

	if (Z_TYPE_P(zv) != IS_ARRAY) {
		return nullptr;
	}

	return Z_ARRVAL_P(zv);
}

};
//zval_user.cpp
#endif