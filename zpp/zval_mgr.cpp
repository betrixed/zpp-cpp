 /*  
  *  PHP extension C++ classes - zpp 
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  */

#ifndef ZVAL_MGR_CPP
#define ZVAL_MGR_CPP

#ifndef ZVAL_MGR_H
#include "zval_mgr.h"
#endif

#ifndef ZVAL_USER_H
#include "zval_user.h"
#endif

#ifndef ZSTR_MGR_H
#include "zstr_mgr.h"
#endif

#ifndef HTAB_MGR_H
#include "htab_mgr.h"
#endif

#ifndef ZOBJ_MGR_H
#include "zobj_mgr.h"
#endif

namespace zpp {

zval_mgr zval_mgr::EmptyArray = zval_mgr((HashTable*) &zend_empty_array);

void 
zval_mgr::init()
{
    zv_ = {0};
    ZVAL_NULL(&zv_);
}

int   
zval_mgr::ref_type() const
{
    return Z_TYPE_P((const zval*) zval_user::real_zval(&zv_));
}

void 
zval_mgr::make_ref()
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

zval_mgr::~zval_mgr()
{
    zval_mgr::try_decref(&zv_);
}


zend_string* 
zval_mgr::zstr() const
{
    zval* p = zval_user::real_zval(&zv_);
    if (Z_TYPE_P(p) != IS_STRING)
    {
        return nullptr;
    }
    return Z_STR_P(p);
}

HashTable*   
zval_mgr::zarray() const
{
    zval* p = zval_user::real_zval(&zv_);
    if (Z_TYPE_P(p) != IS_ARRAY)
    {
        return nullptr;
    }
    return Z_ARR_P(p);
}

void 
zval_mgr::new_array()
{
    lose();
    HashTable* ht = htab_mgr::new_array();
    // added with rc == 1 
    zval_user(&zv_).bind_array(ht);  
    htab_mgr::try_decref(ht); // because new primary mgr
    //showmem("new_array", &zv_);
}

void 
zval_mgr::empty_array()
{
    lose();
    // zend_empty_array has rc == 2 
    zval_user(&zv_).bind_array((zend_array*) &zend_empty_array);
}


void
zval_mgr::addref()
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
zval_mgr::lose()
{
    try_decref(&zv_);
    zv_ = {0};
    ZVAL_NULL(&zv_);
}

const zval_mgr& 
zval_mgr::operator=(zend_long value)
{
    try_decref(&zv_);
    zv_ = {0};
    ZVAL_LONG(&zv_, value);
    return *this;
}

const zval_mgr& 
zval_mgr::operator=(double value)
{
    try_decref(&zv_);
    zv_ = {0};
    ZVAL_DOUBLE(&zv_, value);
    return *this;
}

zval_mgr::zval_mgr() 
{
    zv_ = {0};
    ZVAL_NULL(&zv_);
}

void zval_mgr::set_null()
{
    lose();
}

void zval_mgr::set_bool(bool value)
{
    try_decref(&zv_);
    zv_ = {0};
    ZVAL_BOOL(&zv_, value);
}

zval_mgr::zval_mgr(HashTable* ht)
{
     zv_ = {0};
     zval_user(&zv_).bind_array(ht);
}

zval_mgr::zval_mgr(base_d* cobj)
{
    zv_ = {0};
    zval_user(&zv_).bind_object(cobj->vobj());
}

zval_mgr::zval_mgr(bool bval)
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

zval_mgr::zval_mgr(zval* zv)
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

zval_mgr::zval_mgr(const zval_user& rc)
{
    zv_ = {0};
    if (rc.p_) {
        ZVAL_COPY(&zv_, rc.p_);
    }
    else {
        ZVAL_NULL(&zv_);
    }    
}

const zval_mgr& 
zval_mgr::operator=(zval* rc)
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

const zval_mgr& 
zval_mgr::operator=(const htab_mgr &rc)
{
    lose();
    HashTable* ht = rc.ht_;
    if (ht)
    {
        zval_user(&zv_).bind_array(ht);
    }
    return *this;
}

