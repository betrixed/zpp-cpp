#ifndef WCC_HMAP_H
#define WCC_HMAP_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

namespace wcc {

class Hmap : public base_d {
public:
	static ZEND_RESULT_CODE count_elements(zend_object* object, zend_long* count);
	static zval* 	read_property(zend_object* object, zend_string* name, int type, void** cache_slot, zval* rv);
	static zval* 	write_property(zend_object* object, zend_string* name, zval *value, void** cache_slot);
	static int  	has_property(zend_object* object, zend_string* name, int has_set_exists, void** cache_slot);
	static void  	unset_property(zend_object* object, zend_string* name, void **cache_slot);
	static zval* 	get_property_ptr_ptr(zend_object* object, zend_string* name, int type, void** cache_slot);
	
	static HashTable* get_properties_for(zend_object* obj, zend_prop_purpose purpose);

	//static HashTable* get_properties(zend_object* object);
	class Hmap_Mgr : public base_obj_mgr<Hmap>
	{
	protected:
		virtual void init_class_fn() {
		// base class
			mydef::init_class_fn();

			zend_object_handlers& hand = mydef::handlers_;

			hand.count_elements = Hmap::count_elements;
			hand.read_property = Hmap::read_property;
			hand.write_property = Hmap::write_property;
			hand.get_property_ptr_ptr = Hmap::get_property_ptr_ptr;
			hand.has_property = Hmap::has_property;
			hand.unset_property = Hmap::unset_property;
			//hand.get_properties = Hmap::get_properties;

			hand.get_properties_for = Hmap::get_properties_for;


			hand.get_debug_info = nullptr;

			/*
			hand.read_dimension = Hmap::read_dimension;
			hand.write_dimension = Hmap::write_dimension;
			hand.has_dimension = Hmap::has_dimension;
			hand.unset_dimension = Hmap::unset_dimension;
			*/
			
			/*
			
			hand.get_gc = Hmap::get_gc;
			hand.read_property = Hmap::read_property;
			hand.write_property = Hmap::write_property;
			hand.get_property_ptr_ptr = Hmap::get_property_ptr_ptr;
			hand.has_property = Hmap::has_property;
			hand.unset_property = Hmap::unset_property;

			zend_class_entry* ce = mydef::class_entry_;
			*/
			
		}
	};

	static Hmap_Mgr omg;
	
protected:
	htab_empty data_;

	

		/*
	static zval* read_dimension(zend_object* obj, zval* offset, int type, zval* return_value);
  static void write_dimension(zend_object* obj, zval* offset, zval* set_value);
  static void unset_dimension(zend_object* object, zval *unset);
	static int  has_dimension(zend_object* object, zval *offset, int check_empty);

	static zval* read_property(zend_object* object, zend_string* name, int type, void **cache_slot, zval *rv);
	static zval* write_property(zend_object* object, zend_string* name, zval *value, void **cache_slot);
	static int  has_property(zend_object* object, zend_string* name, int has_set_exists, void **cache_slot);
	static void  unset_property(zend_object* object, zend_string* name, void **cache_slot);
	static zval* get_property_ptr_ptr(zend_object* object, zend_string* name, int type, void **cache_slot);
	
	static ZEND_RESULT_CODE count_elements(zend_object* object, zend_long *count);

	static HashTable* get_gc(zend_object* obj, zval **gc_data, int *gc_data_count);
		*/
public:

	//void debug_info(htab_write hw) override;
	
	void construct(htab_read values);

	/** Avoid warning for missing property */
	zval_mgr getOrNot(zstr_user name, zval_user ifnot);

	bool   has(zstr_user name);
	
	zval_mgr get(zstr_user name);
	
	void   set(zstr_user name, zval_user value);

	void   unset(zstr_user name);
	
#ifdef HMAP_DIMENSIONS
	zval_mgr get(zval_user name);
	void   set(zval_user key, zval_user value);
	void 	 unset(zval_user key);
	bool   has(zval_user  key);
#endif

	// for ArrayAccess interface, dimensions interface

	htab_mgr subsetkey(zstr_user key);
	htab_mgr subset(htab_read data);

	void   addArray(htab_read data);

	htab_read toArray();

	zend_long count() const;

	zstr_mgr unhive(zstr_user data);

	htab_mgr serialize();
	void     unserialize(htab_read htab);

	void      clear();

	VIRTUAL_ZOBJPTR
};



}; // namespace wcc

//wcc_hmap.h
#endif