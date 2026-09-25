 /*  
  *  @file val_ptr.cpp
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2025 Michael Rynn
  *  @brief val_ptr class, holds a zval pointer.	
  *  @license BSD 3-Clause License
  */

#ifndef VAL_PTR_CPP
#define VAL_PTR_CPP

#ifndef STR_RC_H
#include "str_rc.h"
#endif

#ifndef VAL_PTR_H
#include "val_ptr.h"
#endif

#ifndef HTAB_PTR_H
#include "htab_ptr.h"
#endif

#ifndef VAL_RC_H
#include "val_rc.h"
#endif

namespace zpp {
	
const val_rc val_null;

val_ptr val_ptr::nullval()
{
	return val_ptr(val_null);
}

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


void //static
val_ptr::try_addref(zval* p)
{
    HashTable*      ht;
    zend_object*    ob;

    if (Z_REFCOUNTED_P(p))
    {
        auto ztype = Z_TYPE_P(p);
        switch(ztype) {
        case IS_STRING:
            {
                zend_string* s = Z_STR_P(p);;
                if (GC_FLAGS(s) & IS_STR_INTERNED)
                {
                    break;
                }
                s->gc.refcount++;
            }
            break;
        case IS_REFERENCE:
            {   
                zend_reference*   zref = p->value.ref;
                zref->gc.refcount++;
                
            }
            break;
            
        case IS_ARRAY:
            {
                ht = Z_ARR_P(p);
                if (ht->gc.u.type_info & GC_IMMUTABLE)
                {
                    break;
                }
                ht->gc.refcount++;
            }
            break;
        case IS_OBJECT:
            {
                ob = Z_OBJ_P(p);
                ob->gc.refcount++;
            }
            break;
        default:
            {
                GC_ADDREF(p->value.counted);
            }
            break;
        }
    }
}
void //static.
val_ptr::try_decref(zval* p)
{
    if (Z_REFCOUNTED_P(p))
    {
        auto rct = zval_refcount_p(p);
        if (rct <= 0)
        {
            //showmem("!!! RC emergency", p );
            *p = {};
            return;
        }
        //auto ztype = Z_TYPE_P(p);
        switch(Z_TYPE_P(p)) 
        {
        case IS_STRING:
            {
                zend_string* s = Z_STR_P(p);
                //showstr("val_rc decref", s);
                zend_string_release(s);
            }
            break;
        case IS_REFERENCE:
            {
                auto zref = Z_REF_P(p);
                //showmem("reference", p);
                if (rct == 1) 
                {

                    val_ptr::try_decref(&zref->val);
                    efree_size(zref, sizeof(zend_reference));
                    //showmem("reference", p);
                }
                zref->gc.refcount--;
            }
            break;
        case IS_ARRAY:
            {

                HashTable* ht = Z_ARR_P(p);
                htab_ptr::try_decref(ht);
            }
            break;

        case IS_OBJECT:
            {
                obj_ptr::try_decref(Z_OBJ_P(p));
            }
            break;
        default:
            return;
        }
        *p = {};
    }
}


zval* //static 
val_ptr::real_zval(zval* zv)
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

val_ptr 
val_ptr::refto() const
{
	return val_ptr(&p_->value.ref->val);
}

// If already confirmed  ztype IS_DIRECT
val_ptr 
val_ptr::indto() const
{
	return val_ptr(p_->value.zv);
}	


val_ptr 
val_ptr::referent()
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
	int result = ztype();
	switch(result)
	{
	case IS_REFERENCE:
		return refto().ztype();
	case IS_INDIRECT:
		return indto().ztype();
	default:
		return result;
	}
}


bool 
val_ptr::isDouble() const
{
    switch(ztype())
	{
	case IS_DOUBLE: 
		return true;
	case IS_UNDEF:
		return false;
	case IS_REFERENCE:
		return (refto().isDouble());
	case IS_INDIRECT:
		return (indto().isDouble());
	default:
		return false;
	}
}

bool 
val_ptr::isLong() const
{
    switch(ztype())
	{
	case IS_LONG: 
		return true;
	case IS_UNDEF:
		return false;
	case IS_REFERENCE:
		return (refto().isLong());
	case IS_INDIRECT:
		return (indto().isLong());
	default:
		return false;
	}
}

bool 
val_ptr::isArray() const
{
	switch(ztype())
	{
	case IS_ARRAY: 
		return true;
	case IS_UNDEF:
		return false;
	case IS_REFERENCE:
		return (refto().isArray());
	case IS_INDIRECT:
		return (indto().isArray());
	default:
		return false;
	}

}

 bool val_ptr::isResource() const
 {
 	return (p_ && (ref_type() == IS_RESOURCE));
 }

