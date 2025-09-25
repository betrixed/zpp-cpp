 /*  
  *  @file val_rc.cpp
  *  @brief val_rc class, reference counted zval value.
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  * @license Artistic License 2.0
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

void 
val_rc::init()
{
    zv_ = {0};
    ZVAL_NULL(&zv_);
}

int   
val_rc::ref_type() const
{
    return Z_TYPE_P((const zval*) val_ptr::real_zval(&zv_));
}

void 
val_rc::make_ref()
{
    zval *zp = &zv_;
    if (!Z_ISREF_P(zp)) { 
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
    }                       
}

val_rc::~val_rc()
{
    val_rc::try_decref(&zv_);
}


zend_string* 
val_rc::zstr() const
{
    zval* p = val_ptr::real_zval(&zv_);
    if (Z_TYPE_P(p) != IS_STRING)
    {
        return nullptr;
    }
    return Z_STR_P(p);
}

zend_long 
val_rc::zlong() const
{
    val_ptr result(*this);
    return result.zlong();
}

HashTable*   
val_rc::zarray() const
{
    zval* p = val_ptr::real_zval(&zv_);
    if (Z_TYPE_P(p) != IS_ARRAY)
    {
        return nullptr;
    }
    return Z_ARR_P(p);
}

zend_object*    
val_rc::zobject() const
{
    zval* p = val_ptr::real_zval(&zv_);
    zend_object* result;

    if (Z_TYPE_P(p) == IS_OBJECT)
    {
        result = Z_OBJ(zv_);
    }
    else {
        result = nullptr;
    }

    return result;
}

void 
val_rc::new_array()
{
    lose();
    HashTable* ht = htab_rc::new_array();
    // added with rc == 1 
    val_ptr(&zv_).bind_array(ht);  
    htab_rc::try_decref(ht); // because new primary mgr
    //showmem("new_array", &zv_);
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
    try_decref(&zv_);
    /*
    if (Z_REFCOUNTED_P(&zv_))
    {
        zv_.value.counted->gc.refcount++;
    }
    */
    
}

void // protected
val_rc::lose()
{
    try_decref(&zv_);
    zv_ = {0};
    ZVAL_NULL(&zv_);
}

const val_rc& 
val_rc::operator=(zend_long value)
{
    try_decref(&zv_);
    zv_ = {0};
    ZVAL_LONG(&zv_, value);
    return *this;
}

const val_rc& 
val_rc::operator=(double value)
{
    try_decref(&zv_);
    zv_ = {0};
    ZVAL_DOUBLE(&zv_, value);
    return *this;
}

val_rc::val_rc() 
{
    zv_ = {0};
    ZVAL_NULL(&zv_);
}

void val_rc::set_null()
{
    try_decref(&zv_);
    zv_ = {0};
    ZVAL_NULL(&zv_);
}

void val_rc::set_bool(bool value)
{
    try_decref(&zv_);
    zv_ = {0};
    ZVAL_BOOL(&zv_, value);
}

val_rc::val_rc(HashTable* ht)
{
     zv_ = {0};
     val_ptr(&zv_).bind_array(ht);
}

val_rc::val_rc(base_d* cobj)
{
    zv_ = {0};
    
    val_ptr(&zv_).bind_object(cobj->vobj());
}

val_rc::val_rc(double value)
{
    zv_ = {0};
    ZVAL_DOUBLE(&zv_, value);
}

val_rc::val_rc(bool bval)
{
    zv_ = {0};
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
    zv_ = {0};
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
    zv_ = {0};
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
    zv_ = {0};
    ZVAL_LONG(&zv_, value);
 }

val_rc::val_rc(const val_rc& rc, bool byRef) 
{

    zv_ = {0};
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
    try_decref(&zv_); 
    ZVAL_COPY(&zv_ , &rc.zv_);
    return *this;
}

void 
val_rc::move_zv(zval* return_value)
{
    ZVAL_COPY_VALUE(return_value, &zv_);
    zv_ = {0};
}

void 
val_rc::return_zv(zval* return_value)
{
    ZVAL_COPY_VALUE(return_value, &zv_);
    zv_ = {0};
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
    zv_ = {0};
    val_ptr(&zv_).bind_string(rc);
}

val_rc::val_rc(zend_object* rc)
{
    zv_ = {0};
    val_ptr(&zv_).bind_object(rc);
}

val_rc::val_rc(zend_long value)
{
    zv_ = {0};
    ZVAL_LONG(&zv_, value);
}

val_rc::val_rc(const str_ptr& rc)
{
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

void 
val_rc::try_addref(zval* p)
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

void val_rc::decref()
{
    try_decref(&zv_);
}

void //static.
val_rc::try_decref(zval* p)
{
    if (Z_REFCOUNTED_P(p))
    {
        auto rct = zval_refcount_p(p);
        auto ztype = Z_TYPE_P(p);
        switch(ztype) 
        {
        case IS_STRING:
            {
                zend_string* s = Z_STR_P(p);
                str_rc::try_decref(s);
            }
            break;
        case IS_REFERENCE:
            {
                auto zref = Z_REF_P(p);
                if (rct == 1) 
                {
                    zval_ptr_dtor(&zref->val);
                    efree_size(zref, sizeof(zend_reference));
                    return;
                }
                zref->gc.refcount--;
            }
            break;
        case IS_ARRAY:
            {

                HashTable* ht = Z_ARR_P(p);
                htab_rc::try_decref(ht);
            }
            break;

        case IS_OBJECT:
            {
                obj_rc::try_decref(Z_OBJ_P(p));
            }
            break;
        }
    }
    return;
}

val_rc //static 
val_rc::empty_str()
{
    return val_rc(zend_empty_string);
}

}; // namespace Php

#endif
//val_rc.cpp