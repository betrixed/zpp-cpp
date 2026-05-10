#ifndef WCC_PAIR_H
#define WCC_PAIR_H

//pair.h

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

namespace wcc {
using namespace zpp;

	
	class Pair : public base_d 
	{
	public:

		/*

		static zval* get_property_ptr_ptr(zend_object *zobj, zend_string *name, 
						int type, void **cache_slot);

		static zval* read_property(
				zend_object *object, zend_string *member,
				int type, void **cache_slot, zval *rv);

		static zval *write_property(zend_object *zobj, zend_string *name, 
						zval *value, void **cache_slot);

		static int has_property(zend_object *zobj, zend_string *name, 
			             int has_set_exists, void **cache_slot);

		static void unset_property(zend_object *zobj, zend_string *name, void **cache_slot);
	    */

		/*
		class Pair_omg : public base_obj_mgr<Pair> {
		public:
			void init_class_fn() override;


		};
		*/

		/*
		static void set_first(zend_object* pair, zval* val);
		static void set_second(zend_object* pair, zval* val);
		
		static zval* get_first(zend_object* pair,  zval* ret);
		static zval* get_second(zend_object* pair, zval* ret);
		*/

		val_rc one_;
		val_rc two_;

		void construct(val_ptr p1, val_ptr p2);
		
		val_ptr first()
		{
			return val_ptr(one_);
		}

		val_ptr second()
		{
			return val_ptr(two_);
		}

		val_ptr key()
		{
			return val_ptr(one_);
		}

		val_ptr value()
		{
			return val_ptr(two_);
		}
		
		double test_calc() const 
		{
			long a, b;
			a = one_.zlong();
			b = two_.zlong();
			return (a + b) / double(a);
		}

	public:
		static base_obj_mgr<Pair> omg;
		
		VIRTUAL_ZOBJPTR


	};

}; // namespace

#endif