 zval_mgr::zval_mgr(int value)
 {
    zv_ = {0};
    ZVAL_LONG(&zv_, value);
 }

zval_mgr::zval_mgr(const zval_mgr& rc, bool byRef) 
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

zval_mgr::zval_mgr(zval_mgr&& m)
{
    ZVAL_COPY_VALUE(&zv_, &m.zv_);
    m.init();
}


/** copy with careful addref */
void
zval_mgr::copy(zval *p)
{
    ZVAL_COPY(&zv_, p);
    //try_addref(&zv_);
}
void 
zval_mgr::assign_ptr(zval* p)
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
zval_mgr::ok() const
{
    return zval_user(*this).ok();
}

const zval_mgr& 
zval_mgr::operator=(const zval_user &rc)
{
	zval* p = (zval*) rc;

	assign_ptr(p);

	return *this;		
}

const zval_mgr& 
zval_mgr::operator=(const zstr_user& rc)
{
    lose();
    zval_user(&zv_).bind_string(rc.s);
    return *this;
}

zval_mgr& 
zval_mgr::operator=(zval_mgr&& rc)
{
	if (&rc != this)
	{
		lose();
        ZVAL_COPY_VALUE(&zv_, &rc.zv_);
		rc.init();
	}
    return *this;
}

const zval_mgr& 
zval_mgr::operator=(const zobj_mgr &rc)
{
    lose();
    zend_object* obj = (zend_object*) rc;
    if (obj)
    {
        ZVAL_OBJ_COPY(&zv_, obj);
    }
    return *this;
}

const zval_mgr& 
zval_mgr::operator=(const zval_mgr &rc)
{
    try_decref(&zv_); 
    ZVAL_COPY(&zv_ , &rc.zv_);
    return *this;
}

void 
zval_mgr::move_zv(zval* return_value)
{
    ZVAL_COPY_VALUE(return_value, &zv_);
    zv_ = {0};
    ZVAL_NULL(&zv_);
}

void 
zval_mgr::return_zv(zval* return_value)
{
    ZVAL_COPY(return_value, &zv_);
}


//! mutate value
void 
zval_mgr::toLong()
{
    zval* p = &zv_;
    
    if (Z_TYPE_P(p) != IS_LONG) {
        ZVAL_DEREF(p);
        zend_long value =  zval_get_long_ex(p,false);
        lose();
        ZVAL_LONG(&zv_,value);
    }
}

void zval_mgr::toDouble()
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
zval_mgr::toString() 
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

zval_mgr::zval_mgr(zstr_mgr&& rc)
{
    if (rc.s)
    {
        ZVAL_STR(&zv_, rc.s);
        rc.s = nullptr;
    }
}

zval_mgr::zval_mgr(zend_string* rc)
{
    zv_ = {0};
    zval_user(&zv_).bind_string(rc);
}

zval_mgr::zval_mgr(zend_object* rc)
{
    zv_ = {0};
    zval_user(&zv_).bind_object(rc);
}

zval_mgr::zval_mgr(zend_long value)
{
    zv_ = {0};
    ZVAL_LONG(&zv_, value);
}

const zval_mgr& 
zval_mgr::operator=(zend_object* rc)
{
    lose();
    zval_user(&zv_).bind_object(rc);
    return *this;
}

const zval_mgr& 
zval_mgr::operator=(HashTable* rc)
{
    lose();
    zval_user(&zv_).bind_array(rc);
    return *this;
}

void 
zval_mgr::try_addref(zval* p)
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

bool //static. Return true if contents become invalid
zval_mgr::try_decref(zval* p)
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
                if (GC_FLAGS(s) & IS_STR_INTERNED)
                {
                    return false;
                }
                zend_string_release(s);
                return (rct==1);
            }
            
        case IS_REFERENCE:
            {
                auto zref = Z_REF_P(p);
                if (rct == 1) 
                {
                    zval_ptr_dtor(&zref->val);
                    efree_size(zref, sizeof(zend_reference));
                    return true;
                }
                zref->gc.refcount--;
                return false;
            }
            
        case IS_ARRAY:
            {

                HashTable* ht = Z_ARR_P(p);
                //showarray("try_decref", ht);
                if (ht->gc.u.type_info & GC_IMMUTABLE)
                {
                    return false;
                }
                if (rct<=1) 
                {
                    zend_array_destroy(ht);
                    return true;
                }
                rct = GC_DELREF(ht);
                //zend_printf("new rct %d for %lx\n", rct, ht);
            }
           
            return false;

        case IS_OBJECT:
            {
                return zobj_mgr::try_decref(Z_OBJ_P(p));
            }
        }
    }
    return false;
}

}; // namespace Php

#endif
//zval_mgr.cpp