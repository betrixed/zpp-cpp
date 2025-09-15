#ifndef STR_OUT_CPP
#define STR_OUT_CPP

/**
 * @file str_out.cxx
 * @author Michael Rynn <michael.rynn.500@gmail.com>
 * @brief str_out class, string buffer with zend_string memory layout.	
 * @copyright Copyright (c) 2025 Michael Rynn
 * @license Artistic License 2.0
 */

#ifndef STR_OUT_H
#include "str_out.h"
#endif

#ifndef STR_PTR_H
#include "str_ptr.h"
#endif

#ifndef VAL_PTR_H
#include "val_ptr.h"
#endif


namespace zpp {


fm_endl endl;

str_out& 
str_out::operator<<(const iform& form)
{
	nf_ = form;
	return *this;
}

str_out& 
str_out::operator<<(const fm_endl& el)
{
	append('\n');
	return *this;
}

str_out& 
str_out::operator<<(void* vp)
{
	zend_string* pf = strpprintf(0,"%lx", (unsigned long) vp);
	append(pf);
	zend_string_release(pf);
	return *this;
}

void 
str_out::quote_name(zend_string* name)
{
	append('"');
	if (name) { append(name); }
	append('"');
}

void 
str_out::quote_name(const char* name)
{
	append('"');
	if (name) { append(name,strlen(name)); }
	append('"');
}

str_out& 
str_out::operator<<(val_ptr zv)
{
	return operator<<((zval*) zv);
}
str_out& 
str_out::operator<<(double d)
{
	zend_string* s = zend_double_to_str(d);
	append(s);
	zend_string_release(s);
	return *this;
}

str_out& 
str_out::operator<<(zval* zv) 
{
	if (!zv) {
		return *this;
	}
	val_ptr fx(zv);

	if (fx.isString())
	{
		append(fx.zstr());
	}
	else {
		str_rc convert;
		convert.adopt(fx.to_zstr());

		append((zend_string*) convert);
	}
	return *this;
}


str_out& 
str_out::operator<<(char c)
{
	append(c);
	return *this;
}

str_out& 
str_out::operator<<(const std::string_view &v) 
{
	size_t slen = v.length();
	if (slen > 0) {
		append(v.data(), slen);
	}
	return *this;
}

str_out& 
str_out::operator<<(unsigned int iv)
{
	const char* sfmt;

	if (nf_.value_ == Numf::DEC)
	{
		sfmt = "%u";
	}
	else {
		sfmt = "%x";
	}
	zend_string* pf = strpprintf(0,sfmt,iv);
	append(pf);
	zend_string_release(pf);
	return *this;
}

str_out& 
str_out::operator<<(int iv) 
{	
	const char* sfmt;

	if (nf_.value_ == Numf::DEC)
	{
		sfmt = "%d";
	}
	else {
		sfmt = "%x";
	}
	zend_string* pf = strpprintf(0,sfmt,iv);
	append(pf);
	zend_string_release(pf);
	return *this;
}

str_out& 
str_out::operator<<(long iv)
{
	const char* sfmt;

	if (nf_.value_ == Numf::DEC)
	{
		sfmt = "%lu";
	}
	else {
		sfmt = "%lx";
	}
	zend_string* pf = strpprintf(0,sfmt,iv);
	append(pf);
	zend_string_release(pf);
	return *this;

}

str_out& 
str_out::operator<<(size_t iv) 
{	
	const char* sfmt;

	if (nf_.value_ == Numf::DEC)
	{
		sfmt = "%lu";
	}
	else {
		sfmt = "%lx";
	}
	zend_string* pf = strpprintf(0,sfmt,iv);
	append(pf);
	zend_string_release(pf);
	return *this;
}

void
str_out::append(zend_string* s) 
{
	if (!s) {
		return;
	}
	append(ZSTR_VAL(s), ZSTR_LEN(s));
}


str_out& 
str_out::operator<<(const str_rc &w)
{
	append((zend_string*)w);
	return *this;
}

str_out& 
str_out::operator<<(str_ptr w)
{
	append(w);
	return *this;
}

str_out& 
str_out::operator<<(zend_string* s)
{
 	append(s);
 	return *this;
}

str_out& 
str_out::operator<<(const char* c)
{
	append(c, strlen(c));
	return *this;
}


void 
str_out::append(const char* c, size_t slen)
{
	zend_write(c, slen);
}

void 
str_out::append(char c)
{
	char temp[2];
	temp[0] = c;
	temp[1] = '\0';
	

	zend_write(&temp[0], 1);
}
};

#endif