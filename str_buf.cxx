#ifndef STR_BUF_CPP
#define STR_BUF_CPP

/**
 * @file zpp/str_buf.cpp
 * @author Michael Rynn <michael.rynn.500@gmail.com>
 * @brief str_buf class, string buffer with zend_string memory layout.	
 * @copyright Copyright (c) 2025 Michael Rynn
 * @license BSD 3-Clause License
 */


#ifndef STR_BUF_H
#include "str_buf.h"
#endif

#ifndef STR_PTR_H
#include "str_ptr.h"
#endif

#ifndef VAL_PTR_H
#include "val_ptr.h"
#endif

#include <stdarg.h>

namespace zpp {

void str_buf::initbuf()
{
	buf.s = nullptr;
	buf.a = 0;
	final_ = str_ptr::empty_str();
}

str_buf::str_buf()
{
	initbuf();
}

zend_string* //static
str_buf::init_zs(const char* c, size_t slen)
{
	return zend_string_init(c, slen, 0);
}

void
str_buf::lose()
{
	if (buf.s)
	{
		smart_str_free(&buf);
		initbuf();
	}
}

str_buf::~str_buf() 
{
	lose();
}

str_buf::str_buf(zval *v) : str_out()
{
	initbuf();
	str_rc temp = val_ptr(v).to_zstr();
	zend_string* s = temp;
	if (s) {
		append(ZSTR_VAL(s), ZSTR_LEN(s));
	}
}

str_buf::str_buf(zend_string* w) : str_out()
{
	initbuf();
	str_out::append(w);	
}

str_buf::str_buf(const std::string_view& cs) : str_out()
{
	initbuf();
	auto slen = cs.size();
	if (slen) 
	{
		append(cs.data(), slen);
	}	
}

str_buf::str_buf (const char* c, size_t slen) : str_out() 
{
	initbuf();
	if (slen)
	{
		append(c, slen);
	}
}

str_buf::str_buf (const char* c) : str_out()
{
	initbuf();
	auto slen = strlen(c);
	if (slen)
	{
		append(c, slen);
	}
}

str_buf::str_buf(const rqstring& cs) : str_out()
{
	initbuf();
	auto slen = cs.size();
	if (slen) {
		append(cs.data(), slen);
	}
}

void str_buf::append(char c)
{
	smart_str_appendc_ex(&buf, c, 0);	
}


void
str_buf::append(const char* c, size_t slen)
{
	if (!c || !slen) {
		return;
	}
	smart_str_appendl_ex(&buf, c, slen, 0);
}


void str_buf::reset()
{
	lose();
}

str_buf& 
str_buf::operator=(const char* c)
{
	lose();
	append(c, strlen(c));
	return *this;
}	


const char* 
str_buf::data() const {
	if (buf.s) {
		smart_str_0((smart_str*) &buf);
		return ZSTR_VAL(buf.s);
	}
	else {
		return str_ptr::empty;
	}
}

size_t 
str_buf::size() const 
{
	if (buf.s) {
		return ZSTR_LEN(buf.s);
	}
	else {
		return 0;
	}
}


// return rqstring duplicate of the buffer far, as std::string type

rqstring
str_buf::str()
{
	if (buf.s)
	{
		return rqstring(data(), size());
	}
	else {
		return rqstring(); // don't know what this points to
	}
}




// extract returns final zend string reallocated
// with null terminator, and also nulls the internal 
// zend_string s.
// result will need to be "adopted"

str_ptr
str_buf::zstr()
{
	if (buf.s)
	{
		zend_string* value = smart_str_extract_ex(&buf, 0);
		final_.adopt(value); // erases old value
		//showstr("zstr adopt", value);
	}
	else {
		//TODO: EMPTY or null?
		// old value remains
	}
	return final_;
}
/*
zend_string*
str_buf::finalize()
{
	if (buf.s)
	{
		return smart_str_extract_ex(&buf, 0);
	}
	return final_;
}
*/
// this doesn't seem to be useful.
/*
str_buf& 
str_buf::endnull() {
	smart_str_0((smart_str*)(this));
	return *this;
}
*/

std::string_view 
str_buf::vstr() const
{
	if (buf.s)
	{
		return std::string_view(ZSTR_VAL(buf.s), ZSTR_LEN(buf.s));
	}
	else {
		return final_.vstr();
	}
}

void 
str_buf::printf(const char *format, ...)
{
	std::va_list args;
	va_start(args, format);
	php_printf_to_smart_str(&buf, format, args);
	va_end(args);
}
		
		
};//namespace
//str_buf.cpp
#endif
