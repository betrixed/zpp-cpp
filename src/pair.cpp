#ifndef WCC_PAIR_CPP
#define WCC_PAIR_CPP

#ifndef WCC_PAIR_H
#include "pair.h"
#endif

namespace wcc {
	base_obj_mgr<Pair> Pair::omg;
};

using namespace wcc;

ZEND_METHOD(Wcc_Pair, __construct)
{
	zval* p1;
	zval* p2;

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

	zval_user result = cobj->first();
	result.return_zv(return_value);
}

ZEND_METHOD(Wcc_Pair, second)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Pair* cobj = zval_toc<Pair>(ZEND_THIS);

	zval_user  result = cobj->second();
	result.return_zv(return_value);
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