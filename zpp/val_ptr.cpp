 /*  
  *  PHP extension C++ classes - zpp 
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  */

#ifndef VAL_PTR_CPP
#define VAL_PTR_CPP

#ifndef STR_RC_H
#include "str_rc.h"
#endif

#ifndef VAL_PTR_H
#include "val_ptr.h"
#endif

#ifndef HTAB_RD_H
#include "htab_rd.h"
#endif

#ifndef VAL_RC_H
#include "val_rc.h"
#endif

namespace zpp {
	
bool 
val_ptr::same(const val_ptr& test) const
{
	zval* a = p_;
	zval* b = test.p_;

    if (a == b)
    {
        return true;
    }
    if (!a || !b) {
        return false;
    }
    int mytype = Z_TYPE_P(a);

    if (mytype != Z_TYPE_P(b))
    {
        return false;
    }
    
    switch(mytype) {
    case IS_STRING:
        return (zs_cmp(Z_STR_P(a), Z_STR_P(b))==0);
    default:
        return (a->value.lval == b->value.lval);
    }
    
}

zval* //static 
val_ptr::real_zval(const zval* zv)
{
    switch(Z_TYPE_P(zv)) {
        case IS_REFERENCE:
            zv = Z_REFVAL_P(zv);
            break;
        case IS_INDIRECT:
            zv = zv->value.zv; 
            break;  
    }
    return (zval*) zv;
}

val_ptr val_ptr::referent()
{
	if (p_) {
		zval* x = p_;
		ZVAL_DEREF(x);
		return val_ptr(x);
	}
	return val_ptr(p_);	
}


int  
val_ptr::ref_type() const
{
    if (!p_) {
        return IS_NULL;
    }
    return Z_TYPE_P(val_ptr::real_zval(p_));
}


bool 
val_ptr::isDouble() const
{
    return (p_ && (ref_type() == IS_DOUBLE));
}

bool 
val_ptr::isLong() const
{
    return (p_ && (ref_type() == IS_LONG));
}

bool 
val_ptr::isArray() const
{
    return (p_ && (ref_type() == IS_ARRAY));
}

