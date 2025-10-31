#ifndef WCC_CONFIG_H
#define WCC_CONFIG_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

namespace wcc {

using namespace zpp;

#define CONFIG_HANDLERS

class  Config : public base_d {
public:
	class Config_Mgr : public base_obj_mgr<Config>
	{
	

		virtual void init_class_fn() {
		// base class
			mydef::init_class_fn();

#ifdef CONFIG_HANDLERS
			zend_object_handlers& hand = mydef::handlers_;

			hand.read_dimension = Config::read_dimension;
			hand.write_dimension = Config::write_dimension;
			hand.has_dimension = Config::has_dimension;
			hand.unset_dimension = Config::unset_dimension;

			/*hand.count_elements = Config::count_elements;

			hand.get_gc = Config::get_gc;
			hand.read_property = Config::read_property;
			hand.write_property = Config::write_property;
			hand.get_property_ptr_ptr = Config::get_property_ptr_ptr;
			hand.has_property = Config::has_property;
			hand.unset_property = Config::unset_property;
			hand.count_elements = Config::get_count;
			*/
#endif
			/*
			zend_class_entry* ce = mydef::class_entry_;
			*/
			
		}



	}; // end Config_Mgr

	static Config_Mgr omg;
	
	static obj_rc make(htab_ptr initdata);

	void construct(htab_ptr values);

	/** Avoid warning for missing property */
	val_rc 	  getOrNot(str_ptr name, val_ptr ifnot);

	bool      has(str_ptr name);
	
	val_rc    get(str_ptr name);
	
	void      set(str_ptr name, val_ptr value);

	void      unset(str_ptr name);
	
#ifdef CONFIG_DIMENSIONS
	val_rc    get(val_ptr name);
	void      set(val_ptr  key,  val_ptr value);
	void 	  unset(val_ptr  key);
	bool      has(val_ptr   key);
#endif

	// for ArrayAccess interface, dimensions interface

	htab_rc  subsetkey(str_ptr key);
	htab_rc  subset(htab_ptr data);

	void      addArray(htab_ptr data);

	htab_rc  toArray();

	zend_long count() const;

	str_rc  unhive(str_ptr data);

	void      clear();

#ifdef CONFIG_HANDLERS

		static zval* read_dimension(zend_object* obj, zval* offset, int type,  zval* return_value);
	    static void  write_dimension(zend_object* obj, zval* offset,  zval* set_value);
	    static void  unset_dimension(zend_object *object, zval *unset);
		static int   has_dimension(zend_object *object, zval *offset, int check_empty);

		/*
		static  zval* read_property(zend_object *object, zend_string *name, int type, void **cache_slot, zval *rv);
		static  zval* write_property(zend_object *object, zend_string *name, zval *value, void **cache_slot);
		static  int   has_property(zend_object *object, zend_string *name, int has_set_exists, void **cache_slot);
		static void   unset_property(zend_object *object, zend_string *name, void **cache_slot);
		static zval*  get_property_ptr_ptr(zend_object *object, zend_string *name, int type, void **cache_slot);
		static zval*  get_count(zend_object *object, zend_long *count);
		*/

		static HashTable* get_gc(zend_object *obj, zval **gc_data, int *gc_data_count);

#endif	
	
	VIRTUAL_ZOBJPTR




	virtual   void debug_info(htab_rw hw);
	
};


}; // namespace wcc

//wcc_config.h
#endif