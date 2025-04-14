 /*  
  *  PHP extension C++ classes - zpp 
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  */

#ifndef ZVAL_MGR_H
#define ZVAL_MGR_H

namespace zpp {

class zval_user;
class zobj_mgr;
class base_d;
class zstr_mgr;

class ZPP_EXPORT zval_mgr {
protected:
    zval zv_;

    /** try to lose whatever is inside 
     *  Return true if handle was reference counted
     *  and likely was freed and nullified 
     *  (unless interned string or handle)
     **/
    

    /**
     *  Free and clear contents to null
     */
    void lose();

    void init();

    void assign_ptr(zval* p);

    void copy(zval* p);

    friend class zval_user;
    friend class zstr_mgr;
    friend class zobj_mgr;
    friend class htab_mgr;
    friend class htab_walk;
    
public:
    static zval_mgr EmptyArray;
    //! Return true if contents become invalid
    static bool try_decref(zval* p);

    //! Increment reference if not interned or immutable
    static void try_addref(zval* p);

	operator zval*() const  { return (zval*) &zv_; }

	zval_mgr();

    zval_mgr(base_d*);

    zval_mgr(zval* zv);
    

    ~zval_mgr();

    zval_mgr(zend_string* rc);

    zval_mgr(zend_object* rc);

    zval_mgr(zend_long value);

    zval_mgr(bool value);
    
    zval_mgr(const zval_mgr& rc, bool byRef = false);

    zval_mgr(zval_mgr&& rc);

    zval_mgr(HashTable* ht);

    zval_mgr(zstr_mgr&& rc);

    zval_mgr(int value);

    zval_mgr(const zval_user& rc);

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

    const zval_mgr& operator=(const zstr_mgr &rc);

    const zval_mgr& operator=(zend_long value);

    const zval_mgr& operator=(double value);

    const zval_mgr& operator=(const zval_mgr &rc);

    const zval_mgr& operator=(zval* rc);

    const zval_mgr& operator=(zend_object* rc);

    const zval_mgr& operator=(HashTable* rc);

    zval_mgr& operator=(zval_mgr&& rc);

    void move_zv(zval* ret);
    void return_zv(zval* ret);
};

}; // namespace Php
#endif //ZVAL_MGR_H

