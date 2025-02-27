 /*  
  *  PHP extension C++ classes - zpp 
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  */

#ifndef ZVAL_USER_H
#define ZVAL_USER_H


/** It turns out that PHP and its zend types
 *  rarely if ever allocate _struct_zval directly
 *  using heap memory. Allocating and freeing 
 *  _struct_zval is by inclusion and mixin.
 * 
 *  Binding various values requires 
 *  reference counting them all.
 */

#ifndef ZVAL_MGR_H
#include "zval_mgr.h"
#endif

namespace zpp {

class zstr_mgr;

class ZPP_EXPORT zval_user {
protected:
    zval *p_;
public:

    zval_user() : p_(nullptr) {}

    zval_user(zval* rc)
    {
        p_ = rc;
    }   

    zval_user(const zval_user &c)
    {
        p_ = c.p_;
    }

    zval_user(const zval_mgr& mgr)
    {
        p_ = (zval*) mgr;
    }

    operator zval*() const 
    {
        return p_;
    }

    zval_user& operator=(zval* p)
    {
        p_ = p;
        return *this;
    }

    // dereference if necessary
    zval_user referent();

    //! methods to check contained PHP type
    int  ref_type() const;

    bool isDouble() const
    {
        return ((p_ != nullptr) && (ref_type() == IS_DOUBLE));
    }

    bool isLong() const
    {
        return ((p_ != nullptr) && (ref_type() == IS_LONG));
    }

    bool isArray() const
    {
        return ((p_ != nullptr) && (ref_type() == IS_ARRAY));
    }

    
    bool isNull() const
    {
        return ((p_ == nullptr) || (ref_type() == IS_NULL));
    }

    bool isObject() const
    { 
        return ((p_ != nullptr) && (ref_type() == IS_OBJECT));
    }

    bool isString() const
    {
        return ((p_ != nullptr) && (ref_type() == IS_STRING));
    }

    bool isTrue() const 
    {
        return ((p_ != nullptr) && (ref_type() == IS_TRUE));
    }

    bool isFalse() const 
    {
        return ((p_ == nullptr) || (ref_type() == IS_FALSE));
    }

    bool isPointer() const
    {
        return ((p_ == nullptr) || (ref_type() == IS_PTR));
    }

    bool isCallable() const;


    /** zend_object* methods */

    //! return interned class name string if an object, else nullptr

    zend_string* className() const;

    //! return  the zend_object* pointer, else nullptr
    zend_object* zobject() const;

    /** size methods for string or array, else return 0 */
    size_t size() const;

    /** zend_string* methods */
    zstr_mgr  to_zstr() const;

    //! return  the zend_string* , else nullptr
    zend_string* zstr() const;

    //! return value as a string
    std::string  cstr() const;

    //! return string value, if references real string
    std::string_view vstr() const;

    //! return zend_string* pointer and if length > 0
    bool getStringData(zend_string** retstr = nullptr) const ;

    //! Contains a pointer ,  as void* or nullptr
    void*  voidptr() const;

    /** value conversion to scaler methods */
    //! return or convert to double
    double  zdouble() const;

    //! return or convert to boolean
    
    bool zbool() const; 

    //! return or convert to long
    zend_long zlong() const;

    //! return HashTable* pointer or nullptr
    HashTable* zarray() const;

    int ztype() const {
        if (!p_)
            return 0;
        return Z_TYPE_P(p_);
    }

};

};


#endif
