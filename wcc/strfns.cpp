#ifndef STRFNS_CPP
#define STRFNS_CPP

#include "strfns.h"

#ifndef WCC_ARGINFO_H
#define WCC_ARGINFO_H
extern "C" {
#include "stub/wcc_arginfo.h"
}
#endif

zend_class_entry* wcc_str_ce;

using namespace zpp;


/* 
 *	Emulate debug_zval_dump, except accumulate.
 *	
 */


void 
zpp_dump(val_ptr zu, int level)
{
	dump_info::dump(zu, level);
}

str_rc phiz_intern(str_ptr s)
{
	return str_intern(s.data());
}

/** Only does one character seperator */
str_rc 
phiz_uncamel(const zend_string *src, const zend_string *sep)
{
	smart_str uncamel_str = {0};
	const char *marker;
	const char* psep;
	int sep_len = 0;
	int i;
	int src_len;
	char ch, sepch;

	if (sep!=NULL) {
		psep = ZSTR_VAL(sep);
		sep_len = ZSTR_LEN(sep);
	}
	if (sep_len == 0) {
		sepch = '_';
	}
	else {
		sepch = *psep;
	}

	marker = ZSTR_VAL(src);
	src_len = ZSTR_LEN(src);

	for (i = 0; i < src_len; i++) {
		ch = *marker;
		if (ch == '\0') {
			break;
		}
		if (ch >= 'A' && ch <= 'Z') {
			if (i > 0) {
				smart_str_appendc(&uncamel_str, sepch);
			}
			// lower case
			smart_str_appendc(&uncamel_str, ch + 32);
		} else {
			smart_str_appendc(&uncamel_str, ch);
		}
		marker++;
	}

	str_rc result;

	if (uncamel_str.s) {
		zend_string* sse = smart_str_extract(&uncamel_str);
		result.adopt(sse);
	}
	return result;
}
/**
 * Convert dash/underscored texts returning camelized
 * (an optional delimiter can be specified as character-mask as for ltrim)
 */
str_rc 
phiz_camel(const zend_string *src, const zend_string *sep)
{
	smart_str camel_str = {0};
	const char* marker;
	const char* psep;
	int   sep_len = 0;
	int i, len, found = 1;
	char ch;

	marker = ZSTR_VAL(src);
	len    = ZSTR_LEN(src);


	if (sep != NULL) {
		psep = ZSTR_VAL(sep);
		sep_len = ZSTR_LEN(sep);
	}
	if (sep_len == 0) {
		psep = "_-";
		sep_len = 2;
	}

	for (i = 0; i < len; i++) {
		ch = marker[i];

		if (memchr(psep, ch, sep_len)) {
			found = 1;
			continue;
		}
		if (found == 1) {
			smart_str_appendc(&camel_str, toupper(ch));
			found = 0;
		} else {
			smart_str_appendc(&camel_str, tolower(ch));
		}
	}
	str_rc result;
	if (camel_str.s) {
		zend_string* sse = smart_str_extract(&camel_str);
		result.adopt(sse);
	}
	return result;
}

PHP_FUNCTION(Wcc_debug_zpp_dump) 
{
	zval* value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	zpp_dump(val_ptr(value), 0);
}

;

ZEND_METHOD(Wcc_Str, uncamel) {
	zend_string* src = NULL;
	zend_string* sep = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(src)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(sep)
	ZEND_PARSE_PARAMETERS_END();

	str_rc result = phiz_uncamel(src, sep );
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Str, camel) {
	zend_string* src = NULL;
	zend_string* sep = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(src)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(sep)
	ZEND_PARSE_PARAMETERS_END();

	str_rc result = phiz_camel(src, sep );
	result.move_zv(return_value);
}

PHP_FUNCTION(Wcc_str_intern)
{
	zend_string* src = NULL;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(src)
	ZEND_PARSE_PARAMETERS_END();

	str_rc value = phiz_intern(src);

	value.move_zv(return_value);
}
/*
static val_rc global_ref(const char* gname)
{
	val_rc gval(GLOBALS[gname].value(),true);
	return gval;
}
*/

