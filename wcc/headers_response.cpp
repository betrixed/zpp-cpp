#ifndef HEADERS_RESPONSE_CPP
#define HEADERS_RESPONSE_CPP

// not used anymore, excised from global_response
#ifndef HEADERS_RESPONSE_H
#include "headers_response.h"
#endif

#ifndef RESPONSE_ARGINFO_H
#define RESPONSE_ARGINFO_H
extern "C" {
	#include "stub/global_response_arginfo.h"	
};
#endif

namespace wcc {

using namespace zpp;

Headers_mgr Headers::omg;

bool Headers::send()
{

	val_rc issent = wis->headers_sent.callme();

	if (issent.isTrue())
	{
		return false;
	}

	htab_walk wk;
	auto hkey = wk.key();
	auto hvalue = wk.value();

	val_rc true_arg(true);

	for(wk.start(headers_); wk.ok(); wk.next())
	{
		str_rc harg = hkey.zstr();
		if (!hvalue.isNull())
		{
			str_buf  d2;

			d2 << harg << ": " << hvalue.zstr();

			val_rc arg1 = d2.zstr();
			wis->header_key.callme(arg1,true_arg);
		}
		else {
			if ( (harg.find(':') >= 0) || (harg.subview(0,5)=="HTTP/"))
			{
				wis->header_key.callme(hkey,true_arg);
			}
			else 
			{
				str_buf  d1;
				d1 << harg << ": ";
				val_rc arg = d1.zstr();
				wis->header_key.callme(arg,true_arg);
			}
		}
	}
	return true;
}


void Headers::Headers::setRaw(str_ptr zs)
{
	val_rc valnull;
	//showstr("set raw as key ", zs);

	headers_.set(zs, valnull);
}


}; //namespace wcc

using namespace wcc;
using namespace zpp;

ZEND_METHOD(Wcc_Headers, get)
{
	str_ptr name;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Headers>(ZEND_THIS);
	str_rc result = cobj->get(name);

	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Headers, has)
{
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Headers>(ZEND_THIS);
	bool result = cobj->has(name);
	RETURN_BOOL(name);
}

ZEND_METHOD(Wcc_Headers, remove)
{
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Headers>(ZEND_THIS);
	cobj->remove(name);

	obj_rc result(ZEND_THIS);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Headers, reset)
{
	ZEND_PARSE_PARAMETERS_NONE();

	auto cobj = zval_toc<Headers>(ZEND_THIS);
	cobj->reset();
	obj_rc result(ZEND_THIS);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Headers, send)
{
	ZEND_PARSE_PARAMETERS_NONE();

	auto cobj = zval_toc<Headers>(ZEND_THIS);
	bool result = cobj->send();

	RETURN_BOOL(result);
}

ZEND_METHOD(Wcc_Headers, set)
{
	zend_string* name;
	zend_string* value;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_STR(name)
	Z_PARAM_STR(value)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Headers>(ZEND_THIS);
	cobj->set(name, value);

	obj_rc result(ZEND_THIS);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Headers, setRaw)
{
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Headers>(ZEND_THIS);
	cobj->setRaw(name);

	obj_rc result(ZEND_THIS);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Headers, toArray)
{
	ZEND_PARSE_PARAMETERS_NONE();

	auto cobj = zval_toc<Headers>(ZEND_THIS);
	htab_ptr result = cobj->toArray();

	result.copy_zv(return_value);
}

PHP_MINIT_FUNCTION(Wcc_Headers_reg)
{
	auto ce = register_class_Wcc_Headers();

	Headers::omg.classEntry(ce);

	return SUCCESS;
}
//headers_response.cpp
#endif

