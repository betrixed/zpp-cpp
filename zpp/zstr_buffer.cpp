#ifndef ZSTR_BUFFER_CPP
#define ZSTR_BUFFER_CPP



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

void zstr_buffer::append(char c)
{
	smart_str_appendc_ex(&buf, c, 0);
}


void
zstr_buffer::append(const char* c, size_t slen)
{
	if (!c || !slen) {
		return;
	}
	smart_str_appendl_ex(&buf, c, slen, 0);
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


const char* 
zstr_buffer::data() const {
	if (buf.s) {
		return ZSTR_VAL(buf.s);
	}
	else {
		return zstr_user::empty_zstr;
	}
}

size_t 
zstr_buffer::size() const 
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
	return smart_str_extract_ex(&buf, 0);
}
// this doesn't seem to be useful.
/*
zstr_buffer& 
zstr_buffer::endnull() {
	smart_str_0((smart_str*)(this));
	return *this;
}
*/

std::string_view 
zstr_buffer::vstr() const
{
	if (buf.s)
	{
		return std::string_view(ZSTR_VAL(buf.s), ZSTR_LEN(buf.s));
	}
	else {
		return std::string_view(zstr_user::empty_zstr, 0);
	}
}
};
//zstr_buffer.cpp
#endif