bool 
val_ptr::isNull() const
{
	switch(ztype())
	{
	case IS_UNDEF:
	case IS_NULL:
		return true;
	case IS_REFERENCE:
		return refto().isNull();
	case IS_INDIRECT:
		return indto().isNull();
	default:
		return false;
	}
}

bool 
val_ptr::isObject() const
{ 
    switch(ztype())
	{
	case IS_OBJECT: 
		return true;
	case IS_UNDEF:
		return false;
	case IS_REFERENCE:
		return (refto().isObject());
	case IS_INDIRECT:
		return (indto().isObject());
	default:
		return false;
	}
}

bool 
val_ptr::isString() const
{
	switch(ztype())
	{
	case IS_STRING: 
		return true;
	case IS_UNDEF:
		return false;
	case IS_REFERENCE:
		return (refto().isString());
	case IS_INDIRECT:
		return (indto().isString());
	default:
		return false;
	}
}


bool 
val_ptr::isReference() const {
	return (p_ && (Z_TYPE_P(p_) == IS_REFERENCE));
}

bool 
val_ptr::isBoolean() const
{

	if (!p_) {
		return false;
	}
	auto ztype = ref_type();
	return (ztype == IS_TRUE) || (ztype == IS_FALSE);
}

bool 
val_ptr::isTrue() const 
{
    switch(ztype())
    {
    case IS_TRUE:
    	return true;
    case IS_UNDEF:
    case IS_NULL:
    case IS_FALSE:
    	return false;
    case IS_REFERENCE:
    	return (refto().isTrue());
    case IS_INDIRECT:
    	return (indto().isTrue());
    default:
    	return false;
    }
}

bool 
val_ptr::isFalse() const 
{
    switch(ztype())
    {
    case IS_FALSE:
    case IS_NULL:
    case IS_UNDEF:
    	return true;
    case IS_TRUE:
    	return false;
    case IS_REFERENCE:
    	return (refto().isFalse());
    case IS_INDIRECT:
    	return (indto().isFalse());
    default:
    	return false;
    }
}

bool 
val_ptr::isPointer() const
{
    return (p_ && (ref_type() == IS_PTR));
}


bool 
val_ptr::isEmpty() const 
{
	switch (ztype())
	{
	case IS_UNDEF:
		return true;
	case IS_NULL:
	case IS_FALSE:
		return true;
	case IS_LONG:
    	return (Z_LVAL_P(p_) == 0) ? true : false;
    case IS_DOUBLE:
    	return (Z_DVAL_P(p_) == 0.0) ? true : false;
	case IS_ARRAY:
    	return htab_ptr(Z_ARR_P(p_)).size() ? false : true;
    case IS_STRING:
    	{
    	str_ptr test(Z_STR_P(p_));
    	switch(test.size()) {
    	case 0:
    		return true;
    	case 1:
    		if (test.data()[0]=='0') {
    			return true;
    		}
    		// fall through
    	default:
    		return false;
    	};
    	}
    case IS_REFERENCE:
    	return (refto().isEmpty());
    case IS_INDIRECT:
    	return (indto().isEmpty());
	default:
    	return false;
	}
}

bool 
val_ptr::ok() const {
	switch(ztype())
	{
	case IS_UNDEF:
	case IS_NULL:
	case IS_FALSE:
		return false;
	// empty array or string , don't look futher
	case IS_ARRAY:
    	return htab_ptr(Z_ARR_P(p_)).size() ? true : false;
    case IS_STRING:
    	return str_ptr(Z_STR_P(p_)).size() ? true : false;
    case IS_REFERENCE:
    	return (refto().ok());
    case IS_INDIRECT:
    	return (indto().ok());
	default:
	// zero numeric is ok
    	return true;
	}
}

zend_string* 
val_ptr::className() const
{
	switch(ztype())
	{
	case IS_OBJECT:
		{
			zend_class_entry *ce = Z_OBJCE_P(p_);
			return ce->name;
		}
	case IS_REFERENCE:
		return refto().className();
	case IS_INDIRECT:	
		return indto().className();
	default:
		return nullptr;
		//TODO: Throw exception?
	}
}

zend_object* 
val_ptr::zobject() const
{
	switch(ztype())
	{
	case IS_OBJECT:
		return Z_OBJ_P(p_);
	case IS_REFERENCE:
		return refto().zobject();
	case IS_INDIRECT:
		return indto().zobject();
	default:
		return nullptr;
	}
	if (!p_) {
		return nullptr;
	}
}

