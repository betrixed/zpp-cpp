#ifndef STR_BUF_CPP
#define STR_BUF_CPP

#ifndef STR_BUF_H
#include "str_buf.h"
#endif

#ifndef STR_PTR_H
#include "str_ptr.h"
#endif

#ifndef VAL_PTR_H
#include "val_ptr.h"
#endif

namespace zpp {

void str_buf::initbuf()
{
	buf.s = nullptr;
	buf.a = 0;
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

str_buf::str_buf(const std::string& cs) : str_out()
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


// inalizes zend_string buffer, 
// return buffer copy as C-string
// and releases/destroys the zend_string
std::string 
str_buf::str()
{
	zend_string* xs = zstr(); // reassign buffer back here.
	std::string result(ZSTR_VAL(xs), ZSTR_LEN(xs));
	zend_string_release(xs);
	return result;
}



// extract returns final zend string reallocated
// with null terminator, and also nulls the internal 
// zend_string s.
// result will need to be "adopted"

str_rc
str_buf::zstr()
{
	str_rc result;

	if (buf.s)
	{
		zend_string* value = smart_str_extract_ex(&buf, 0);
		result.adopt(value);
		//showstr("zstr adopt", value);
	}
	else {
		//TODO: EMPTY or null?

		result = str_ptr::empty_str();
		//showstr("zstr empty", result);
	}
	return result;
}

zend_string*
str_buf::finalize()
{
	if (buf.s)
	{
		return smart_str_extract_ex(&buf, 0);
	}
	return (zend_string*) nullptr;
}

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
		return std::string_view(str_ptr::empty, 0);
	}
}

};//namespace
//str_buf.cpp
#endif