 bool val_ptr::isResource() const
 {
 	return (p_ && (ref_type() == IS_RESOURCE));
 }

bool 
val_ptr::isNull() const
{
    return (!(p_) || (ref_type() == IS_NULL));
}

bool 
val_ptr::isObject() const
{ 
    return (p_ && (ref_type() == IS_OBJECT));
}



bool 
val_ptr::isString() const
{
    return (p_ && (ref_type() == IS_STRING));
}

bool 
val_ptr::isTrue() const 
{
    return (p_ && (ref_type() == IS_TRUE));
}

bool 
val_ptr::isFalse() const 
{
    return ( !(p_) || (ref_type() == IS_FALSE));
}

bool 
val_ptr::isPointer() const
{
    return (p_ && (ref_type() == IS_PTR));
}

val_ptr::val_ptr(const zval* rc)
{
    p_ = (zval*) rc;
    //showmem("val_ptr:: ", (zval*) rc);
}   

bool 
val_ptr::empty() const 
{
	if (!p_)
		return true;
	zval* rp = val_ptr::real_zval(p_);

	int rtype = Z_TYPE_P(rp);

	switch (rtype)
	{
	case IS_FALSE:
		return true;
	case IS_LONG:
    	return (Z_LVAL_P(rp) == 0) ? true : false;
    case IS_DOUBLE:
    	return (Z_DVAL_P(rp) == 0.0) ? true : false;
	case IS_ARRAY:
    	return htab_rd(Z_ARR_P(rp)).size() ? false : true;
    case IS_STRING:
    	return str_ptr(Z_STR_P(rp)).size() ? false : true;
	default:
    	return false;
	}
}

bool 
val_ptr::ok() const {
	if (!p_)
		return false;
	zval* rp = val_ptr::real_zval(p_);
	int rtype = Z_TYPE_P(rp);
    if (rtype < IS_TRUE)
    {
    	return false;
    }
    switch(rtype)
    {
    case IS_ARRAY:
    	return htab_rd(Z_ARR_P(rp)).size() ? true : false;
    case IS_STRING:
    	return str_ptr(Z_STR_P(rp)).size() ? true : false;
    }
    // Don't care about zero values of LONG or DOUBLE
    return true;
}

zend_string* 
val_ptr::className() const
{
	if (!p_)
		return nullptr;

	zval* zv = val_ptr::real_zval(p_);

	if (Z_TYPE_P(zv) == IS_OBJECT)
	{
		zend_class_entry *ce = Z_OBJCE_P(zv);
		return ce->name;	
	}
	//TODO: Throw exception?
	return nullptr;
}

zend_object* 
val_ptr::zobject() const
{
	if (!p_) {
		return nullptr;
	}
	zval* zv = val_ptr::real_zval(p_);

	if (Z_TYPE_P(zv) != IS_OBJECT) {
		return nullptr;
	}
	return Z_OBJ_P(zv);
}

size_t val_ptr::size() const
{
	if (!p_) 
	{
		return 0;
	}
	zval* zv = val_ptr::real_zval(p_);
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

val_ptr::val_ptr(const val_rc& mgr) : p_((zval*) mgr)
{
}

/*
* Return managed string
*/
str_rc
val_ptr::to_zstr() const 
{
	str_rc result;

	if (!p_)
	{
		return result;
	}

	zval* zv = val_ptr::real_zval(p_);

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
val_ptr::zstr() const
{	
	if (!p_)
		return nullptr;
	zval* zv = val_ptr::real_zval(p_);
	if (Z_TYPE_P(zv) != IS_STRING)
	{
		return nullptr;
	}
	return Z_STR_P(zv);
}

std::string 
val_ptr::cstr() const
{
	str_rc temp = to_zstr();
	return str_ptr(temp).cstr();	
}

std::string_view 
val_ptr::vstr() const
{
	str_ptr temp(this->zstr());
	return temp.vstr();
}

bool 
val_ptr::isCallable()  const
{
	if (!p_)
		return false;
	zval* zv = val_ptr::real_zval(p_);
	return (zend_is_callable(zv, 0, nullptr));
}

bool 
val_ptr::getStringData(zend_string** retstr) const 
{
	zend_string* s = this->zstr();

	if (retstr)
	{
		*retstr = s;
	}
	return ( (s) &&  (ZSTR_LEN(s) > 0));
}

double 
val_ptr::zdouble() const
{
	if (!p_) {
		return 0.0; 
	}
	zval* zv = val_ptr::real_zval(p_);
	if (Z_TYPE_P(zv) != IS_DOUBLE) {
		return zval_get_double_func(zv);
	}
	return Z_DVAL_P(zv);
}

void* 
val_ptr::voidptr() const 
{
	if (!p_) 
	{
		return nullptr; 
	}
	zval* zv = val_ptr::real_zval(p_);
	if ( Z_TYPE_P(zv) != IS_PTR) {
		return nullptr;
	}
	return Z_PTR_P(zv);
}

bool
val_ptr::zbool() const {
	if (!p_)
	{
		return false;
	}
	zval* zv = val_ptr::real_zval(p_);
	switch(Z_TYPE_P(zv)) {
		case IS_TRUE: return true;
		case IS_FALSE: return false;
		default: {
			// become ridiculous
			val_rc temp(zv);
			convert_to_boolean(temp);
			return val_ptr(temp).zbool();
		}
	}
}

zend_long 
val_ptr::zlong() const
{
	if (!p_)
	{
		return 0;
	}
	zval* zv = val_ptr::real_zval(p_);

	switch(Z_TYPE_P(zv))
	{
		case IS_LONG: return Z_LVAL_P(zv);
		case IS_TRUE: return 1;
		case IS_FALSE: return 0;
		default: return zval_get_long_func(zv, false);
	}
}

HashTable* 
val_ptr::zarray() const
{
	if (!p_)
		return nullptr;
	zval* zv = val_ptr::real_zval(p_);

	if (Z_TYPE_P(zv) != IS_ARRAY) {
		return nullptr;
	}
	return Z_ARRVAL_P(zv);
}

void 
val_ptr::return_zv(zval* ret)
{
	if (p_)
	{
		ZVAL_COPY(ret, p_);
	}
	else {
		ZVAL_NULL(ret);
	}
}

const val_ptr& 
val_ptr::operator=(const val_rc& rc)
{
	p_ = rc;
	return *this;
}

void // protected
val_ptr::bind_string(zend_string* s)
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
val_ptr::bind_long(zend_long value)
{
    ZVAL_LONG(p_, value);
}

void // protected
val_ptr::bind_object(zend_object* obj)
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
val_ptr::bind_array(HashTable* ht)
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

void val_ptr::setbool(bool value)
{
    *p_ = {0};
    ZVAL_BOOL(p_, value);
}

int 
val_ptr::refcount() const
{
	if (p_ && Z_REFCOUNTED_P(p_))
		return Z_REFCOUNT_P(p_);
	else
		return 0;
}

val_ptr 
val_ptr::php_constant(str_ptr name)
{
	return val_ptr(zend_get_constant(name));
}


// bind and set zval flags for reference counting this array
void // static 
val_ptr::array_bind(zval* tmp, HashTable* t)
{
	if (!t)
	{
		ZVAL_NULL(tmp);
	}
	else
	{	
		
		ZVAL_ARR(tmp, t);
		if (GC_FLAGS(t) & GC_IMMUTABLE)
		{
			Z_TYPE_FLAGS_P(tmp) = 0; // mark as not reference counted
		}
	}
}

void
val_ptr::object_bind(zval* temp, zend_object* obj)
{
	if (!obj)
	{
		ZVAL_NULL(temp);
	}
	else
	{	
		ZVAL_OBJ(temp, obj);
		if (GC_FLAGS(obj) & GC_IMMUTABLE)
		{
			Z_TYPE_FLAGS_P(temp) = 0; // mark as not reference counted
		}
	}
}


void // static 
val_ptr::string_bind(zval* tmp, zend_string* s)
{
	if (s) 
    {
    	ZVAL_STR(tmp, s);
    	if ((GC_FLAGS(s) & IS_STR_INTERNED))
    	{
    		Z_TYPE_FLAGS_P(tmp) = 0; 
	    }
    }
    else {
        ZVAL_NULL(tmp);
    }
}


}; //namespace
//val_ptr.cpp
#endif