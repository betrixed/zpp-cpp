 /*  
  *  PHP extension C++ classes - zpp 
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  */

#ifndef ZSTR_MGR_H
#define ZSTR_MGR_H

#ifndef ZSTR_USER_H
#include "zstr_user.h"
#endif

namespace zpp {

	class zstr_user;
	class zval_mgr;
	class zval_user;
	
	class zstr_buffer;
	class zstr_temp;
	
	class zstr_mgr  : public zstr_user
	{
	protected:

	    void own();
	    void lose();
	    void bind(zend_string* rc);

	    friend class zstr_user;
	    friend class zval_mgr;

	public:

		static void try_addref(zend_string* zs);
		static bool try_decref(zend_string* zs);

		~zstr_mgr();

	    zstr_mgr() : zstr_user()
	    {   
	    }
		
	    zstr_mgr(zend_string* p) : zstr_user(p)
	    {
	        own();
	    }

		zstr_mgr(zstr_mgr&& rc) : zstr_user(rc.s)
	    {
	        rc.s = nullptr;
	    }

		zstr_mgr(const zstr_mgr& rc) : zstr_user(rc.s)
	    {
	        own();
	    }

	    zstr_mgr(zval* copy);

	    zstr_mgr(const zval_user& rc);

	    zstr_mgr(zval_mgr&& rc);

	    zstr_mgr(zend_long ival);
	    
	    zstr_mgr(zstr_buffer&& m);

	    bool ok() const {
	    	return (s);
	    }

	    bool isNull() const {
	    	return !(s);
	    }
	    
	    void init();
	    
	    size_t size() const;
	    const char* data() const;

	    const zstr_mgr& operator=(const zstr_mgr& rc);
	    const zstr_mgr& operator=(zend_string* rc);
	    const zstr_mgr& operator=(zval* rc);

	    /* Use zval_user.to_zstr() instead */
		//const zstr_mgr& operator=(const zval_user& rc); 

		const zstr_mgr& operator=(zstr_buffer&& m);

	    zstr_mgr& operator=(zval_mgr&& rc);
	    zstr_mgr& operator=(zstr_mgr&& rc);
	    zstr_mgr& operator=(zstr_temp&& rc);

	    void move_zv(zval* ret);

	    void adopt(zend_string* rc);

	    operator zend_string*() const { return (zend_string*) s; }

	    const zstr_mgr& operator=(const zstr_user& rc);

	    static zstr_mgr base64_decode(const unsigned char* c, size_t slen);
	    static zstr_mgr base64_encode(const unsigned char* c, size_t slen);
	};

	class zstr_empty : public zstr_mgr
	{
	public:
		zstr_empty();
	};
	
	/** A "persistent" string, not using emalloc and efree */
	class zstr_perm : public zstr_mgr {
	public:
		zstr_perm() : zstr_mgr() {}
		zstr_perm(const char* c, size_t slen = 0);

		operator zend_string*() const { return (zend_string*) s; }
	};

	/** A "temporary" string, during a request, uses emalloc and efree */
	class zstr_temp : public zstr_mgr {
	public:
		zstr_temp() : zstr_mgr() {}
		zstr_temp(const char* c, size_t slen = 0);

		operator zend_string*() const { return (zend_string*) s; }
	};
	
	/** A "persistent" string stored as "interned", for module/class initialize */
	class zstr_intern : public zstr_mgr {
	public:
		zstr_intern() : zstr_mgr() {}
		zstr_intern(const char* c, size_t slen = 0);

		~zstr_intern() { 
			// let PHP take care of it
			s = nullptr; 
		}

		const zstr_intern& operator=(const char* cp);
		
		operator zend_string*() const { return (zend_string*) s; }
	};



}; // namespace Php

//zstr_mgr.h
#endif