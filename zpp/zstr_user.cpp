 /*  
  *  PHP extension C++ classes - zpp 
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  */

#ifndef ZSTR_USER_CPP
#define ZSTR_USER_CPP

namespace zpp {

const char* zstr_user::empty_zstr = "\0";

const char* 
zstr_user::data() const
{
	if (!s)
	{
		return nullptr;
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


};
//zstr_user.cpp
#endif