 /*  
  *  PHP extension C++ classes - zpp 
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  */

#ifndef STR_PTR_CPP
#define STR_PTR_CPP

#ifndef FN_CALL_H
#include "fn_call.h"
#endif

extern "C" {
	#include <ext/standard/php_string.h>
	#include <ext/json/php_json.h>
};

#ifndef STR_RC_H
#include "str_rc.h"
#endif

namespace zpp {

const char* str_ptr::empty = "\0";


int 
zs_cmp(zend_string* a, zend_string* b)
{
	if (!a && !b)
	{
		return 0;
	}
	if (!a) {
		return -1;
	}
	if (!b) {
		return 1;
	}
	const char* ap = ZSTR_VAL(a);
	const char* bp = ZSTR_VAL(b);

	for (;; ap++, bp++) {
        int d = *ap - *bp;
        if (d != 0 || !*ap)
            return d;
    }
    return 0;
}

int zs_cmp_ci(zend_string* a, zend_string* b)
{
	if (!a && !b)
	{
		return 0;
	}
	if (!a) {
		return -1;
	}
	if (!b) {
		return 1;
	}
	const char* ap = ZSTR_VAL(a);
	const char* bp = ZSTR_VAL(b);

	for (;; ap++, bp++) {
        int d = tolower((unsigned char)*ap) - tolower((unsigned char)*bp);
        if (d != 0 || !*ap)
            return d;
    }
    return 0;
}

// call php for its well-tested complex implementation
str_rc
str_ptr::strtr(const char* from, const char* to)
{
	str_temp fstr(from);
	str_temp tstr(to);
	return zpp::strtr(s, fstr, tstr);
}

str_ptr::str_ptr(const val_ptr& rc)
{
    s = rc.zstr();
}

str_ptr::str_ptr(zval* p)
{
	s = val_ptr(p).zstr();
}

str_ptr::str_ptr(const str_rc& mgr)
{
    s = mgr.s;
}

str_ptr::str_ptr(const str_intern& zs)
{
	s = zs.s;
}

const char* 
str_ptr::data() const
{
	if (!s)
	{
		return empty;
	}
	return ZSTR_VAL(s);
}

size_t 
str_ptr::size() const 
{
	if (!s)
	{
		return 0;
	}
	return ZSTR_LEN(s);
}

zend_long 
str_ptr::getLong(int base) const
{
	if (!s)
	{
		return 0;
	}
	char* endptr;
	return strtol(ZSTR_VAL(s), &endptr, base);
}

double 
str_ptr::getDouble() const
{
	if (!s)
	{
		return 0.0;
	}
	char* endptr;
	return strtod(ZSTR_VAL(s), &endptr);
}

std::string 
str_ptr::cstr() const
{
    const char* ps = s ? (const char*) ZSTR_VAL(s) : empty;
	size_t      slen = s ? ZSTR_LEN(s) : 0;

	return std::string(ps, slen);
}

std::string_view 
str_ptr::vstr() const 
{
	const char* ps = s ? (const char*) ZSTR_VAL(s) : empty;
	size_t      slen = s ? ZSTR_LEN(s) : 0;

	return std::string_view(ps, slen);
}


std::string_view  
str_ptr::subview(int offset, int len) const
{
	size_t slen = size();
	size_t pos = 0;

	static const char* strview_empty = "\0";

	if (offset < 0)
	{
		if ((int)slen > (-offset))
		{
			pos = slen + offset;
		}
	}
	else {
		pos = offset;
	}
	if (len < 0)
	{
		int remaining = int(slen) + len - int(pos);
		if (remaining <= 0) {
			return std::string_view(strview_empty);
		}
		slen = (size_t)remaining;
	}
	else {
		slen = len;
	}
	//zend_printf("sview substr from %ld , len %ld\n", pos, slen);
	return vstr().substr(pos,slen);
}

str_rc
str_ptr::substr(int offset, int len) const
{
	 std::string_view text(subview(offset, len));

	 str_temp result(text.data(), text.size());

	 //showstr("substr", result);
	 return std::move(result);
}

bool 
str_ptr::starts_with(str_ptr match) const
{
	size_t mlen = match.size();
	if (!mlen || size() < mlen)
	{
		return false;
	}
	std::string_view mb = match.vstr();
	return (this->subview(0,mlen) == mb);
}

str_rc
str_ptr::ucfirst()
{
	str_rc result;

	if (size() < 1)
	{
		return result;
	}
	const unsigned char ch = ZSTR_VAL(s)[0];
	unsigned char r = zend_toupper_ascii(ch);
	if (r == ch) {
		result.adopt(zend_string_copy(s));
	} else {
		zend_string *uc = zend_string_init(ZSTR_VAL(s), ZSTR_LEN(s), 0);
		ZSTR_VAL(uc)[0] = r;
		result.adopt(uc);
	}
	return result;
}

bool 
str_ptr::ends_with(str_ptr match) const
{
	size_t mlen = match.size();
	size_t mysize = size();
	if (!mlen || mysize < mlen)
	{
		return false;
	}
	std::string_view mb = match.vstr();
	std::string_view endslice = this->subview(mysize-mlen, mlen);
	return (endslice == mb);
}

int  
str_ptr::find(const std::string_view& needle, size_t pos) const
{
	if (!s) {
		return -1;
	}
	size_t   slen = ZSTR_LEN(s);
	size_t   nlen = needle.length();

	// if pos + nlen > slen, then cannot be found

	if ((pos + nlen) > slen) {
		return -1;
	}

	const char* p = ZSTR_VAL(s) + pos;
	std::string_view haystack(p, slen-pos);
	size_t result = haystack.find(needle, pos);
	return (result == std::string_view::npos) ? -1 : (int) result;
}

int 
str_ptr::find(char c, size_t pos) const
{
	if (!s) {
		return -1;
	}

	const char* p = ZSTR_VAL(s) + pos;
	const char* lim = p + ZSTR_LEN(s);
	int ix = int(pos);
	while(p < lim)
	{
		if (*p == c)
		{
			return ix;
		}
		p++;
		ix++;
	}
	return -1;
}

str_rc 
str_ptr::uncamel(const char* sep)
{
	str_rc result;	
	if (!s) {
		return result;
	}

	str_buf buf;

	const char* marker;
	const char* psep;
	int   sep_len;

	if (!sep)
	{
		sep = "_-";
		sep_len = 2;
	}
	else {
		sep_len = strlen(sep);
	}

	psep = sep;

	int   i, len, found = 1;
	char ch;

	marker = ZSTR_VAL(s);
	len    = ZSTR_LEN(s);

	for (i = 0; i < len; i++) {
		ch = marker[i];

		if (memchr(psep, ch, sep_len)) {
			found = 1;
			continue;
		}
		if (found == 1) {
			buf << toupper(ch);
			found = 0;
		} else {
			buf << tolower(ch);
		}
	}
	result = buf.zstr();
	return result;
}

int 
str_ptr::rfind(char c, size_t pos) const
{
	if (!s) {
		return -1;
	}

	const char* pfirst = ZSTR_VAL(s);
	size_t slen = ZSTR_LEN(s);

	if (pos >= slen) {
		pos = slen-1;
	}
	const char* plast = pfirst + pos;

	while(plast >= pfirst)
	{
		if (*plast == c)
		{
			return (int)pos;
		}
		plast--;
		pos--;
	}
	return -1;
}

bool
str_ptr::contains(str_ptr needle)
{
	return strpos(needle) >= 0;
}

int
str_ptr::strpos(str_ptr needle)
{
	std::string_view nview(needle.vstr());

	return find(nview,0);
}

void str_ptr::return_zv(zval* ret) const
{
	ZVAL_STR_COPY(ret, s);
}

const str_ptr& 
str_ptr::operator=(zval* rc)
{
	s = val_ptr(rc).zstr();
	return *this;
}


str_rc //static
str_ptr::json_encode(val_ptr value, int flags)
{
	//(smart_str *buf, zval *val, int options);
	// TODO: consider options flags
	str_rc result;

	smart_str buf = {0};

	zend_result zret = php_json_encode(&buf, value, flags);

	if (zret == SUCCESS)
	{
		result.adopt(smart_str_extract_ex(&buf,0));
		return result;
	}
	else {
		//TODO: Check JSON_G(error_code) ? Partial result
		if (buf.s != nullptr)
		{
			smart_str_free(&buf);
		}
		return result;
	}
}


str_rc 
str_replace(
	str_ptr mstr, 
	str_ptr rstr, 
	str_ptr subject )
{
	return str_replace(mstr.vstr(), rstr.vstr(), subject);
}

str_rc
str_replace(
	const std::string_view& src, 
	const std::string_view& replace,
	str_ptr subject)
{
	std::string_view s = subject.vstr();
	
	const size_t rlen = replace.length();
	const size_t slen = s.length();

	const size_t srclen = src.length();

	size_t oklen = slen;
	size_t pos = 0;

	str_buf  result;
	while(true)
	{
		size_t okpos = pos;
		pos = s.find(src, pos);

		if (pos == std::string_view::npos)
		{
			oklen = slen - okpos;
			if (oklen > 0) {
				result << s.substr(okpos, oklen);
			}
			break;
		}
		else {
			oklen = pos - okpos;
			pos += srclen;
			if (oklen > 0) {
				result << s.substr(okpos, oklen);
			}
			if (rlen > 0)
			{
				result << replace;
			}
		}
	}
	return result.zstr();
}

str_ptr //static
str_ptr::empty_str()
{
	return str_ptr(zend_empty_string);
}

str_rc 
str_ptr::duplicate() const
{
	str_rc result;

	if (s)
	{
		zend_string* dup = zend_string_init(ZSTR_VAL(s), ZSTR_LEN(s), 0);
		result.adopt(dup);
	}
	return result;
}

str_rc  operator+(str_ptr lhs, str_ptr rhs)
{
    str_buf buf;

    buf << lhs << rhs;

    return buf.zstr();
}

};
//str_ptr.cpp
#endif