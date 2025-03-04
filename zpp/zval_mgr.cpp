 /*  
  *  PHP extension C++ classes - zpp 
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  */

#ifndef ZVAL_MGR_CPP
#define ZVAL_MGR_CPP

namespace zpp {

void 
zval_mgr::init()
{
    zv_ = {0};
    ZVAL_NULL(&zv_);
}

bool //protected
zval_mgr::try_decref()
{
    zval* p =  &zv_;
    if (Z_REFCOUNTED_P(p))
    {
        // Ready to delete?
        if (zval_refcount_p(p) == 1)
        {
            auto ztype = Z_TYPE_P(p);
            if (ztype == IS_STRING)
            {
                zend_string* s = Z_STR_P(p);
                // check for IS_STR_INTERNED
                if (GC_FLAGS(s) & IS_STR_INTERNED)
                {
                	return false;
                }
                zend_string_release(s);
            }
            else if (ztype == IS_REFERENCE)
            {
                auto zref = Z_REF_P(p);
                zval_ptr_dtor(&zref->val);
                efree_size(zref, sizeof(zend_reference));
            }
            else {
                zval_ptr_dtor(p);
            }
            return true; 
        }
        else {
            Z_TRY_DELREF_P(p);
        }
    } 
    // contents are functionally the same;
    return false;  
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

void 
zval_mgr::new_array()
{
    lose();
    HashTable* ht = htab_mgr::new_array();
    // added with rc == 1 
    ZVAL_ARR(&zv_, ht);   
}

void 
zval_mgr::empty_array()
{
    lose();
    // zend_empty_array has rc == 2 
    ZVAL_ARR(&zv_, (zend_array*) &zend_empty_array);   
}


void
zval_mgr::addref()
{
    Z_TRY_ADDREF(zv_);
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
    try_decref();
    init();
}

void // protected
zval_mgr::bind_string(zend_string* s)
{
	ZVAL_STR(&zv_, s);
	if (!(GC_FLAGS(s) & IS_STR_INTERNED))
	{
		GC_ADDREF(s);
	}
}

void // protected
zval_mgr::bind_long(zend_long value)
{
    ZVAL_LONG(&zv_, value);
}

void // protected
zval_mgr::bind_object(zend_object* obj)
{
    ZVAL_OBJ_COPY(&zv_, obj);
}

void 
zval_mgr::bind_array(HashTable* ht)
{
    ZVAL_ARR(&zv_, ht);
    zval_addref_p(&zv_);
}

const zval_mgr& 
zval_mgr::operator=(zend_long value)
{
    try_decref();
    zv_ = {0};
    bind_long(value);
    return *this;
}



zval_mgr::zval_mgr() 
{
    init();
}

void zval_mgr::set_null()
{
    lose();
}

void zval_mgr::set_bool(bool value)
{
    try_decref();
    zv_ = {0};
    ZVAL_BOOL(&zv_, value);
}

zval_mgr::zval_mgr(HashTable* ht)
{
     init();
     bind_array(ht);
}

zval_mgr::zval_mgr(bool bval)
{
    init();
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
    init();
    ZVAL_COPY(&zv_, zv);
}

zval_mgr::zval_mgr(const zval_user& rc)
{
    init();
    ZVAL_COPY(&zv_, rc);
}

const zval_mgr& 
zval_mgr::operator=(zval* rc)
{
    lose();
    ZVAL_COPY(&zv_, rc);
    return *this;
}


 zval_mgr::zval_mgr(int value)
 {
    init();
    ZVAL_LONG(&zv_, value);
 }

zval_mgr::zval_mgr(const zval_mgr& rc, bool byRef) 
{

    init();
    _zval_struct *p = (_zval_struct*) rc;

    if (!byRef)
    {
        ZVAL_DEREF(p);
    }

    ZVAL_COPY(&zv_, p);
    
    if (byRef)
    {
        ZVAL_MAKE_REF(&zv_);
    }
}

zval_mgr::zval_mgr(zval_mgr&& m)
{
    init();
    ZVAL_COPY_VALUE(&zv_, &m.zv_);
    m.init();
}

/** try not to collide with self */

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

	ZVAL_DEREF(p);
	
	// ?? danger of losing with self assign??
	if (p != &zv_)
	{
		// acquire with reference count;
		ZVAL_COPY(&zv_, p);
	}
	// !!do not alter original zval pointed to
	
}

const zval_mgr& 
zval_mgr::operator=(const zval_user &rc)
{
	zval* p = (zval*) rc;

	assign_ptr(p);

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
    lose(); 
    ZVAL_COPY(&zv_, (zval*) &rc.zv_);
    return *this;
}

void 
zval_mgr::move_zv(zval* return_value)
{
    ZVAL_COPY_VALUE(return_value, &zv_);
    init();
}

//! mutate value
void 
zval_mgr::toLong()
{
    zval* p = &zv_;
    
    if (!Z_TYPE_P(p) != IS_LONG) {
        ZVAL_DEREF(p);
        zend_long value =  zval_get_long_ex(p,false);
        lose();
        ZVAL_LONG(&zv_,value);
    }
}

void zval_mgr::toDouble()
{
    zval* p = &zv_;
    if (!Z_TYPE_P(p) != IS_DOUBLE) {
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
    init();
    bind_string(rc);
}

zval_mgr::zval_mgr(zend_object* rc)
{
    init();
    bind_object(rc);
}

zval_mgr::zval_mgr(zend_long value)
{
    init();
    bind_long(value);
}

const zval_mgr& 
zval_mgr::operator=(zend_object* rc)
{
    lose();
    bind_object(rc);
    return *this;
}

}; // namespace Php

#endif
//zval_mgr.cpp