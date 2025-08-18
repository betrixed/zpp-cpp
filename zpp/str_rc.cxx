 /*  
  *  PHP extension C++ classes - zpp 
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  */

#ifndef STR_RC_CPP
#define STR_RC_CPP

#ifndef STR_RC_H
#include "str_rc.h"
#endif

#ifndef STR_BUF_H
#include "str_buf.h"
#endif

extern "C" {
	#include <ext/standard/php_math.h>
	#include <ext/standard/base64.h>
};

namespace zpp {

void //protected
str_rc::lose()
{
	zend_string* p = s;
	s = nullptr;
	if (!p || (GC_FLAGS(p) & IS_STR_INTERNED)) {
		return;
	}
	zend_string_release(p);
	
}

void 
str_rc::bind(zend_string* rc)
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
str_rc::own()
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

void str_rc::try_addref(zend_string* zs)
{
	if (zs->gc.u.type_info & IS_STR_INTERNED)
	{
		return;
	}
	++zs->gc.refcount;
}

bool str_rc::try_decref(zend_string* zs)
{
	if (zs->gc.u.type_info & IS_STR_INTERNED)
	{
		return false;
	}
	int rc = zs->gc.refcount-1;
	zend_string_release(zs);
	return !(rc);
}


str_rc& 
str_rc::operator=(zstr_temp&& rc)
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

str_rc& 
str_rc::operator=(str_rc&& rc)
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

const str_rc& 
str_rc::operator=(const str_ptr& rc)
{
	bind(rc.s);
	return *this;
}


 const char* str_rc::data() const
 {
 	return str_ptr(*this).data();
 }

const str_rc& 
str_rc::operator=(zend_string* rc)
{
	bind(rc);
	return *this;
}

const str_rc& 
str_rc::operator=(zval* rc)
{
	bind(val_ptr(rc).zstr());
	return *this;
}

str_rc& 
str_rc::operator=(val_rc&& rc)
{
	lose(); //not calling bind
	s = val_ptr(rc).zstr();
	rc.init();
	return *this;	
}
const str_rc& 
str_rc::operator=(const str_rc& rc)
{
	bind(rc.s);
	//showstr("operator= &", s);
	return *this;
}

void str_rc::init()
{
	zend_string* empty = zend_empty_string;
	if (s && (s != empty)) {
		lose();
	}
	s = empty;
}
void str_rc::adopt(zend_string* rc)
{
	if (s != rc)
	{
		lose();
		s = rc;
	}
}

str_rc::str_rc(zval* copy) : str_ptr()
{
	bind(val_ptr(copy).zstr());
}

str_rc::str_rc(const val_ptr& rc): str_ptr()
{
	bind(rc.zstr());
}

str_rc::str_rc(zend_long ival) : str_ptr()
{
	// s has rc==1
    s = _php_math_longtobase(ival,10);
}

str_rc::str_rc(val_rc&& rc): str_ptr()
{
	s = val_ptr(rc).zstr();
	rc.init();
}

size_t 
str_rc::size() const
{
	if (!s)
		return 0;
	return ZSTR_LEN(s);
}


str_rc::~str_rc()
{
	//showstr("~str_rc", s);
    lose();
}


void 
str_rc::move_zv(zval* ret)
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

const str_rc& 
str_rc::operator=(str_buf&& m)
{
	lose();
	// str_buf cleared by this, refcount==1
	s = m.finalize(); 
	return *this;
}

str_rc::str_rc(str_buf&& m)
{
	s = m.finalize();// str_buf cleared by this, refcount==1
}


str_rc 
str_rc::base64_decode(const unsigned char* c, size_t slen)
{
	str_rc result;

	zend_string* d = php_base64_decode(c, slen);

	result.adopt(d);

	return result;
}

str_rc 
str_rc::base64_encode(const unsigned char* c, size_t slen)
{
	str_rc result;

	zend_string* d = php_base64_encode(c, slen);

	result.adopt(d);

	return result;
}

str_rc //static
str_rc::empty_str()
{
	return str_rc(zend_empty_string);
}

};

#endif