// may have occasional real test code
PHP_FUNCTION(Wcc_test_wcc)
{	
	/**

	preg regex("/\\G([-A-Z_a-z0-9]+)/u");
	str_temp    test("[[fruit.blah]]");
	zend_long    offset = 2;

	int count = regex.matches(test,offset);

	htab_own captures = regex.captures();

	zend_printf("match count = %ld, captures = %ld\n", count, captures.size());

	captures.show_data("caught");


	zval* avalue;

	wpp_intern& wi = *base_d::wis;

	showstr("hidden", wi.hidden);
	
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ZVAL(avalue);

	ZEND_PARSE_PARAMETERS_END();
	showmem("avalue arg", avalue);

	//zval_own gserver = GLOBALS["_SERVER"].value();
	//showmem("_SERVER copy", gserver);

	htab_own gcopy = GLOBALS["_SERVER"].value();
	showarray("_SERVER gcopy", gcopy);
	gcopy.cow_violate();

	zval_own gref(gcopy, true);

	zval_ptr xarg(avalue);
	showmem("xarg1", xarg);
	if (xarg.isReference())
	{
		xarg = xarg.referent();
		showmem("xarg2", xarg);
		htab_ptr ht = xarg.cow_array(); 
		showmem("xarg3 post cow", xarg);
		zstr_make<false> astr("String Value");
		ht.set("key", astr);	
	}
	xarg.return_zv(return_value);
	

	showmem("global_ref", gref);

	htab_ptr sptr = gref.zarray();
	showarray("_SERVER ptr", sptr);

	htab_own scopy = gref.zarray();
	showarray("_SERVER own", scopy);
	
	// pass reference to array
	zval_own htabref(scopy, true);
	showmem("scopy ref", htabref);

	// get htab_ptr and write to the reference

	htab_ptr writer(htabref);

	showarray("writer?", writer);

	zstr_make<false> testkey("TEST_KEY");

	writer.set(testkey, "Some Value");

	showarray("writer2", writer);

	zstr_own text = writer[testkey];

	zend_printf("added %s\n ", text.data());

	htab_own myarray;
	myarray.set(testkey, "First Value");
	showarray("myarray", myarray);

	zval_own direct(myarray, true);
	showmem("zval_own direct", direct);

	htab_ptr hdirect(direct);
	hdirect.cow_violate();

	showarray("hdirect", hdirect);
	hdirect.set(testkey, "Violate COW Value");

	hdirect.cow_violate(false);
	showarray("hdirect2", hdirect);
	//hdirect.set(testkey, "Second");

	// make array for self and become reference
	zval_own  share;

	share.init_array();

	share.make_ref();

	zval_own two(share, true);

	htab_ptr h2(two);
	h2.set(testkey, "Second");
	showmem("two make_ref", two);

	zstr_make<false> str1("String_1");
	showstr("str1", str1);

	zstr_make<true> str2("String 2");

	zstr_own s3 = str1;
	//zstr_own s4 = str2;

	showstr("s3", s3);
	//showstr("s4", s4);

	zstr_own s5 = s3.to_upper();

	showstr("s5", s5);
	showstr("s3", s3);

	zstr_own s6 = s3.strtr("_", " ");
	showstr("s6", s6);

	zstr_own p1(str1.ptr());

	showstr("p1", p1);

	str_buf ss;

	zend_printf("sizeofs  base=%ld, ptr = %ld, pass = %ld, own = %ld, mod = %ld\n", 
		sizeof(zstr_base), sizeof(zstr_ptr), sizeof(zstr_pass), sizeof(zstr_own)
		, sizeof(str_buf)); 
	ss << "stuff";

	s6 = std::move(ss);

	showstr("s6 stuff", s6);
	*/
}

PHP_MINIT_FUNCTION(Strfns_reg)
{
	wcc_str_ce = register_class_Wcc_Str();
	return SUCCESS;
}
#endif//strfns.cpp