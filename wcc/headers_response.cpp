#ifndef HEADERS_RESPONSE_CPP
#define HEADERS_RESPONSE_CPP

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
	Hmap_mgr<Headers> Headers::omg;

bool Headers::send()
{
	//zend_printf("headers_sent yet?\n");
	zval_mgr issent = wis->headers_sent.callme();
	//showmem("issent", issent);

	if (issent.isTrue())
	{
		return false;
	}
	//zend_printf("Not sent yet\n");

	htab_walk wk;
	auto& hkey = wk.key();
	auto& hvalue = wk.value();

	zval_mgr true_arg(true);

	for(wk.start(headers_); wk.ok(); wk.next())
	{
		zstr_mgr harg = hkey.zstr();
		if (!hvalue.isNull())
		{
			zstr_buffer  d2;

			d2 << harg << ": " << hvalue.zstr();

			zval_mgr arg1(std::move(d2));
			wis->header_key.callme(arg1,true_arg);
		}
		else {
			if ( (harg.find(':') >= 0) || (harg.subview(0,5)=="HTTP/"))
			{
				wis->header_key.callme(hkey,true_arg);
			}
			else 
			{
				zstr_buffer  d1;
				d1 << harg << ": ";
				zval_mgr arg(std::move(d1));
				wis->header_key.callme(arg,true_arg);
			}
		}
	}
	return true;
}


void Headers::Headers::setRaw(zstr_user zs)
{
	zval_mgr valnull;
	//showstr("set raw as key ", zs);

	headers_.set(zs, valnull);
}


}; //namespace wcc

ZEND_METHOD(Wcc_Headers, get)
{
	zstr_user name;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Headers>(ZEND_THIS);
	zstr_mgr result = cobj->get(name);

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

	zobj_mgr result(ZEND_THIS);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Headers, reset)
{
	ZEND_PARSE_PARAMETERS_NONE();

	auto cobj = zval_toc<Headers>(ZEND_THIS);
	cobj->reset();
	zobj_mgr result(ZEND_THIS);
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

	zobj_mgr result(ZEND_THIS);
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

	zobj_mgr result(ZEND_THIS);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Headers, toArray)
{
	ZEND_PARSE_PARAMETERS_NONE();

	auto cobj = zval_toc<Headers>(ZEND_THIS);
	const htab_mgr& result = cobj->toArray();

	result.return_zv(return_value);
}

PHP_MINIT_FUNCTION(Wcc_Headers_reg)
{
	auto ce = register_class_Wcc_Headers();

	Headers::omg.classEntry(ce);

	return SUCCESS;
}
//headers_response.cpp
#endif

