 /*  
  *  @file val_rc.cpp
  *  @brief val_rc class, reference counted zval value.
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  * @license BSD 3-Clause License
  */

#ifndef VAL_RC_CPP
#define VAL_RC_CPP

#ifndef VAL_RC_H
#include "val_rc.h"
#endif

#ifndef VAL_PTR_H
#include "val_ptr.h"
#endif

#ifndef STR_RC_H
#include "str_rc.h"
#endif

#ifndef HTAB_RC_H
#include "htab_rc.h"
#endif

#ifndef OBJ_RC_H
#include "obj_rc.h"
#endif

namespace zpp {

val_rc val_rc::EmptyArray = val_rc((HashTable*) &zend_empty_array);
val_rc val_rc::NullValue = val_rc();

 
const val_ptr //static
val_rc::null_value_ptr(){
    return val_ptr(val_rc::NullValue);
}

const val_ptr
val_rc::empty_array_ptr()
{
    return val_ptr(val_rc::EmptyArray);
}
/** This init does not de-reference anything, just wipes */
void 
val_rc::init()
{
    zv_ = {};
    ZVAL_NULL(&zv_);
}


int   
val_rc::ref_type() const
{
    int result = zv_.u1.v.type;
    switch(result) 
    {
        case IS_REFERENCE:
            result = val_ptr::php_type(zv_.value.ref->val);
            break;
        case IS_INDIRECT:
            result = val_ptr::php_type(*zv_.value.zv);
            break;
    }
    return result;
}

/** Get zval contained in reference zval */
val_ptr 
val_rc::dereference() const
{
    return val_ptr((zval*)&zv_).referent();
}

void 
val_rc::make_ref()
{
    zval *zp = &zv_;
    if (Z_TYPE_P(zp) != IS_REFERENCE) 
    { 
        /*
        zend_reference *ref = (zend_reference *) emalloc(sizeof(zend_reference));
        ref->gc.refcount = 1;                               
        GC_TYPE_INFO(ref) = GC_REFERENCE;                     
        ZVAL_COPY_VALUE(&ref->val, zp);                        
        ref->sources.ptr = NULL;                                  
        Z_REF_P(zp) = ref;                                    
        Z_TYPE_INFO_P(zp) = IS_REFERENCE_EX; 
        */
        ZVAL_NEW_REF(&zv_, zp);  
        //showmem("make_ref", &zv_); 
    }                      
}

val_rc::~val_rc()
{
    val_ptr::try_decref(&zv_);
}


zend_string* 
val_rc::zstr() const
{
    return val_ptr((zval*)&zv_).zstr();
}


zend_long 
val_rc::get_long() const
{
    return val_ptr((zval*)&zv_).get_long();
}

HashTable*   
val_rc::zarray() const
{
    return val_ptr((zval*)&zv_).zarray();
}

zend_object*    
val_rc::zobject() const
{
    return val_ptr((zval*)&zv_).zobject();
}

void 
val_rc::new_array()
{
    lose();
    // static gives ht.rc == 1
    HashTable* ht = htab_rc::new_array();
    // bind with rc == 1 
    val_ptr::array_bind(&zv_, ht);  
}
bool
val_rc::isEmpty() const
{
    return val_ptr((zval*)&zv_).isEmpty();
}

void 
val_rc::empty_array()
{
    lose();
    // zend_empty_array has rc == 2 
    val_ptr(&zv_).bind_array((zend_array*) &zend_empty_array);
}


void
val_rc::addref()
{
    val_ptr::try_addref(&zv_);
}

bool 
val_rc::get_bool() const
{
    return !(val_ptr(*this).get_bool());
}

void // protected
val_rc::lose()
{
    val_ptr::try_decref(&zv_);
    zv_ = {};
    //ZVAL_NULL(&zv_);
}

const val_rc& 
val_rc::operator=(zend_long value)
{
    val_ptr::try_decref(&zv_);
    zv_ = {};
    ZVAL_LONG(&zv_, value);
    return *this;
}

const val_rc& 
val_rc::operator=(const char* s)
{
    val_ptr::try_decref(&zv_);
    zv_ = {};
    ZVAL_STRING(&zv_, s);
    return *this;
}

const val_rc& 
val_rc::operator=(double value)
{
    val_ptr::try_decref(&zv_);
    zv_ = {};
    ZVAL_DOUBLE(&zv_, value);
    return *this;
}

val_rc::val_rc() 
{
    zv_ = {};
    ZVAL_NULL(&zv_);
}

void val_rc::set_null()
{
    val_ptr::try_decref(&zv_);
    zv_ = {};
    ZVAL_NULL(&zv_);
}

void val_rc::set_bool(bool value)
{
    val_ptr::try_decref(&zv_);
    zv_ = {};
    ZVAL_BOOL(&zv_, value);
}

val_rc::val_rc(HashTable* ht)
{
     zv_ = {};
     val_ptr(&zv_).bind_array(ht);
}

val_rc::val_rc(const char* s)
{
    zv_ = {};
    ZVAL_STRING(&zv_, s);
}

#ifndef OMIT_BASE_D
val_rc::val_rc(base_d* cobj)
{
    zv_ = {};
    
    val_ptr(&zv_).bind_object(cobj->vobj());
}
#endif

val_rc::val_rc(double value)
{
    zv_ = {};
    ZVAL_DOUBLE(&zv_, value);
}

val_rc::val_rc(bool bval)
{
    zv_ = {};
    if (bval)
    {
        ZVAL_TRUE(&zv_);
    }
    else {
        ZVAL_FALSE(&zv_);
    } 
}

val_rc::val_rc(zval* zv)
{
    zv_ = {};
    if (zv) {
        // destructor will try to decref.
        ZVAL_COPY(&zv_, zv);
    }
    else {
        ZVAL_NULL(&zv_);
    }
}

val_rc::val_rc(const val_ptr& rc)
{
    zv_ = {};
    if (rc.p_) {
        ZVAL_COPY(&zv_, rc.p_);
    }
    else {
        ZVAL_NULL(&zv_);
    }    
}

const val_rc& 
val_rc::operator=(zval* rc)
{
    lose();
    if (rc) {
         ZVAL_COPY(&zv_, rc);
    }
    else {
        ZVAL_NULL(&zv_);
    }
    return *this;
}

const val_rc& 
val_rc::operator=(const htab_rc &rc)
{
    lose();
    HashTable* ht = rc.ht_;
    if (ht)
    {
        val_ptr(&zv_).bind_array(ht);
    }
    return *this;
}

