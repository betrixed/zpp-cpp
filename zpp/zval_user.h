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

    friend class zval_mgr;
    friend class htab_read;
    friend class htab_write;

    void bind_string(zend_string* s);

    void bind_object(zend_object* obj);

    void bind_array(HashTable* ht);

    void bind_long(zend_long value);


public:

    static zval* real_zval(zval* zv)
    {
        switch(Z_TYPE_P(zv)) {
            case IS_REFERENCE:
                zv = Z_REFVAL_P(zv);
                break;
            case IS_INDIRECT:
                zv = zv->value.zv; 
                break;  
        }
        return zv;
    }

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

    void setbool(bool value);
    
    // dereference if necessary
    zval_user referent();

    //! methods to check contained PHP type
    int  ref_type() const;

    bool isDouble() const;

    bool isLong() const;

    bool isArray() const;
    
    bool isNull() const;

    bool isObject() const;

    /** A not empty value. Not quite the same as !empty($xx)
     * Not UNDEFINED, NULL or FALSE.
     * NON-empty array or string. All LONG or DOUBLE are considered NOT empty!
     */
    bool ok() const;

    bool isString() const;

    bool isTrue() const;

    bool isFalse() const;

    bool isPointer() const;
    
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

    void return_zv(zval* ret);
    
    int refcount() const;
    int ztype() const {
        if (!p_)
            return 0;
        return Z_TYPE_P(p_);
    }

    void init()
    {
        p_ = nullptr;
    }
    /** 
     * This will be a mistake, for zval_mgr returned from a function.
     * zval_user data = some_func(); where declared as zval_mgr some_func();
     * 
     * as the temporary zval_mgr will disappear, leaving zval_user with a
     * dangling pointer to its zval* memory.
     * 
     */
    const zval_user& operator=(const zval_mgr& rc);

};

};


#endif
