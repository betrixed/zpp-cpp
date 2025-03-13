#ifndef STRFNS_CPP
#define STRFNS_CPP

#include <string>
#include <sstream>


using namespace zpp;

/**
 * route_extract_paramsstring( string $s): array 
 */
/*
PHP_FUNCTION(Wcc_route_extract_params)
{
	zend_string* pattern;
	const char*  pat;
	size_t       pat_len;

	int 		 itemLen;
	const char*  itemStr;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(pattern)
	ZEND_PARSE_PARAMETERS_END();

	std::stringstream route_str;

	std::string_view regex_str;
	std::string_view variable;

	zval_own   tmp;

	const char* k_route = "([^/]*)";

	char ch = 0, prevCh = 0, xch = 0;
	int  bracketCount = 0,  parenCount = 0,  	foundPattern = 0;
	int  intermediate = 0,  matchCount = 0;		
	int  cursor = 0, 		cursorVar = 0;
	int  marker = 0,     	middle = 0;
	int  invalid = 0;
	long  tmpCount = 0;
	int  found = 0;

    zval_own     matches;
    zval_own     list2;

    matches.init_array(4);
    list2.init_array(2);

    pat_len = ZSTR_LEN(pattern);
    pat = ZSTR_VAL(pattern);

    htab_ptr ht_m(matches);
    htab_ptr ht_2(list2);

	for (cursor = 0; cursor < pat_len; cursor++)
	{
		ch = *(pat + cursor);
		if (parenCount == 0) {
			if (ch == '{') {
				if (bracketCount == 0) {
					marker = cursor+1;
					middle = 0;
					invalid = 0;
				}
				bracketCount++;
			}
			else if (ch == '}') {
				bracketCount--;
				if (middle > 0) {
					if (bracketCount==0) {
						matchCount++;
						itemStr = (pat + marker);
						itemLen = cursor - marker;
						for(cursorVar = 0; cursorVar < itemLen; cursorVar++) {
							xch = *(itemStr + cursorVar);
							if (xch==0) {
								break;
							}
							if ((cursorVar == 0) && !isalpha(xch)) {
								invalid = 1;
								break;
							}
							if (isalnum(xch) || xch=='-' || xch=='_' || xch==':') 
							{
								if (xch == ':') {
									// destroy any previous and init
									variable = std::string_view(itemStr,cursorVar);
									regex_str =  std::string_view(itemStr+cursorVar,itemLen-cursorVar);
								}
							}
							else {
								invalid = 1;
								break;
							}
						}
						if (!invalid) {
							tmpCount = matchCount;
							if ( (variable.size() > 0) 
								&& (regex_str.size() > 0) )
							{
								found = 0;
								for(char const &xch: regex_str)
								{
									if (xch==0) {
										break;
									}
									if (!found) {
										if (xch == '(') {
											found = 1;
										}
									} 
									else {
										if (xch == ')') {
											found = 2;
											break;
										}
									}
								}
								if (found != 2) {
									route_str << '(' << regex_str << ')';
								}
								else {
									route_str << regex_str;
								}
								// save it, variable as string key, tmpCount as value
								tmp.set((long)tmpCount);
								ht_m.set(variable.data(), variable.size(), tmp); 
							}
							else {
								route_str << k_route;
								tmp.set((long)tmpCount);
								ht_m.set(regex_str.data(), regex_str.size(), tmp); 
							}
						}
						else {
							route_str << '{' << regex_str << '}';
						}
						continue;
					}
				}

			}
		}
		if (bracketCount == 0) {
			if (ch == '(') {
				parenCount++;
			}
			else if (ch == ')')
			{
				parenCount--;
				if (parenCount == 0) {
					matchCount++;
				}
			}

		}
		if (bracketCount > 0) {
			middle++;
		}
		else {
			if ((parenCount==0) && (prevCh != '\\')) {
				switch(ch) {
					case '.':
					case '+':
					case '|':
					case '#':
						route_str << '\\';
						break;
					default:
						break;
				}
			}
			route_str << ch;
			prevCh = ch;
		}
	}

	std::string route = route_str.str();
	//printf("Route Params %s\n", route.data());

	// push route match string
	ht_2.push_back(route.data(), route.size());
	// push index of parameters
	ht_2.push_back(matches);

	list2.move_zv(return_value);
}
*/

/* 
 *	Emulate debug_zval_dump, except accumulate.
 *	
 */


void 
zpp_dump(zval_user zu, int level)
{
	dump_info::dump(zu, level);
}



/** Only does one character seperator */
void phiz_uncamel(zval* return_value, const zend_string *src, const zend_string *sep)
{
	smart_str uncamel_str = {0};
	const char *marker;
	const char* psep;
	int sep_len = 0;
	unsigned int i;
	int src_len;
	char ch, sepch;

	ZVAL_UNDEF(return_value);
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
	smart_str_0(&uncamel_str);

	if (uncamel_str.s) {
		RETURN_STR(uncamel_str.s);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/**
 * Convert dash/underscored texts returning camelized
 * (an optional delimiter can be specified as character-mask as for ltrim)
 */
void phiz_camel(zval* return_value, const zend_string *src, const zend_string *sep)
{
	smart_str camel_str = {0};
	const char* marker;
	const char* psep;
	int   sep_len = 0;
	int i, len, found = 1;
	char ch;

	ZVAL_UNDEF(return_value);
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

	smart_str_0(&camel_str);

	if (camel_str.s) {
		RETURN_STR(camel_str.s);
	} else {
		RETURN_EMPTY_STRING();
	}
}

PHP_FUNCTION(Wcc_debug_zpp_dump) 
{
	zval* value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	zpp_dump(zval_user(value), 0);
}

PHP_FUNCTION(Wcc_str_uncamel) {
	zend_string* src = NULL;
	zend_string* sep = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(src)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(sep)
	ZEND_PARSE_PARAMETERS_END();

	phiz_uncamel(return_value, src, sep );
}

PHP_FUNCTION(Wcc_str_camel) {
	zend_string* src = NULL;
	zend_string* sep = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(src)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(sep)
	ZEND_PARSE_PARAMETERS_END();

	phiz_camel(return_value, src, sep );
}

/*
static zval_mgr global_ref(const char* gname)
{
	zval_mgr gval(GLOBALS[gname].value(),true);
	return gval;
}
*/
PHP_FUNCTION(Wcc_test_wcc)
{	




	/**

	preg regex("/\\G([-A-Z_a-z0-9]+)/u");
	zstr_temp    test("[[fruit.blah]]");
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
	
	//* pass reference to array
	zval_own htabref(scopy, true);
	showmem("scopy ref", htabref);

	//* get htab_ptr and write to the reference

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

	zstr_buffer ss;

	zend_printf("sizeofs  base=%ld, ptr = %ld, pass = %ld, own = %ld, mod = %ld\n", 
		sizeof(zstr_base), sizeof(zstr_ptr), sizeof(zstr_pass), sizeof(zstr_own)
		, sizeof(zstr_buffer)); 
	ss << "stuff";

	s6 = std::move(ss);

	showstr("s6 stuff", s6);
	*/
}

#endif//strfns.cpp