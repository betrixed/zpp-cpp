#ifndef WCC_HMAP_H
#define WCC_HMAP_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

#define HMAP_DIMENSIONS
#define HMAP_ITERATOR

namespace wcc {


class Hmap_php 
{
public:
	static ZEND_RESULT_CODE count_elements(zend_object* object, zend_long* count);
	
	static zval* 	  read_property(zend_object* object, zend_string* name, int type, void** cache_slot, zval* rv);
	static zval* 	  write_property(zend_object* object, zend_string* name, zval *value, void** cache_slot);
	static int  	  has_property(zend_object* object, zend_string* name, int has_set_exists, void** cache_slot);
	static void  	  unset_property(zend_object* object, zend_string* name, void **cache_slot);
	static zval* 	  get_property_ptr_ptr(zend_object* object, zend_string* name, int type, void** cache_slot);
	static HashTable* get_properties_for(zend_object* obj, zend_prop_purpose purpose);


	static zval* read_dimension(zend_object* obj, zval* offset, int type, zval* return_value);
	static void  write_dimension(zend_object* obj, zval* offset, zval* set_value);
	static void  unset_dimension(zend_object* object, zval* unset);
	static int   has_dimension(zend_object* object, zval* offset, int check_empty);
};

class HmapIterator {
public:
        HmapIterator();

        //! class operator new does not get called in PHP_DEBUG builds
        void* operator new(size_t count);
        void operator delete(void* ptr);

        zend_object_iterator             phpit;
        htab_walk			 walk;
        htab_rc   			 htab;
                    // function table managed by iterator
        static HmapIterator* phmi(zend_object_iterator* zoi) {
                return (HmapIterator*) zoi;
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
        	ce->get_iterator = HmapIterator::create;
        }

        static void setup_handlers(zend_object_handlers& hand)
        {
		hand.count_elements = Hmap_php::count_elements;
		hand.read_property = Hmap_php::read_property;
		hand.write_property = Hmap_php::write_property;
		hand.get_property_ptr_ptr = Hmap_php::get_property_ptr_ptr;
		hand.has_property = Hmap_php::has_property;
		hand.unset_property = Hmap_php::unset_property;

		hand.get_properties_for = Hmap_php::get_properties_for;

		//hand.get_debug_info = nullptr;

		hand.read_dimension = Hmap_php::read_dimension;
		hand.write_dimension = Hmap_php::write_dimension;
		hand.has_dimension = Hmap_php::has_dimension;
		hand.unset_dimension = Hmap_php::unset_dimension;

		
        }
        /* Expose owned values to GC. */
        //HashTable *(*get_gc)(zend_object_iterator *iter, zval **table, int *n);
};

//* for maybe class inheritance of Hmap
class Hmap;


class Hmap_mgr : public base_obj_mgr<Hmap>
{
protected:
	//typedef base_obj_mgr<T> mydef;

	virtual void init_class_fn() 
	{
	mydef::init_class_fn();

	HmapIterator::setup_handlers(mydef::handlers_);
	HmapIterator::setup_class(mydef::class_entry_);
	}
};


class Hmap : public base_d {
protected:
	htab_rc data_;

	friend class Hmap_php;

public:
	//! Object handlers to install
	Hmap();
	virtual ~Hmap();


	htab_ptr reader() const 
	{
            return data_;
	}

	htab_rw writer()
	{
	    return htab_rw(data_);
	}
	static Hmap_mgr omg;
	
	//void debug_info(htab_rw hw) override;
	
	//! Create a new Hmap constructed witn HashTable/zend_array
	static obj_rc newFromArray(val_ptr init);
	static obj_rc new_hmap();
	
	virtual void debug_info(htab_rw hw);

	void construct(htab_ptr values);

	/** Avoid warning for missing property */
	val_rc getOrNot(str_ptr name, val_ptr ifnot);

	bool   has(str_ptr name);
	
	val_rc get(str_ptr name);
	val_rc get(val_ptr name);

	void   set(str_ptr name, val_ptr value);
	//void   set(val_ptr key, val_ptr value);

	void   unset(str_ptr name);
	void   unset(val_ptr key);
	
	bool   has(val_ptr  key);

	// for ArrayAccess interface, dimensions interface

	htab_rc subsetkey(str_ptr key);
	htab_rc subset(htab_ptr data);

	void   addArray(htab_ptr data);
	void   assign(htab_ptr data);

	htab_ptr toArray();

	zend_long count() const;

	str_rc unhive(str_ptr data);

	htab_rc serialize();
	void     unserialize(htab_ptr htab);

	void      clear();

	VIRTUAL_ZOBJPTR

	static htab_ptr map_htab(obj_rc mobj) 
	{
		Hmap* hmap = zobj_toc<Hmap>(mobj);
		return hmap->reader();
	}
};



}; // namespace wcc

//wcc_hmap.h
#endif