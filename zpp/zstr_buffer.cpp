#ifndef ZSTR_BUFFER_CPP
#define ZSTR_BUFFER_CPP

extern "C" {
	#include <Zend/zend_smart_str.h>
};

namespace zpp {


zstr_buffer::zstr_buffer()
{
	initbuf();
}

zend_string* //static
zstr_buffer::init_zs(const char* c, size_t slen)
{
	return zend_string_init(c, slen, 0);
}

void
zstr_buffer::lose()
{
	if (buf.s)
	{
		smart_str_free(&buf);
		initbuf();
	}
}

zstr_buffer::~zstr_buffer() 
{
	lose();
}

zstr_buffer::zstr_buffer(zval *v)
{
	initbuf();
	zstr_mgr temp = zval_user(v).to_zstr();
	zend_string* s = temp;

	append(ZSTR_VAL(s), ZSTR_LEN(s));
}

zstr_buffer::zstr_buffer(const std::string_view& cs) 
{
	initbuf();
	auto slen = cs.size();
	if (slen) 
	{
		append(cs.data(), slen);
	}	
}

zstr_buffer::zstr_buffer (const char* c, size_t slen)  
{
	initbuf();
	if (slen)
	{
		append(c, slen);
	}
}

zstr_buffer::zstr_buffer (const char* c) 
{
	initbuf();
	auto slen = strlen(c);
	if (slen)
	{
		append(c, slen);
	}
}

zstr_buffer::zstr_buffer(const std::string& cs) 
{
	initbuf();
	auto slen = cs.size();
	if (slen) {
		append(cs.data(), slen);
	}
}

zstr_output& 
zstr_output::operator<<(const iform& form)
{
	nf_ = form;
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

void zstr_buffer::append(char c)
{
	smart_str_appendc_ex(&buf, c, 0);
}

void 
zstr_output::quote_name(const char* name)
{
	append('"');
	if (name)
		append(name,strlen(name));
	append('"');
}

void
zstr_buffer::append(const char* c, size_t slen)
{
	if (!c || !slen) {
		return;
	}
	smart_str_appendl_ex(&buf, c, slen, 0);
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
		sfmt = "%ld";
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

// inalizes zend_string buffer, 
// return buffer copy as C-string
// and releases/destroys the zend_string
std::string 
zstr_buffer::str()
{
	zend_string* xs = zstr(); // reassign buffer back here.
	std::string result(ZSTR_VAL(xs), ZSTR_LEN(xs));
	zend_string_release(xs);
	return std::move(result);
}

// extract returns final zend string reallocated
// with null terminator, and also nulls the internal 
// zend_string s.

zend_string*
zstr_buffer::zstr()
{
	// note that member s must is null after this
	return smart_str_extract_ex((smart_str*)(this), 0);
}
// this doesn't seem to be useful.
/*
zstr_buffer& 
zstr_buffer::endnull() {
	smart_str_0((smart_str*)(this));
	return *this;
}
*/


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

void zstr_buffer::reset()
{
	lose();
}

zstr_buffer& 
zstr_buffer::operator=(const char* c)
{
	lose();
	append(c, strlen(c));
	return *this;
}	


};
//zstr_buffer.cpp
#endif
