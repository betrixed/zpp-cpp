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

#ifndef ZSTR_BUFFER_H
#include "zstr_buffer.h"
#endif

extern "C" {
	#include <ext/standard/php_math.h>
	#include <ext/standard/base64.h>
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
    else {
    	//showstr("bind same", s);
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
	GC_ADDREF(s);
	//showstr("zstr own ", s);
}

void zstr_mgr::try_addref(zend_string* zs)
{
	if (zs->gc.u.type_info & IS_STR_INTERNED)
	{
		return;
	}
	++zs->gc.refcount;
}

bool zstr_mgr::try_decref(zend_string* zs)
{
	if (zs->gc.u.type_info & IS_STR_INTERNED)
	{
		return false;
	}
	int rc = zs->gc.refcount-1;
	zend_string_release(zs);
	return !(rc);
}


zstr_mgr& 
zstr_mgr::operator=(zstr_temp&& rc)
{

    zend_string* p = rc.s;
    if (p != s)
    {
    	lose();
    }
    s = p;
    //showstr("operator= zstr_temp&&", s);
    rc.s = nullptr;
    return *this;
}

zstr_mgr& 
zstr_mgr::operator=(zstr_mgr&& rc)
{

    zend_string* p = rc.s;
    if (p != s)
    {
    	lose();
    }
    s = p;
    //showstr("operator= &&", s);
    rc.s = nullptr;
    return *this;
}

const zstr_mgr& 
zstr_mgr::operator=(const zstr_user& rc)
{
	bind(rc.s);
	return *this;
}


 const char* zstr_mgr::data() const
 {
 	return zstr_user(*this).data();
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
	bind(zval_user(rc).zstr());
	return *this;
}

zstr_mgr& 
zstr_mgr::operator=(zval_mgr&& rc)
{
	lose(); //not calling bind
	s = zval_user(rc).zstr();
	rc.init();
	return *this;	
}
const zstr_mgr& 
zstr_mgr::operator=(const zstr_mgr& rc)
{
	bind(rc.s);
	//showstr("operator= &", s);
	return *this;
}

void zstr_mgr::init()
{
	zend_string* empty = zend_empty_string;
	if (s && (s != empty)) {
		lose();
	}
	s = empty;
}
void zstr_mgr::adopt(zend_string* rc)
{
	if (s != rc)
	{
		lose();
		s = rc;
	}
}

zstr_mgr::zstr_mgr(zval* copy) : zstr_user()
{
	bind(zval_user(copy).zstr());
}

zstr_mgr::zstr_mgr(const zval_user& rc): zstr_user()
{
	bind(rc.zstr());
}

zstr_mgr::zstr_mgr(zend_long ival) : zstr_user()
{
	// s has rc==1
    s = _php_math_longtobase(ival,10);
}

zstr_mgr::zstr_mgr(zval_mgr&& rc): zstr_user()
{
	s = zval_user(rc).zstr();
	rc.init();
}

size_t 
zstr_mgr::size() const
{
	if (!s)
		return 0;
	return ZSTR_LEN(s);
}


zstr_mgr::~zstr_mgr()
{
	//showstr("~zstr_mgr", s);
    lose();
}

void 
zstr_mgr::move_zv(zval* ret)
{
	if (s)
	{
		ZVAL_STR(ret, s);
		s = nullptr;
	}
	else {
		ZVAL_NULL(ret);
	}
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
	//showstr("zstr_temp", s);

}

zstr_intern::zstr_intern(const char* c, size_t slen)
{
	if (!slen)
	{
		slen = strlen(c);
	}
	s = zend_string_init(c, slen, 1);
	//showstr("init s",s);
	s = zend_new_interned_string(s);
	//showstr("interned s",p);

}


const zstr_intern& 
zstr_intern::operator=(const char* cp)
{
	auto slen = strlen(cp);
	if (slen)
	{
		s = zend_string_init(cp, slen, 1);
		s = zend_new_interned_string(s);
	}
	return *this;
}


zstr_empty::zstr_empty() {
	s = zend_empty_string;
}

const zstr_mgr& 
zstr_mgr::operator=(zstr_buffer&& m)
{
	lose();
	// zstr_buffer cleared by this, refcount==1
	s = m.finalize(); 
	return *this;
}

zstr_mgr::zstr_mgr(zstr_buffer&& m)
{
	s = m.finalize();// zstr_buffer cleared by this, refcount==1
}


zstr_mgr 
zstr_mgr::base64_decode(const unsigned char* c, size_t slen)
{
	zstr_mgr result;

	zend_string* d = php_base64_decode(c, slen);

	result.adopt(d);

	return result;
}

zstr_mgr 
zstr_mgr::base64_encode(const unsigned char* c, size_t slen)
{
	zstr_mgr result;

	zend_string* d = php_base64_encode(c, slen);

	result.adopt(d);

	return result;
}

};

#endif

