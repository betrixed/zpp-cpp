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
    ZVAL_MAKE_REF(&zv_);
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
		zend_string_addref(s);
	}
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

zval_mgr::zval_mgr() 
{
    init();
}

zval_mgr::zval_mgr(HashTable* ht)
{
     init();

}

zval_mgr::zval_mgr(zval* zv)
{
    init();
    ZVAL_COPY(&zv_, zv);
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

void 
zval_mgr::move_zv(zval* return_value)
{
    ZVAL_COPY_VALUE(return_value, &zv_);
    init();
}

}; // namespace Php

#endif
//zval_mgr.cpp