#ifndef WCC_STR8_H
#define WCC_STR8_H


#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif


namespace wcc {

using namespace zpp;

class Str8Iterator {
public:
    Str8Iterator();

    //! class operator new does not get called in PHP_DEBUG builds
    void* operator new(size_t count);
    void  operator delete(void* ptr);

    zend_object_iterator phpit;
    obj_rc      s8;
    str_ptr     zstr;
    long  	  	current; // 0 - indexed offset
	long  	  	next;	   // 0 - indexed offset
	long  	    ucode; 

                // function table managed by iterator
    static Str8Iterator* psi(zend_object_iterator* zoi) {
		return (Str8Iterator*) zoi;
    }

    static zend_object_iterator_funcs  it_fntab_; 

    // create instance static function
    static zend_object_iterator* create(zend_class_entry* ce, zval* zobj, int byRef);
    // functions to slot in Zend/zend_iterators.h

    /* release all resources associated with this iterator instance */
    static void  it_dtor(zend_object_iterator *iter);

    /* check for end of iteration (FAILURE or SUCCESS if data is valid) */
    static zend_result  it_valid(zend_object_iterator *iter);

    /* fetch the item data for the current element */
    static zval* it_get_data(zend_object_iterator *iter);

    /* get current key for data, if not auto-increment integer key */
    static void  it_get_key(zend_object_iterator *iter, zval *key);

    /* move to next key/value position */
    static void  it_forward(zend_object_iterator *iter);

    /* rewind to start of data (optional, may be NULL) */
    static void  it_rewind(zend_object_iterator *iter);

    /* invalidate current value/key (optional, may be NULL) */
    static void  it_invalidate(zend_object_iterator *iter);

    static void setup_class(zend_class_entry* ce)
    {
    	ce->get_iterator = Str8Iterator::create;
    }
};

class Str8 : public base_d {
public:
	str_rc   sdata;

	void construct(str_ptr s);
	str_ptr  toString();

};

};

#endif
