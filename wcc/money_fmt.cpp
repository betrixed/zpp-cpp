#ifndef MONEY_FMT_CPP
#define MONEY_FMT_CPP

#ifndef MONEY_FMT_H
#include "money_fmt.h"
#endif

#ifndef HTMLGET_ARGINFO_H
#define HTMLGET_ARGINFO_H
extern "C" {
	#include "stub/htmlgem_arginfo.h"
}

#endif

extern "C" {
	#include "ext/standard/php_math.h"
}
namespace wcc {

	class MFInit : public state_init
	{
	public:
		MFInit() : state_init() {}

		zstr_intern lang;
		zstr_intern symbol;
		zstr_intern iformat;
		zstr_intern numfmt_create;
		zstr_intern currency_key;
		zstr_intern intl_currency;
		zstr_intern getsymbol;

		virtual void init()
		{
			lang = "language";
			symbol = "symbol";
			iformat = "formatter";
			numfmt_create = "numfmt_create";
			currency_key = "CURRENCY";
			intl_currency = "INTL_CURRENCY_SYMBOL";
			getsymbol = "getsymbol";
		}
	};

	MFInit  MFI;

	base_obj_mgr<MoneyFmt> MoneyFmt::omg;


//easier than getting them from Zend constants
//#include <unicode/unum.h>
enum {
	 UNUM_CURRENCY=2,
	 UNUM_INTL_CURRENCY_SYMBOL = 9,
};

void 
MoneyFmt::debug_info(htab_write hw)
{
	hw.set(MFI.lang,   lang_str_);
	hw.set(MFI.symbol, money_sym_);
	hw.set(MFI.iformat, money_fmt_);
}

void 
MoneyFmt::construct(zstr_user slang)
{
	lang_str_ = slang;

	zval_mgr lang(slang);
	
	zval_mgr currency_flag(UNUM_CURRENCY); 

	zval_mgr symbol_flag(UNUM_INTL_CURRENCY_SYMBOL);  

	money_fmt_ = FCall2(MFI.numfmt_create).call(lang, currency_flag);

	money_sym_ = zobj_user(money_fmt_).call(MFI.getsymbol, symbol_flag);
}

zstr_mgr 
MoneyFmt::formatNoSym(zval_user value)
{
	zstr_mgr result;

	double dval = value.zdouble();
	zend_string* fs = _php_math_number_format(dval / 100.0, 2, '.', ',');
	result.adopt(fs);
	return result;
}

zstr_user MoneyFmt::symbol() const
{
	return money_sym_;
}

zstr_user MoneyFmt::language() const
{
	return lang_str_;
}

zstr_mgr 
MoneyFmt::format(zval_user value)
{
	zstr_buffer result;

	result << money_sym_ << ' ' << formatNoSym(value);

	return result;
}

//money_fmt.cpp
}; // namespace wcc

ZEND_METHOD(Wcc_Money, __construct)
{
	zend_string* slang;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(slang)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<MoneyFmt>(ZEND_THIS);

	cobj->construct(slang);


}

ZEND_METHOD(Wcc_Money, formatNoSym)
{
	zval* value;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<MoneyFmt>(ZEND_THIS);
	zstr_mgr result = cobj->formatNoSym(value);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_Money, format)
{
	zval* value;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<MoneyFmt>(ZEND_THIS);
	zstr_mgr result = cobj->format(value);
	result.move_zv(return_value);
	
}

ZEND_METHOD(Wcc_Money, symbol)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<MoneyFmt>(ZEND_THIS);
	zstr_user result = cobj->symbol();
	result.return_zv(return_value);
}

ZEND_METHOD(Wcc_Money, language)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<MoneyFmt>(ZEND_THIS);
	zstr_user result = cobj->language();
	result.return_zv(return_value);
}

PHP_MINIT_FUNCTION(Wcc_Money_reg)
{
	auto ce = register_class_Wcc_Money();

	MoneyFmt::omg.classEntry(ce);

	return SUCCESS;
}
#endif