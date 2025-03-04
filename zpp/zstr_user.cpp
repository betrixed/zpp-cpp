 /*  
  *  PHP extension C++ classes - zpp 
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  */

#ifndef ZSTR_USER_CPP
#define ZSTR_USER_CPP

namespace zpp {

const char* zstr_user::empty_zstr = "\0";

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

zstr_mgr
zstr_user::to_lower() const
{
	zstr_mgr result;
	if (s)
	{
		result.adopt(zend_string_tolower(s));
	}
	return result;
}

zstr_mgr 
zstr_user::to_upper() const
{
	zstr_mgr result;
	if (s)
	{
		result.adopt(zend_string_toupper(s));
	}
	return result;
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