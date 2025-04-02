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
	#include "ext/standard/php_string.h"
};

namespace zpp {

const char* zstr_user::empty_zstr = "\0";

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

const char* 
zstr_user::data() const
{
	if (!s)
	{
		return empty_zstr;
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
    const char* ps = s ? (const char*) ZSTR_VAL(s) : empty_zstr;
	size_t      slen = s ? ZSTR_LEN(s) : 0;

	return std::string(ps, slen);
}

std::string_view 
zstr_user::vstr() const 
{
	const char* ps = s ? (const char*) ZSTR_VAL(s) : empty_zstr;
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
		if (slen > (-offset))
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
	 return result;
}

bool 
zstr_user::starts_with(zstr_user match)
{
	size_t mlen = match.size();
	if (size() < mlen)
	{
		return false;
	}
	std::string_view mb = match.vstr();
	return (this->subview(0,mlen) == mb);
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
		if (p == s)
		{
			zstr_mgr::try_decref(p);
		}
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
		if (p == s)
		{
			zstr_mgr::try_decref(p);
		}
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
		if (p == s)
		{
			zstr_mgr::try_decref(p);
		}
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


};
//zstr_user.cpp
#endif