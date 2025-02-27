 /*  
  *  PHP extension C++ classes - zpp 
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  */

#ifndef ZSTR_MGR_CPP
#define ZSTR_MGR_CPP

#ifndef ZSTR_MGR_H
#include "zstr_mgr.h"
#endif

extern "C" {
	#include "ext/standard/php_math.h"
};

namespace zpp {

void //protected
zstr_mgr::lose()
{
	zend_string* p = s;
	s = nullptr;
	if (!p || (GC_FLAGS(p) & IS_STR_INTERNED)) {
		return;
	}
	zend_string_release(p);
}

void 
zstr_mgr::bind(zend_string* rc)
{
	if (s != rc)
	{
        lose();
        s = rc;
        own();
    }
}

void //protected
zstr_mgr::own()
{
	if (!s) {
		return;
	}
	if (GC_FLAGS(s) & IS_STR_INTERNED)
	{
		return;
	}
	zend_string_addref(s);
}

void 
zstr_mgr::decref()
{
	if (!s || (GC_FLAGS(s) & IS_STR_INTERNED)) return;
	zend_string* p = s;
	if (GC_REFCOUNT(s) == 1) s = nullptr;
	zend_string_release(p);
}

zstr_mgr& 
zstr_mgr::operator=(zstr_mgr&& rc)
{
    bind(rc.s);
    rc.s = nullptr;
    return *this;
}

const zstr_mgr& 
zstr_mgr::operator=(zend_string* rc)
{
	bind(rc);
	return *this;
}

const zstr_mgr& 
zstr_mgr::operator=(zval* rc)
{
	zend_string* p = zval_user(rc).zstr();
	bind(p);
	return *this;
}


void zstr_mgr::adopt(zend_string* rc)
{
	if (s != rc)
	{
		lose();
		s = rc;
	}
}

zstr_mgr::zstr_mgr(zend_long ival)
{
    s = _php_math_longtobase(ival,10);
}


size_t 
zstr_mgr::size() const
{
	if (!s)
		return 0;
	return ZSTR_LEN(s);
}

zstr_perm::zstr_perm(const char* c, size_t slen)
{
	if (!slen)
	{
		slen = strlen(c);
	}
	s = zend_string_init(c, slen, 1);
}

zstr_temp::zstr_temp(const char* c, size_t slen)
{
	if (!slen)
	{
		slen = strlen(c);
	}
	s = zend_string_init(c, slen, 0);
}

zstr_intern::zstr_intern(const char* c, size_t slen)
{
	if (!slen)
	{
		slen = strlen(c);
	}
	s = zend_string_init(c, slen, 1);
	zend_string* p = zend_new_interned_string(s);
	if ((p != s) && (GC_FLAGS(p) & IS_STR_INTERNED))
	{
		lose(); 
		s = p;
	}
}

};

#endif

