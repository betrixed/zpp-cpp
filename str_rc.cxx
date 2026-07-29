	 /*  
  *  @file str_rc.cxx
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2025 Michael Rynn
  *  @brief str_rc class, reference counted zend_string pointer.	
  *  @license BSD 3-Clause License	
  */

#ifndef STR_RC_CPP
#define STR_RC_CPP

#ifndef STR_RC_H
#include "str_rc.h"
#endif

extern "C" {
	#include <ext/standard/php_math.h>
	#include <ext/standard/base64.h>
};

namespace zpp {

void //protected
str_rc::lose()
{
	if (!s) {
		return;
	}
	zend_string* p = s;
	s = nullptr;
	if ((GC_FLAGS(p) & IS_STR_INTERNED)) {
		return;
	}
	if (GC_DELREF(p) == 0) {
		//showstr("free-", p);
		pefree(p, GC_FLAGS(p) & IS_STR_PERSISTENT);
	} else {
		//showstr("lose-", p);
	}
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
    else if (s) 
    {
    	//showstr("Error bind same", s);
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

/*
void str_rc::try_addref(zend_string* zs)
{
	if (zs->gc.u.type_info & IS_STR_INTERNED)
	{
		return;
	}
	++zs->gc.refcount;
}
*/

/*
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
*/
// make a temporary string
str_rc::str_rc(const char* cp, int slen) : str_ptr()
{
	if (!cp)
	{
		return;
	}
	if (slen < 1)
	{
		//check!!
		slen = strlen(cp);
	}
	if (slen==0)
	{
		s = str_ptr::empty_str();
	}
	else {
		s = zend_string_init(cp, slen, 0);
	}
	//showstr("str_rc(): ", s);
}


str_rc& 
str_rc::operator=(str_rc&& rc)
{

    zend_string* p = rc.s;
    //showstr("operator= str_rc&&", p);
    if (p != s)
    {
    	lose();
    	s = p;
    }
    else {
    	// presume 2 copies, but now one is erased.
    	if (s) {
    		try_decref(s); // 1 instance disappears
    	}
    }
    //showstr("operator= &&", s);
    rc.s = nullptr;

    return *this;
}

const str_rc& 
str_rc::operator=(const str_ptr& rc)
{
	//showstr("operator= str_ptr&", rc.s);
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
	//showstr("operator= zend_string*", rc);
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
	//showstr("operator= str_rc&", rc.s);
	bind(rc.s);
	//showstr("operator= &", s);
	return *this;
}

const str_rc& 
str_rc::operator=(const char* cp)
{
	auto slen = strlen(cp);
	if (slen)
	{
		s = zend_string_init(cp, slen, 0);
	}
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

str_rc 
str_rc::to_lower() 
{
	str_rc result(*this);
	if (result.ok())
	{
		result.lowercase();
	}
	return result;
}

str_rc 
str_rc::to_upper() 
{
	str_rc result(*this);
	if (result.ok())
	{
		result.uppercase();
	}
	return result;
}

void
str_rc::lowercase() 
{
	if (s)
	{
		// always added reference count
		zend_string* p = zend_string_tolower(s);
		if (p == s)
		{
			try_decref(p); // undo unwanted rc++
		}
		else {
			adopt(p); 
		}
	}
}

str_rc
str_rc::trim(const char* what, int mode) 
{
	str_rc result(*this);

	if (result.ok())
	{
		result.trim_self(what, mode);
	}
	return result;
}

void
str_rc::trim_self(const char* what, int mode) 
{
	if (s)
	{
		size_t slen = what ? strlen(what) : 0;
		zend_string* p = php_trim(s, what, slen, mode);
		//showstr("php_trim", p);
		if (p == s)
		{
			try_decref(p);
		}
		else {
			adopt(p);
		}
	}
}

void
str_rc::uppercase() 
{
	if (s)
	{
		zend_string* p = zend_string_toupper(s);
		if (p == s)
		{
			try_decref(p);
		}
		else {
			adopt(p);
		}
	}
}

str_rc::str_rc(const std::string_view& sv)
{
	auto slen = sv.size();
	if (slen)
	{
		s = zend_string_init(sv.data(), slen, 0);
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
str_rc::copy_zv(zval* ret)
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

str_perm::str_perm(const char* c, size_t slen)
{
	if (!slen)
	{
		slen = strlen(c);
	}
	s = zend_string_init(c, slen, 1);
}


str_intern::str_intern(const char* c, size_t slen)
{
	if (!slen)
	{
		slen = strlen(c);
	}
	s = zend_string_init(c, slen, 1);
	
	//showstr("init s",s);
	if (!interned())
	{	
		GC_ADDREF(s);
		s = zend_new_interned_string(s);
	}
	
	//showstr("interned s",s);

}


const str_intern& 
str_intern::operator=(const char* cp)
{
	
	//lose(); //don't need this
	auto slen = strlen(cp);
	if (slen)
	{
		zend_string* p = zend_string_init(cp, slen, 1);
		if ((GC_FLAGS(p) & IS_STR_INTERNED))
		{
			s = p;
		}
		else {
			GC_ADDREF(p);
			s = zend_new_interned_string(p);
		}
	}
	return *this;
}


str_empty::str_empty() {
	s = zend_empty_string;
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



};

#endif

