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



		class Pair_omg : public base_obj_mgr<Pair> {
		public:
			void init_class_fn() override;


		};

		/*
		static void set_first(zend_object* pair, zval* val);
		static void set_second(zend_object* pair, zval* val);
		
		static zval* get_first(zend_object* pair,  zval* ret);
		static zval* get_second(zend_object* pair, zval* ret);
		*/

		zval_mgr key_;
		zval_mgr value_;

		void construct(zval_user p1, zval_user p2)
		{
			key_ = p1;
			value_ = p2;
		}
		
		zval_user first()
		{
			return key_;
		}

		zval_user second()
		{
			return value_;
		}

		zval_user key()
		{
			return key_;
		}

		zval_user value()
		{
			return value_;
		}
		
		double sum() const 
		{
			long a, b;
			a = zval_user(key_).zlong();
			b = zval_user(value_).zlong();
			return (a + b) / double(a);
		}
		/*
		zval_mgr key()
		{
			zval_mgr result;
			get_first(p_zobj_, result);
			return result;
		}

		zval_mgr value()
		{
			zval_mgr result;
			get_second(p_zobj_, result);
			return result;
		}
		*/

	public:
		static Pair_omg omg;
		
		VIRTUAL_ZOBJPTR


	};

}; // namespace

#endif