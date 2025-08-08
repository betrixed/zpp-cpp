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

#ifndef PHP_EXTERN_H
#include "php_extern.h"
#endif

#ifndef ZSTR_USER_H
#include "str_ptr.h"
#endif


namespace zpp {

class str_rc;
class val_rc;

class val_ptr {
protected:
    zval *p_;

    friend class val_rc;
    friend class htab_rd;
    friend class htab_wr;

    void bind_string(zend_string* s);

    void bind_object(zend_object* obj);

    void bind_array(HashTable* ht);

    void bind_long(zend_long value);


public:
    static void array_bind(zval* tmp, HashTable* ht);
    static void string_bind(zval* tmp, zend_string* s);
    static void object_bind(zval* temp, zend_object* obj);

    static zval* real_zval(const zval* zv);

    val_ptr() : p_(nullptr) {}

    //val_ptr(const zval* rc) : p_(rc) {}
    val_ptr(const zval* rc);
    
    val_ptr(const val_ptr &c) : p_(c.p_) {}

    val_ptr(const val_rc& mgr);

    operator zval*() const 
    {
        return p_;
    }

    val_ptr& operator=(zval* p)
    {
        p_ = p;
        return *this;
    }

    void setbool(bool value);
    
    // dereference if necessary
    val_ptr referent();

    //! methods to check contained PHP type
    int  ref_type() const;

    bool isDouble() const;

    bool isLong() const;

    bool isArray() const;
    
    bool isNull() const;

    bool isObject() const;

    /** A not empty value. Not quite the same as !empty($xx)
     * Not UNDEFINED, NULL or FALSE.
     * NON-empty array or string. All LONG or DOUBLE are considered ok!
     */
    bool ok() const;

    bool empty() const;
    
    bool isString() const;

    bool isTrue() const;

    bool isFalse() const;

    bool isPointer() const;
    
    bool isCallable() const;

    bool isResource() const;


    /** zend_object* methods */

    //! return interned class name string if an object, else nullptr

    zend_string* className() const;

    //! return  the zend_object* pointer, else nullptr
    zend_object* zobject() const;

    /** size methods for string or array, else return 0 */
    size_t size() const;

    /** zend_string* methods */
    str_rc  to_zstr() const;

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
    bool same(const val_ptr& test) const;
    
    
    void init()
    {
        p_ = nullptr;
    }
    /** 
     * This will be a mistake, for val_rc returned from a function.
     * val_ptr data = some_func(); where declared as val_rc some_func();
     * 
     * as the temporary val_rc will disappear, leaving val_ptr with a
     * dangling pointer to its zval* memory.
     * 
     */
    const val_ptr& operator=(const val_rc& rc);

    static val_ptr php_constant(str_ptr name);

};

bool operator!=(const val_ptr& a, const val_ptr& b) 
{
    return !a.same(b);
}

};


#endif
