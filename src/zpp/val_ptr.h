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

#ifdef isset 
#undef isset
#endif
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


    static zval* real_zval(zval* zv);

    val_ptr refto() const;
    val_ptr indto() const;


    void bind_string(zend_string* s);

    void bind_object(zend_object* obj);

    void bind_array(HashTable* ht);

    void bind_long(zend_long value);
    
public:
    static val_ptr nullval();

    static void try_decref(zval* p);

    static void try_addref(zval* p);
    
    /** bind_xxx calls for empty zval */
 

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

    

    val_ptr() : p_(nullptr) {}

    val_ptr(zval* rc) : p_(rc) {}
    
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
    
    bool isEmpty() const;
    bool isArray() const;
    bool isBoolean() const;
    bool isCallable() const;
    bool isDouble() const;
    bool isFalse() const;
    bool isLong() const;
    bool isNull() const;

    bool is_nullptr() const
    {
        return (!p_);
    }

    bool isObject() const;
    bool isPointer() const;
    bool isReference() const;
    bool isResource() const;
    bool isString() const;
    bool isTrue() const;
    bool ok() const;
    

    int refcount() const;

    int ztype() const 
    {
        if (!p_)
        {
            return IS_UNDEF;
        }
        return p_->u1.v.type; //Z_TYPE_P(p_);
    }
    
    bool isset() const 
    {
        switch(ztype())
        {
        case IS_UNDEF:
        case IS_NULL:
            return false;
        default:
            return true;
        }
    }
    // Ultimately returnable value type
    int  ref_type() const;
    

    /** zend_object* methods */

    //! return interned class name string if an object, else nullptr
    zend_string* className() const;

    //! return  the zend_object* pointer, else nullptr
    zend_object* zobject() const;

    //! size method for string or array, else return 0 */
    size_t size() const;

    //! Return zend_string wrapper, or coerced string */
    str_rc  to_zstr() const;

    //! return  the zend_string* , else nullptr
    zend_string* zstr() const;

    //! return value as a string
    rqstring  cstr() const;

    //! return string_view with value, if IS_STRING
    std::string_view vstr() const;

    //! return zend_string* pointer, if length > 0
    bool getStringData(zend_string** retstr = nullptr) const ;

    //! Contains a pointer ,  as void* or nullptr
    void*  voidptr() const;

    /** value conversion to scaler methods */
    //! return or convert to double
    double  get_double() const;

    //! return or convert to boolean
    bool get_bool() const; 

    //! return or convert to long
    zend_long get_long() const;

    //! return HashTable* pointer or nullptr
    HashTable* zarray() const;

    //! copy this zval* contents to argument.
    void copy_zv(zval* ret) const;
    
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

    static zval* php_constant(str_ptr name);
    /*
    void  set_zlong(zend_long val);
    void  set_zstr(str_ptr val);
    void  set_htab(htab_ptr val);
    void  set_zobj(obj_ptr val);
    
    void  make_ref();
    */
};

inline bool operator!=(const val_ptr& a, const val_ptr& b) 
{
    return !a.same(b);
}

};


#endif
