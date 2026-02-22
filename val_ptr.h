 /*  
  *  @file val_ptr.h    
  *  @brief val_ptr class, holds a zval pointer.    
  *  @license BSD 3-Clause License  
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2025 Michael Rynn
  */

#ifndef VAL_PTR_H
#define VAL_PTR_H


#ifndef PHP_EXTERN_H
#include "php_extern.h"
#endif

#ifndef STR_PTR_H
#include "str_ptr.h"
#endif


namespace zpp {

class str_rc;
class val_rc;

/**
 * @class val_ptr
 * @brief val_ptr class, holds a zval pointer. Not reference counted.   
 * A non-null value refers to zval structure that must exists somewhere.
 */
class val_ptr {
protected:
    zval *p_;

    friend class val_rc;
    friend class htab_ptr;
    friend class htab_rw;



public:
    /** bind_xxx calls best on already empty zval */
    void bind_string(zend_string* s);

    void bind_object(zend_object* obj);

    void bind_array(HashTable* ht);

    void bind_long(zend_long value);

    static void set_global(str_ptr key, val_ptr value);
    static val_ptr get_global(str_ptr key);

    // Potential inlines. 
    // Return true if reference counted value
    static bool string_bind(zval* zt, zend_string* s)
    {
        bool result = false;
        if (s) 
        {
            Z_STR_P(zt) = s;
            result = (GC_FLAGS(s) & IS_STR_INTERNED) ? false : true;
            Z_TYPE_INFO_P(zt) = result ? IS_STRING_EX : IS_STRING;
        }
        else {
            ZVAL_NULL(zt);
        }
        return result;
    }

    // return true if reference counted value
    static bool array_bind(zval* zt, HashTable* ht)
    {
        bool result = false;
        if (ht)
        {
            Z_ARR_P(zt)=ht;
            result = (GC_FLAGS(ht) & GC_IMMUTABLE) ? false : true;
            Z_TYPE_INFO_P(zt) = result ? IS_ARRAY_EX : IS_ARRAY;       
        }
        else
        {   
            ZVAL_NULL(zt);
        }
        return result;
    }
    
    // return true if reference counted value
    static bool object_bind(zval* zt, zend_object* obj)
    {
        bool result = false;
        if (obj)
        {
            Z_OBJ_P(zt) = obj;
            result = (GC_FLAGS(obj) & GC_IMMUTABLE) ? false : true;
            Z_TYPE_INFO_P(zt) = result ? IS_OBJECT_EX : IS_OBJECT;
        }
        else
        {   
            ZVAL_NULL(zt);
        }
        return result;
    }

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

    bool is_nullptr() const
    {
        return (!p_);
    }

    bool isObject() const;

    /**
     * @detail A not empty value. Not quite the same as !empty($xx)
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
    rqstring  cstr() const;

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

    void copy_zv(zval* ret) const;
    
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

inline bool operator!=(const val_ptr& a, const val_ptr& b) 
{
    return !a.same(b);
}

};


#endif