size_t 
val_ptr::size() const
{
	switch(ztype())
	{
	case IS_ARRAY:
		return zend_array_count(Z_ARRVAL_P(p_));
	case IS_STRING:
		return ZSTR_LEN(Z_STR_P(p_));
	case IS_REFERENCE:
		return refto().size();
	case IS_INDIRECT:
		return indto().size();
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
	zend_string* s = zstr();
	if (!s)
	{
		s = zval_get_string(p_);
		if (s)
		{
			result.adopt(s);
		}
	}
	else {
		result = s;
	}
	return result;
}

zend_string*  
val_ptr::zstr() const
{	
	zend_string* result;
	switch(ztype())
	{
	case IS_STRING:
		result = Z_STR_P(p_);
		break;
	case IS_REFERENCE:
		result = refto().zstr();
		break;
	case IS_INDIRECT:
		result = indto().zstr();
		break;
	default:
		result = (zend_string*) nullptr;
		break;
	}
	return result;
}

rqstring
val_ptr::cstr() const
{
	str_rc temp = to_zstr();
	return rqstring(temp.data(), temp.size());	
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
	switch(ztype())
	{
	case IS_STRING:
	case IS_ARRAY:
	case IS_OBJECT:
		return (zend_is_callable(p_, 0, nullptr));
	case IS_REFERENCE:
		return refto().isCallable();
	default:
		return false;
	}
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
val_ptr::get_double() const
{
	if (!p_) {
		return 0.0; 
	}
	switch(ztype())
	{
	case IS_DOUBLE:
		return Z_DVAL_P(p_);
	case IS_REFERENCE:
		return refto().get_double();
	default:
		return zval_get_double_func(p_);
	}
}

void* 
val_ptr::voidptr() const 
{
	switch(ztype())
	{
	case IS_PTR:
		return Z_PTR_P(p_);
	case IS_REFERENCE:
		return refto().voidptr();
	case IS_INDIRECT:
		return indto().voidptr();
	default:
		return nullptr;
	}
}

bool
val_ptr::get_bool() const 
{
	switch(ztype())
	{

		case IS_TRUE: return true;
		case IS_FALSE: 
		case IS_UNDEF:
			return false;	
		case IS_REFERENCE:
			return refto().get_bool();
		default:  
		{
			//  ridiculous
			val_rc temp(p_);
			convert_to_boolean(temp);
			return val_ptr(temp).get_bool();
		}
	}
}

zend_long 
val_ptr::get_long() const
{
	switch(ztype())
	{
	case IS_LONG:
		return Z_LVAL_P(p_);
	case IS_TRUE:
		return 1;
	case IS_FALSE:
	case IS_NULL:
	case IS_UNDEF:
		return 0;
	case IS_REFERENCE:
		return refto().get_long();
	case IS_INDIRECT:
		return indto().get_long();
	default:
		return zval_get_long_func(p_, false);
	}
}

HashTable* 
val_ptr::zarray() const
{
	switch(ztype())
	{
	case IS_ARRAY:
		return Z_ARRVAL_P(p_);
	case IS_REFERENCE:
		return refto().zarray();
	case IS_INDIRECT:
		return indto().zarray();
	default:
		return nullptr;
	}

	
}

void 
val_ptr::copy_zv(zval* ret) const
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

void // protected, can bump reference count
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
        ZVAL_OBJ(p_, obj);
        if (! (GC_FLAGS(obj) & GC_IMMUTABLE))
        {
        	GC_ADDREF(obj);
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
val_ptr::bind_array(HashTable* ht)
{
	if (val_ptr::array_bind(p_, ht))
	{
		GC_ADDREF(ht);
	}
}

void val_ptr::setbool(bool value)
{
    *p_ = {};
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

zval* 
val_ptr::php_constant(str_ptr name)
{
	return zend_get_constant(name);
}
/*
void  
val_ptr::set_zlong(zend_long val)
{
	val_ptr::try_decref(p_);
	ZVAL_LONG(p_, val);
}

void  
val_ptr::set_zstr(str_ptr val)
{
	val_ptr::try_decref(p_);
	if (val_ptr::string_bind(p_, val))
	{
		GC_ADDREF((zend_string*)val);
	};
}

void  
val_ptr::set_htab(htab_ptr val)
{
	val_ptr::try_decref(p_);
	if (val_ptr::array_bind(p_, val))
	{
		GC_ADDREF((HashTable*)val);
	}
}

void  
val_ptr::set_zobj(obj_ptr val)
{
	val_ptr::try_decref(p_);
	if (val_ptr::object_bind(p_, val))
	{
		GC_ADDREF((zend_object*)val);
	}
}

void
val_ptr::make_ref()
{
	if (p_)
	{
		int rtype = Z_TYPE_P(p_);
		switch(rtype)
		{
		case IS_STRING:
		case IS_ARRAY:
		case IS_OBJECT:
		case IS_LONG:
	    case IS_DOUBLE:
			ZVAL_NEW_REF(p_, p_);
			break;
		default:
			//TODO: throw error?
			break;
		}
	}
}
*/

}; //namespace
//val_ptr.cpp
#endif