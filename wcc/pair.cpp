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
	 obj_ptr self(vobj());

	 self.property(PairSI.one, p1);
	 self.property(PairSI.two, p2);

	 one_ = self.property_ptr(PairSI.one); 
	 two_ = self.property_ptr(PairSI.two);
}

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
