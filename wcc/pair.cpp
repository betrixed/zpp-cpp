#ifndef WCC_PAIR_CPP
#define WCC_PAIR_CPP

#ifndef WCC_PAIR_H
#include "pair.h"
#endif

#ifndef WCC_PAIR_ARGINFO
#define WCC_PAIR_ARGINFO
extern "C" {
	#include "stub/pair_arginfo.h"
};
#endif

namespace wcc {

base_obj_mgr<Pair> Pair::omg;

//! static class interned strings
class PairInit : public state_init {
public:
	str_intern one;
	str_intern two;

	void init() override
	{
		one = "one";
		two = "two";
	}
};

PairInit PairSI;

void Pair::construct(val_ptr p1, val_ptr p2)
{
	 obj_ptr self(self_);

	 self.property(PairSI.one, p1);
	 self.property(PairSI.two, p2);
}

val_ptr 
Pair::first() const
{
	return obj_ptr(self_).property_ptr(PairSI.one);
}

val_ptr 
Pair::second() const
{
	return obj_ptr(self_).property_ptr(PairSI.two);
}

val_ptr 
Pair::key() const
{
	return obj_ptr(self_).property_ptr(PairSI.one);
}

val_ptr 
Pair::value() const
{
	return obj_ptr(self_).property_ptr(PairSI.two);
}

}; // namespace wcc

using namespace wcc;

ZEND_METHOD(Wcc_Pair, __construct)
{
	zarg_rd args(execute_data);

	val_ptr a1 = args.option(0);
	val_ptr a2 = args.option(1);

	if (a1.is_nullptr())
	{
		a1 = val_rc::null_value_ptr();
	}
	if (a2.is_nullptr())
	{
		a2 = val_rc::null_value_ptr();
	}

	if (!args.throw_errors(__FUNCTION__))
	{
		Pair* cobj = zval_toc<Pair>(ZEND_THIS);

		cobj->construct(a1,a2);
	}

	
}

ZEND_METHOD(Wcc_Pair, first)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Pair* cobj = zval_toc<Pair>(ZEND_THIS);
	val_ptr result = cobj->first();
	result.copy_zv(return_value);

}

ZEND_METHOD(Wcc_Pair, second)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Pair* cobj = zval_toc<Pair>(ZEND_THIS);
	val_ptr result = cobj->second();
	result.copy_zv(return_value);
}

ZEND_METHOD(Wcc_Pair, key)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Pair* cobj = zval_toc<Pair>(ZEND_THIS);

	val_ptr result = cobj->first();
	result.copy_zv(return_value);

}

ZEND_METHOD(Wcc_Pair, value)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Pair* cobj = zval_toc<Pair>(ZEND_THIS);
	val_ptr result = cobj->second();
	result.copy_zv(return_value);
}

ZEND_METHOD(Wcc_Pair, test_calc)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Pair* cobj = zval_toc<Pair>(ZEND_THIS);
	ZVAL_DOUBLE(return_value, cobj->test_calc());
	//cobj->value_.copy_zv(return_value);
}

PHP_MINIT_FUNCTION(wcc_pair_d)
{
	//WcR_ce = wcc_class_reg("Wc\\Route", class_Wcc_Route_methods);
	auto ce = register_class_Wcc_Pair();

	Pair::omg.classEntry(ce);

	STATE_INIT_ADD(PairSI)
	
	return SUCCESS;
}

//pair.cpp
#endif
