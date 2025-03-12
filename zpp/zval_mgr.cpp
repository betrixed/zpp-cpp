 /*  
  *  PHP extension C++ classes - zpp 
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  */

#ifndef ZVAL_MGR_CPP
#define ZVAL_MGR_CPP

namespace zpp {

zval_mgr zval_mgr::EmptyArray = zval_mgr((HashTable*) &zend_empty_array);

void 
zval_mgr::init()
{
    zv_ = {0};
    ZVAL_NULL(&zv_);
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
    init();
}

const zval_mgr& 
zval_mgr::operator=(zend_long value)
{
    try_decref(&zv_);
    zv_ = {0};
    ZVAL_LONG(&zv_, value);
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
    try_decref(&zv_);
    zv_ = {0};
    ZVAL_BOOL(&zv_, value);
}

zval_mgr::zval_mgr(HashTable* ht)
{
     init();
     zval_user(&zv_).bind_array(ht);
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
    ZVAL_COPY_VALUE(&zv_, zv);
}

zval_mgr::zval_mgr(const zval_user& rc)
{
    init();
    ZVAL_COPY_VALUE(&zv_, rc);
}

const zval_mgr& 
zval_mgr::operator=(zval* rc)
{
    lose();

    ZVAL_COPY_VALUE(&zv_, rc);
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

    copy(p);
    
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


/** copy with careful addref */
void
zval_mgr::copy(zval *p)
{
    ZVAL_COPY_VALUE(&zv_, p);
    try_addref(&zv_);
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

	ZVAL_DEREF(p);
	
	// ?? danger of losing with self assign??
	if (p != &zv_)
	{
		// acquire with reference count;
		copy(p);
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
		copy(&rc.zv_);
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
    copy((zval*) &rc.zv_);
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
    zval_user(&zv_).bind_string(rc);
}

zval_mgr::zval_mgr(zend_object* rc)
{
    init();
    zval_user(&zv_).bind_object(rc);
}

zval_mgr::zval_mgr(zend_long value)
{
    init();
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
        switch(ztype) {
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
                if (ht->gc.u.type_info & GC_IMMUTABLE)
                {
                    return false;
                }
                if (rct==1) 
                {
                    zend_array_destroy(ht);
                    return true;
                }
                ht->gc.refcount--;
            }
           
            return false;

        case IS_OBJECT:
            {
                zend_object* ob = Z_OBJ_P(p);
                zend_object_release(ob);
                return (rct==1);
            }
        }
    }
    return false;
}

}; // namespace Php

#endif
//zval_mgr.cpp