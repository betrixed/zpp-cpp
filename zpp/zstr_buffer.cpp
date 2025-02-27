#ifndef ZSTR_BUFFER_CPP
#define ZSTR_BUFFER_CPP

extern "C" {
	#include <Zend/zend_smart_str.h>
};

namespace zpp {

zend_string* 
zstr_buffer::init_zs(const char* c, size_t slen)
{
	return zend_string_init(c, slen, 0);
}


zstr_buffer::zstr_buffer(zval *v) : zstr_mgr()
{
	s = zval_user(v).to_zstr();
	a = size();
}

zstr_buffer::zstr_buffer(const std::string_view& cs) 
		:  zstr_mgr()
{
	auto slen = cs.size();
	if (slen) 
	{
		s = init_zs(cs.data(), slen);
	}
	a = size();
}

zstr_buffer::zstr_buffer (const char* c, size_t slen) 
		:  zstr_mgr()
{
	if (slen)
	{
		s = init_zs(c, slen);
	}
	a = size();
}

zstr_buffer::zstr_buffer (const char* c) 
		:  zstr_mgr()
{
	auto slen = strlen(c);
	if (slen)
	{
		s = init_zs(c, slen);
	}
	a = size();
}


zstr_buffer::zstr_buffer(const std::string& cs) 
				:  zstr_mgr()
{
	auto slen = cs.size();
	if (slen) {
		s = init_zs(cs.data(), slen);
	}
	a = size();
}



zstr_buffer& 
zstr_buffer::operator<<(zval* zv) 
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
		zstr_mgr convert(fx.to_zstr());
		convert.decref();
		append((zend_string*) convert);
	}
	return *this;
}

void
zstr_buffer::append(const char* c, size_t slen)
{
	if (!c || !slen) {
		return;
	}
	smart_str_appendl_ex((smart_str*)(this), c, slen, 0);
}

zstr_buffer& 
zstr_buffer::operator<<(char c)
{

	smart_str_appendc_ex((smart_str*)this, c, 0);
	return *this;
}

zstr_buffer& 
zstr_buffer::operator<<(const std::string_view &v) 
{
	size_t slen = v.length();
	if (slen > 0) {
		append(v.data(), slen);
	}
	return *this;
}

zstr_buffer& 
zstr_buffer::operator<<(int iv) 
{	
	zstr_mgr intstr((zend_long) iv);
	append((zend_string*)intstr);
	return *this;
}

void
zstr_buffer::append(zend_string* s) 
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

void zstr_buffer::reset()
{
	if (s) {
		if (a != 0)
		{
			// TODO : logic error, need to finalize? 
			// to stop zend API complaint.
			zstr();
		}
		zend_string_release(s);
		s = nullptr;
		//init_list(0);
	}
}

};
//zstr_buffer.cpp
#endif
