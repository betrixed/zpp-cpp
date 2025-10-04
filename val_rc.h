 /*  
  *  @file val_rc.h
  *  @brief val_rc class, reference counted zval value.  
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  *  @license Artistic License 2.0
  */

#ifndef VAL_RC_H
#define VAL_RC_H

#ifndef PHP_EXTERN_H
#include "php_extern.h"
#endif



namespace zpp {

class val_ptr;
class obj_rc;
class base_d;
class str_rc;
class obj_ptr;

/**
 * @class val_rc
 * @brief val_rc class, reference counted zval value.  
 * Holds an actual zval structure, not a pointer to one.
 * 
 */
class val_rc {
protected:
    zval zv_;
    
    void lose();

    void init();

    void assign_ptr(zval* p);

    void copy(zval* p);

    friend class val_ptr;
    friend class str_rc;
    friend class obj_rc;
    friend class htab_rc;
    friend class htab_walk;

    static val_rc EmptyArray;

public:
    
    
    static void try_decref(zval* p);

    static void try_addref(zval* p);

	operator zval*() const  { return (zval*) &zv_; }

	val_rc();

    val_rc(zval* zv);
    

    ~val_rc();

    val_rc(zend_string* rc);

    val_rc(zend_object* rc);

    val_rc(zend_long value);

    val_rc(base_d* cobj);

    val_rc(bool value);
    
    val_rc(const val_rc& rc, bool byRef = false);

    val_rc(val_rc&& rc);


    val_rc(zval&& rc)
    {
        ZVAL_COPY_VALUE(&zv_, &rc);
        rc = {0};
    }

    val_rc(HashTable* ht);

    val_rc(str_rc&& rc);

    val_rc(int value);

    val_rc(double value);

    val_rc(const val_ptr& rc);

    val_rc(const str_ptr& rc);


    //! mutate in place to suggested type if necessary
    void     toLong();
    void     toDouble();
    void     toString();

    void     decref();
    void     addref();

    void    make_ref();
    void    set_bool(bool value);
    void    set_null();


    void new_array();
    void empty_array();

    const val_rc& operator=(const val_ptr &rc);

    const val_rc& operator=(const obj_ptr &rc);

    const val_rc& operator=(const str_ptr &rc);

    const val_rc& operator=(const htab_rc &rc);

    const val_rc& operator=(zend_long value);

    const val_rc& operator=(double value);

    const val_rc& operator=(const val_rc &rc);

    const val_rc& operator=(zval* rc);

    const val_rc& operator=(zend_object* rc);

    const val_rc& operator=(HashTable* rc);

    const val_rc& operator=(zend_string* rc);

    val_rc& operator=(val_rc&& rc);

    void move_zv(zval* ret);
    void return_zv(zval* ret);

    zend_string* zstr() const;
    HashTable*   zarray() const;
    zend_long    zlong() const;

    zend_object* zobject() const;
    
    int   ref_type() const;
    
    // inlines

    bool ok() const;
    
    int   ztype() const {
        return Z_TYPE_P(&zv_);
    }

    bool isDouble() const
    {
        return (ref_type() == IS_DOUBLE);
    }

    bool isLong() const
    {
        return (ref_type() == IS_LONG);
    }

    bool isArray() const
    {
        return (ref_type() == IS_ARRAY);
    }

    bool isResource() const
    {
        return (ref_type() == IS_RESOURCE);
    }

    bool isNull() const
    {
        return (ref_type() == IS_NULL);
    }

    bool isObject() const
    { 
        return (ref_type() == IS_OBJECT);
    }

    bool isString() const
    {
        return (ref_type() == IS_STRING);
    }

    bool isTrue() const 
    {
        return (ref_type() == IS_TRUE);
    }

    bool isFalse() const 
    {
        return (ref_type() == IS_FALSE);
    }

    bool isPointer() const
    {
        return (ref_type() == IS_PTR);
    }

    static val_rc empty_str(); 
};

}; // namespace Php
#endif //VAL_RC_H

