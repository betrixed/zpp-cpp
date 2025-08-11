#ifndef WCC_PAIR_CPP
#define WCC_PAIR_CPP

#ifndef WCC_PAIR_H
#include "pair.h"
#endif

namespace wcc {

base_obj_mgr<Pair> Pair::omg;

class PairInit : public state_init {
public:
	zstr_intern one;
	zstr_intern two;

	void init() override
	{
		one = "one";
		two = "two";
	}
};

PairInit PairSI;

void Pair::construct(val_ptr p1, val_ptr p2)
{
	 obj_ptr self(vobj());

	 self.property(PairSI.one, p1);
	 self.property(PairSI.two, p2);

	 one_ = self.property_ptr(PairSI.one); 
	 two_ = self.property_ptr(PairSI.two);
}
/*
zval* 
property_ptr(zend_object* zobj, zend_string* name)
{
	Pair* p = zobj_toc<Pair>(zobj);

	if (zs_cmp(PairSI.key, name)==0 )
	{
		return p->key_;
	}
	if (zs_cmp(PairSI.value, name)==0)
	{
		return p->value_;
	}
	return nullptr;
}

zval* 
Pair::get_property_ptr_ptr(zend_object *zobj, zend_string *name, 
						int type, void **cache_slot)
{
	return property_ptr(zobj, name);
}

int //static 
Pair::has_property(zend_object *zobj, zend_string *name, 
			             int has_set_exists, void **cache_slot)
{
	return (property_ptr(zobj, name) != nullptr);
}

void //static 
Pair::unset_property(zend_object *zobj, zend_string *name, void **cache_slot)
{
	zval* ptr = property_ptr(zobj, name);
	if (ptr)
	{
		if (val_rc::try_decref(ptr))
		{
			ZVAL_NULL(ptr);
		};
	}
}

zval* //static 
Pair::read_property(
	zend_object *object, zend_string *member, 
	int type, void **cache_slot, zval *rv)
{
	return property_ptr(object, member);
}

zval* //static 
Pair::write_property(zend_object *zobj, zend_string *name, 
						zval *value, void **cache_slot)
{
	zval* ptr = property_ptr(zobj, name);
	if (ptr)
	{
		val_rc::try_decref(ptr);
		ZVAL_COPY(ptr, value);
		val_rc::try_addref(ptr);
	}
	return ptr;
}

*/

/*
void //static
Pair::set_first(zend_object* pair, zval* val)
{
	obj_ptr myobj(pair);
	myobj.property(PairSI.first, val);	
}

 
void //static
Pair::set_second(zend_object* pair, zval* val)
{
	obj_ptr myobj(pair);
	myobj.property(PairSI.second, val);
}

zval*  //static
Pair::get_first(zend_object* pair, zval* ret)
{
	obj_ptr myobj(pair);
	return myobj.property_get(PairSI.first, ret);
}


zval*  //static
Pair::get_second(zend_object* pair,  zval* ret)
{
	obj_ptr myobj(pair);
	return myobj.property_get(PairSI.second, ret);
}
*/

/*
void
Pair::Pair_omg::init_class_fn()
{
	mydef::init_class_fn();

	handlers_.read_property = Pair::read_property;
	handlers_.write_property = Pair::write_property;
	handlers_.read_property = Pair::read_property;
	handlers_.has_property = Pair::has_property;
	handlers_.get_property_ptr_ptr = Pair::get_property_ptr_ptr;

//zend_declare_typed_property
	//mydef::class_entry_->ce_flags |= ZEND_ACC_FINAL;

	zend_type dtype =  {nullptr, 0};

	val_rc null_init;

	//ZEND_TYPE_INIT_MASK(MAY_BE_ANY);

	class_data cinit(mydef::class_entry_);
	cinit.typed_property(PairSI.first, null_init, dtype);
	cinit.typed_property(PairSI.second, null_init, dtype);

	//ce, name, property, access_type, doc_comment, (zend_type) ZEND_TYPE_INIT_NONE(0)
	// zend_declare_typed_property(mydef::class_entry_, PairSI.first, null_init,
									ZEND_ACC_PUBLIC, nullptr, (zend_type) ZEND_TYPE_INIT_NONE(0));
	// zend_declare_typed_property(mydef::class_entry_, PairSI.second, null_init,
									ZEND_ACC_PUBLIC, nullptr, (zend_type) ZEND_TYPE_INIT_NONE(0)); 

}
	*/



}; // namespace wcc

using namespace wcc;

ZEND_METHOD(Wcc_Pair, __construct)
{
	zval* p1 = nullptr;
	zval* p2 = nullptr;

	ZEND_PARSE_PARAMETERS_START(0,2)
	Z_PARAM_OPTIONAL
	Z_PARAM_ZVAL(p1)
	Z_PARAM_ZVAL(p2)
	ZEND_PARSE_PARAMETERS_END();

	Pair* cobj = zval_toc<Pair>(ZEND_THIS);

	cobj->construct(p1,p2);
}

ZEND_METHOD(Wcc_Pair, first)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Pair* cobj = zval_toc<Pair>(ZEND_THIS);
	cobj->one_.return_zv(return_value);

}

ZEND_METHOD(Wcc_Pair, second)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Pair* cobj = zval_toc<Pair>(ZEND_THIS);
	cobj->two_.return_zv(return_value);
}

ZEND_METHOD(Wcc_Pair, key)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Pair* cobj = zval_toc<Pair>(ZEND_THIS);
	cobj->one_.return_zv(return_value);

}

ZEND_METHOD(Wcc_Pair, value)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Pair* cobj = zval_toc<Pair>(ZEND_THIS);
	cobj->two_.return_zv(return_value);
}

ZEND_METHOD(Wcc_Pair, sum)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Pair* cobj = zval_toc<Pair>(ZEND_THIS);
	ZVAL_DOUBLE(return_value, cobj->sum());
	//cobj->value_.return_zv(return_value);
}

PHP_MINIT_FUNCTION(wcc_pair_d)
{
	//WcR_ce = wcc_class_reg("Wc\\Route", class_Wcc_Route_methods);
	auto ce = register_class_Wcc_Pair();

	Pair::omg.classEntry(ce);

	return SUCCESS;
}

//pair.cpp
#endif
