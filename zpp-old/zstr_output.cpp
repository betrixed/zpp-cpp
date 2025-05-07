#ifndef ZSTR_OUTPUT_CPP
#define ZSTR_OUTPUT_CPP

//zstr_output.cpp
#ifndef ZSTR_OUTPUT_H
#include "zstr_output.h"
#endif


namespace zpp {


fm_endl endl;

zstr_output& 
zstr_output::operator<<(const iform& form)
{
	nf_ = form;
	return *this;
}

zstr_output& 
zstr_output::operator<<(const fm_endl& el)
{
	append('\n');
	return *this;
}

zstr_output& 
zstr_output::operator<<(void* vp)
{
	zend_string* pf = strpprintf(0,"%lx", vp);
	append(pf);
	zend_string_release(pf);
	return *this;
}

void 
zstr_output::quote_name(zend_string* name)
{
	append('"');
	append(name);
	append('"');
}

void 
zstr_output::quote_name(const char* name)
{
	append('"');
	if (name)
		append(name,strlen(name));
	append('"');
}

zstr_output& 
zstr_output::operator<<(double d)
{
	zend_string* s = zend_double_to_str(d);
	append(s);
	zend_string_release(s);
	return *this;
}

zstr_output& 
zstr_output::operator<<(zval* zv) 
{
	if (!zv) {
		return *this;
	}
	zval_user fx(zv);

	if (fx.isString())
	{
		append(fx.zstr());
	}
	else {
		zstr_mgr convert;
		convert.adopt(fx.to_zstr());

		append((zend_string*) convert);
	}
	return *this;
}


zstr_output& 
zstr_output::operator<<(char c)
{
	append(c);
	return *this;
}

zstr_output& 
zstr_output::operator<<(const std::string_view &v) 
{
	size_t slen = v.length();
	if (slen > 0) {
		append(v.data(), slen);
	}
	return *this;
}

zstr_output& 
zstr_output::operator<<(int iv) 
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

zstr_output& 
zstr_output::operator<<(long iv)
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

zstr_output& 
zstr_output::operator<<(size_t iv) 
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
zstr_output::append(zend_string* s) 
{
	if (!s) {
		return;
	}
	append(ZSTR_VAL(s), ZSTR_LEN(s));
}


zstr_output& 
zstr_output::operator<<(const zstr_mgr &w)
{
	append((zend_string*)w);
	return *this;
}

zstr_output& 
zstr_output::operator<<(zstr_user w)
{
	append(w);
	return *this;
}

zstr_output& 
zstr_output::operator<<(zend_string* s)
{
 	append(s);
 	return *this;
}

zstr_output& 
zstr_output::operator<<(const char* c)
{
	append(c, strlen(c));
	return *this;
}


void 
zstr_output::append(const char* c, size_t slen)
{
	zend_write(c, slen);
}

void 
zstr_output::append(char c)
{
	char temp[2];
	temp[0] = c;
	temp[1] = '\0';
	zend_write(&temp[0], 1);
}
};

#endif