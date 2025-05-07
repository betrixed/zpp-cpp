 /*  
  *  PHP extension C++ classes - zpp 
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  */

#ifndef ZVAL_USER_CPP
#define ZVAL_USER_CPP

#ifndef ZSTR_MGR_H
#include "zstr_mgr.h"
#endif

#ifndef ZVAL_USER_H
#include "zval_user.h"
#endif

#ifndef HTAB_READ_H
#include "htab_read.h"
#endif

#ifndef ZVAL_MGR_H
#include "zval_mgr.h"
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
    return Z_TYPE_P(zval_user::real_zval(p_));
}


bool 
zval_user::isDouble() const
{
    return (p_ && (ref_type() == IS_DOUBLE));
}

bool 
zval_user::isLong() const
{
    return (p_ && (ref_type() == IS_LONG));
}

bool 
zval_user::isArray() const
{
    return (p_ && (ref_type() == IS_ARRAY));
}


bool 
zval_user::isNull() const
{
    return (!(p_) || (ref_type() == IS_NULL));
}

bool 
zval_user::isObject() const
{ 
    return (p_ && (ref_type() == IS_OBJECT));
}


bool 
zval_user::ok() const {
	if (!p_)
		return false;
	zval* rp = zval_user::real_zval(p_);
	int rtype = Z_TYPE_P(rp);
    if (rtype < IS_TRUE)
    {
    	return false;
    }
    switch(rtype)
    {
    case IS_ARRAY:
    	return htab_read(Z_ARR_P(rp)).size() ? true : false;
    case IS_STRING:
    	return zstr_user(Z_STR_P(rp)).size() ? true : false;
    }
    // Don't care about zero LONG or DOUBLE
    return true;
}

bool 
zval_user::isString() const
{
    return (p_ && (ref_type() == IS_STRING));
}

bool 
zval_user::isTrue() const 
{
    return (p_ && (ref_type() == IS_TRUE));
}

bool 
zval_user::isFalse() const 
{
    return ( !(p_) || (ref_type() == IS_FALSE));
}

bool 
zval_user::isPointer() const
{
    return (p_ && (ref_type() == IS_PTR));
}

zend_string* 
zval_user::className() const
{
	if (!p_)
		return nullptr;

	zval* zv = zval_user::real_zval(p_);

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
	zval* zv = zval_user::real_zval(p_);

	if (Z_TYPE_P(zv) != IS_OBJECT) {
		return nullptr;
	}
	return Z_OBJ_P(zv);
}

size_t zval_user::size() const
{
	if (!p_) 
	{
		return 0;
	}
	zval* zv = zval_user::real_zval(p_);
	switch(Z_TYPE_P(zv))
	{
	case IS_ARRAY:
		return zend_array_count(Z_ARRVAL_P(zv));
	case IS_STRING:
		return ZSTR_LEN(Z_STR_P(zv));
	default:
		return 0;
	}
}

zval_user::zval_user(const zval_mgr& mgr) : p_((zval*) mgr)
{
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

	zval* zv = zval_user::real_zval(p_);

	if (Z_TYPE_P(zv) != IS_STRING) {
		// return a string representation
		result.adopt(zval_get_string(zv));
	}
	else {
		result = Z_STR_P(zv);
	}
	return result;
}

zend_string*  
zval_user::zstr() const
{	
	if (!p_)
		return nullptr;
	zval* zv = zval_user::real_zval(p_);
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
	zval* zv = zval_user::real_zval(p_);
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
	zval* zv = zval_user::real_zval(p_);
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
	zval* zv = zval_user::real_zval(p_);
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
	zval* zv = zval_user::real_zval(p_);
	switch(Z_TYPE_P(zv)) {
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
	zval* zv = zval_user::real_zval(p_);

	switch(Z_TYPE_P(zv))
	{
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
	zval* zv = zval_user::real_zval(p_);

	if (Z_TYPE_P(zv) != IS_ARRAY) {
		return nullptr;
	}

	return Z_ARRVAL_P(zv);
}

void 
zval_user::return_zv(zval* ret)
{
	ZVAL_COPY(ret, p_);
}

const zval_user& 
zval_user::operator=(const zval_mgr& rc)
{
	p_ = rc;
	return *this;
}

void // protected
zval_user::bind_string(zend_string* s)
{
    if (s) 
    {
    	ZVAL_STR(p_, s);
    	if (!(GC_FLAGS(s) & IS_STR_INTERNED))
    	{
    		GC_ADDREF(s);
	    }
	    else {
	    	Z_TYPE_FLAGS_P(p_) = 0; 
	    }
    }
    else {
        ZVAL_NULL(p_);
    }
}

void // protected
zval_user::bind_long(zend_long value)
{
    ZVAL_LONG(p_, value);
}

void // protected
zval_user::bind_object(zend_object* obj)
{
    if (obj)
    {
        ZVAL_OBJ_COPY(p_, obj);
    }
    else {
        ZVAL_NULL(p_);
    }
}

void 
zval_user::bind_array(HashTable* ht)
{
    if (ht) {
        ZVAL_ARR(p_, ht);
        if ( !(ht->gc.u.type_info & GC_IMMUTABLE)){
            ht->gc.refcount++;
        }
        else { // clear 
        	Z_TYPE_FLAGS_P(p_) = 0; 
        }
    }
    else {
        ZVAL_NULL(p_);
    }
}

void zval_user::setbool(bool value)
{
    *p_ = {0};
    ZVAL_BOOL(p_, value);
}

int 
zval_user::refcount() const
{
	if (p_ && Z_REFCOUNTED_P(p_))
		return Z_REFCOUNT_P(p_);
	else
		return 0;
}

zval_user 
zval_user::php_constant(zstr_user name)
{
	return zval_user(zend_get_constant(name));
}

}; //namespace
//zval_user.cpp
#endif