 val_rc::val_rc(int value)
 {
    zv_ = {};
    ZVAL_LONG(&zv_, value);
 }

val_rc::val_rc(const val_rc& rc, bool byRef) 
{

    zv_ = {};
    zval *p = (zval *) rc;

    //  Does addref count
    ZVAL_COPY(&zv_, p);
    
    if (byRef)
    {
        ZVAL_MAKE_REF(&zv_);
    }
}

val_rc::val_rc(val_rc&& m)
{
    ZVAL_COPY_VALUE(&zv_, &m.zv_);
    m.init();
}

// copy and already incremented rc value,
// like a move. Argument is wiped
void val_rc::adopt(zval* from)
{
    lose();
    ZVAL_COPY_VALUE(&zv_, from);
    *from = {};
}

/** copy with careful addref */
void
val_rc::copy(zval *p)
{
    ZVAL_COPY(&zv_, p);
    //try_addref(&zv_);
}
void 
val_rc::assign_ptr(zval* p)
{
	//avoid self assign and lose
	if (p != &zv_)
	{
		lose();
	}
	if (!p) {
        init();
		return;
	}

	// ?? danger of losing with self assign??
	if (p != &zv_)
	{
		// copy add reference count;
		ZVAL_COPY(&zv_, p);
	}

}

bool 
val_rc::ok() const
{
    return val_ptr(*this).ok();
}

const val_rc& 
val_rc::operator=(const val_ptr &rc)
{
	zval* p = (zval*) rc;

	assign_ptr(p);

	return *this;		
}

const val_rc& 
val_rc::operator=(const str_ptr& rc)
{
    lose();
    val_ptr(&zv_).bind_string(rc.s);
    return *this;
}

val_rc& 
val_rc::operator=(val_rc&& rc)
{
	if (&rc != this)
	{
		lose();
        ZVAL_COPY_VALUE(&zv_, &rc.zv_);
		rc.init();
	}
    return *this;
}

const val_rc& 
val_rc::operator=(const obj_ptr &rc)
{
    lose();
    zend_object* obj = (zend_object*) rc;
    if (obj)
    {
        ZVAL_OBJ_COPY(&zv_, obj);
    }
    return *this;
}

const val_rc& 
val_rc::operator=(const val_rc &rc)
{
    val_ptr::try_decref(&zv_); 
    ZVAL_COPY(&zv_ , &rc.zv_);
    return *this;
}

void 
val_rc::move_zv(zval* return_value)
{
    ZVAL_COPY_VALUE(return_value, &zv_);
    zv_ = {};
}

void 
val_rc::copy_zv(zval* return_value)
{
    ZVAL_COPY(return_value, &zv_);
}


//! mutate value
void 
val_rc::toLong()
{
    zval* p = &zv_;
    
    if (Z_TYPE_P(p) != IS_LONG) {
        ZVAL_DEREF(p);
        zend_long value =  zval_get_long_ex(p,false);
        lose();
        ZVAL_LONG(&zv_,value);
    }
}

void val_rc::toDouble()
{
    zval* p = &zv_;
    if (Z_TYPE_P(p) != IS_DOUBLE) {
        ZVAL_DEREF(p);
        double value =  zval_get_double_func(p);
        lose();
        ZVAL_DOUBLE(&zv_,value);
    }
}

void
val_rc::toString() 
{
    zval* p = (zval*)  &zv_;
    ZVAL_DEREF(p);
    if (Z_TYPE_P(p) != IS_STRING)
    {
        zend_string* s = zval_get_string(p);
        lose();
        ZVAL_STR(&zv_, s);
    }
}

/*! 
 * Make any  value first value of an array.
 * */
void
val_rc::toArray()
{
    val_rc temp(std::move(*this));

    HashTable* ht = htab_rc::new_array();
    htab_cow wrap(ht);
    wrap.push_back(temp);
    // Already rc==1
    val_ptr::array_bind(&zv_, ht); 
}

val_rc::val_rc(str_rc&& rc)
{
    if (rc.s)
    {
        ZVAL_STR(&zv_, rc.s);
        rc.s = nullptr;
    }
}

val_rc::val_rc(zend_string* rc)
{
    zv_ = {};
    val_ptr(&zv_).bind_string(rc);
}

val_rc::val_rc(zend_object* rc)
{
    zv_ = {};
    val_ptr(&zv_).bind_object(rc);
}

val_rc::val_rc(zend_long value)
{
    zv_ = {};
    ZVAL_LONG(&zv_, value);
}

val_rc::val_rc(const str_ptr& rc)
{
    zv_ = {};
    val_ptr(&zv_).bind_string(rc);
}

const val_rc& 
val_rc::operator=(zend_object* rc)
{
    lose();
    val_ptr(&zv_).bind_object(rc);
    return *this;
}

const val_rc& 
val_rc::operator=(HashTable* rc)
{
    lose();
    val_ptr(&zv_).bind_array(rc);
    return *this;
}

const val_rc& 
val_rc::operator=(zend_string* rc)
{
    lose();
    val_ptr(&zv_).bind_string(rc);
    return *this;
}


void val_rc::decref()
{
    val_ptr::try_decref(&zv_);
}


val_rc //static 
val_rc::empty_str()
{
    return val_rc(zend_empty_string);
}


    
}; // namespace Php

#endif
//val_rc.cpp