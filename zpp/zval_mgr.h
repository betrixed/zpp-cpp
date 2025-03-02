 /*  
  *  PHP extension C++ classes - zpp 
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  */

#ifndef ZVAL_MGR_H
#define ZVAL_MGR_H

#ifndef ZVAL_INIT_H
#include "zval_init.h"
#endif


namespace zpp {

class zval_user;
class zobj_mgr;
class base_d;

class ZPP_EXPORT zval_mgr {
protected:
    zval zv_;

    /** try to lose whatever is inside 
     *  Return true if handle was reference counted
     *  and likely was freed and nullified 
     *  (unless interned string or handle)
     **/
    bool try_decref();

    /**
     *  Free and clear contents to null
     */
    void lose();

    void init();

	void bind_string(zend_string* s);

    void bind_object(zend_object* obj);

    void bind_array(HashTable* ht);

    void assign_ptr(zval* p);

    void bind_long(zend_long value);

    friend class zval_user;
    friend class zstr_mgr;
    friend class zobj_mgr;
    friend class htab_mgr;
    
public:
	/** not usually called directlly */

	operator zval*() const  { return (zval*) &zv_; }

	zval_mgr();

    zval_mgr(base_d*);

    zval_mgr(zval* zv);
    

    ~zval_mgr()
    {
        lose();
    }

    zval_mgr(zend_string* rc)
    {
    	bind_string(rc);
    }

    zval_mgr(zend_object* rc)
    {
        bind_object(rc);
    }

    zval_mgr(zend_long value)
    {
        bind_long(value);
    }

    zval_mgr(bool value);
    
    zval_mgr(const zval_mgr& rc, bool byRef = false);

    zval_mgr(zval_mgr&& rc);

    zval_mgr(HashTable* ht);

    zval_mgr(zstr_mgr&& rc);

    zval_mgr(int value);

    //! mutate to suggested type if necessary
    void     toLong();
    void     toDouble();
    void     toString();

    void     decref();
    void     addref();

    void make_ref();
    void set_bool(bool value);
    void set_null();

    void new_array();
    void empty_array();

    const zval_mgr& operator=(const zval_user &rc);

    const zval_mgr& operator=(const zobj_mgr &rc);

    const zval_mgr& operator=(zend_long value);

    const zval_mgr& operator=(const zval_mgr &rc);

    zval_mgr& operator=(zval_mgr&& rc);

    void move_zv(zval* ret);
    
};

}; // namespace Php
#endif //ZVAL_MGR_H

