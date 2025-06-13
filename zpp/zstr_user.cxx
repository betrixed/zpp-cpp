 /*  
  *  PHP extension C++ classes - zpp 
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  */

#ifndef ZSTR_USER_CPP
#define ZSTR_USER_CPP

#ifndef FN_CALL_H
#include "fn_call.h"
#endif

extern "C" {
	#include <ext/standard/php_string.h>
	#include <ext/json/php_json.h>
};

namespace zpp {

const char* zstr_user::empty = "\0";


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
zstr_mgr
zstr_user::strtr(const char* from, const char* to) const
{
	zstr_temp fstr(from);
	zstr_temp tstr(to);
	return zpp::strtr(s, fstr, tstr);
}

zstr_user::zstr_user(const zval_user& rc)
{
    s = rc.zstr();
}

zstr_user::zstr_user(zval* p)
{
	s = zval_user(p).zstr();
}

zstr_user::zstr_user(const zstr_mgr& mgr)
{
    s = mgr.s;
}

zstr_user::zstr_user(const zstr_intern& zs)
{
	s = zs.s;
}

const char* 
zstr_user::data() const
{
	if (!s)
	{
		return empty;
	}
	return ZSTR_VAL(s);
}

size_t 
zstr_user::size() const 
{
	if (!s)
	{
		return 0;
	}
	return ZSTR_LEN(s);
}

zend_long 
zstr_user::getLong(int base) const
{
	if (!s)
	{
		return 0;
	}
	char* endptr;
	return strtol(ZSTR_VAL(s), &endptr, base);
}

double 
zstr_user::getDouble() const
{
	if (!s)
	{
		return 0.0;
	}
	char* endptr;
	return strtod(ZSTR_VAL(s), &endptr);
}

std::string 
zstr_user::cstr() const
{
    const char* ps = s ? (const char*) ZSTR_VAL(s) : empty;
	size_t      slen = s ? ZSTR_LEN(s) : 0;

	return std::string(ps, slen);
}

std::string_view 
zstr_user::vstr() const 
{
	const char* ps = s ? (const char*) ZSTR_VAL(s) : empty;
	size_t      slen = s ? ZSTR_LEN(s) : 0;

	return std::string_view(ps, slen);
}


std::string_view  
zstr_user::subview(int offset, int len) const
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

zstr_mgr
zstr_user::substr(int offset, int len) const
{
	 std::string_view text(subview(offset, len));

	 zstr_temp result(text.data(), text.size());

	 //showstr("substr", result);
	 return std::move(result);
}

bool 
zstr_user::starts_with(zstr_user match) const
{
	size_t mlen = match.size();
	if (!mlen || size() < mlen)
	{
		return false;
	}
	std::string_view mb = match.vstr();
	return (this->subview(0,mlen) == mb);
}

bool 
zstr_user::ends_with(zstr_user match) const
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
zstr_user::find(const std::string_view& needle, size_t pos) const
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
zstr_user::find(char c, size_t pos) const
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

zstr_mgr 
zstr_user::uncamel(const char* sep) const
{
	zstr_mgr result;	
	if (!s) {
		return result;
	}

	zstr_buffer buf;

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
zstr_user::rfind(char c, size_t pos) const
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
zstr_user::contains(zstr_user needle)
{
	return strpos(needle) >= 0;
}

zstr_mgr
zstr_user::to_lower() const
{
	zstr_mgr result;
	if (s)
	{
		zend_string* p = zend_string_tolower(s);
		result.adopt(p);	
	}
	return result;
}

zstr_mgr 
zstr_user::to_upper() const
{
	zstr_mgr result;
	if (s)
	{
		zend_string* p = zend_string_toupper(s);
		result.adopt(p);	
	}
	return result;
}

zstr_mgr
zstr_user::trim(const char* what, int mode) const
{
	zstr_mgr result;

	size_t slen = what ? strlen(what) : 0;

	if (s) {
		zend_string* p = php_trim(s, what, slen, mode);
		result.adopt(p);
	}
	return result;
}

int
zstr_user::strpos(zstr_user needle)
{
	std::string_view nview(needle.vstr());

	return find(nview,0);
}

void zstr_user::return_zv(zval* ret)
{
	ZVAL_STR_COPY(ret, s);
}

const zstr_user& 
zstr_user::operator=(zval* rc)
{
	s = zval_user(rc).zstr();
	return *this;
}


zstr_mgr //static
zstr_user::json_encode(zval_user value, int flags)
{
	//(smart_str *buf, zval *val, int options);
	// TODO: consider options flags
	zstr_mgr result;

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


zstr_mgr 
str_replace(
	zstr_user mstr, 
	zstr_user rstr, 
	zstr_user subject )
{
	return str_replace(mstr.vstr(), rstr.vstr(), subject);
}

zstr_mgr
str_replace(
	const std::string_view& src, 
	const std::string_view& replace,
	zstr_user subject)
{
	std::string_view s = subject.vstr();
	
	const size_t rlen = replace.length();
	const size_t slen = s.length();

	const size_t srclen = src.length();

	size_t oklen = slen;
	size_t pos = 0;

	zstr_buffer  result;
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

};
//zstr_user.cpp
#endif