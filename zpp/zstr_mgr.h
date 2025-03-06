 /*  
  *  PHP extension C++ classes - zpp 
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  */

#ifndef ZSTR_MGR_H
#define ZSTR_MGR_H

namespace zpp {

	class zstr_user;
	class zval_mgr;
	class zstr_buffer;
	
	class ZPP_EXPORT zstr_mgr {
	protected:
	    zend_string* s;

	    void own();
	    void lose();
	    void bind(zend_string* rc);

	    friend class zstr_user;
	    friend class zval_mgr;

	public:
	    zstr_mgr() : s((zend_string*) nullptr)
	    {   
	    }


	    ~zstr_mgr(){
	        lose();
	    }
	    
	    zstr_mgr(zend_string* p) : s(p)
	    {
	        own();
	    }

	    zstr_mgr(zval* copy);

	    zstr_mgr(const zstr_mgr& rc)
	    {
	        s = rc.s;
	        own();
	    }

	    zstr_mgr(zval_mgr&& rc);

	    zstr_mgr(zend_long ival);
	    
	    zstr_mgr(zstr_mgr&& rc)
	    {
	        s = rc.s;
	        rc.s = nullptr;
	    }

	    zstr_mgr(zstr_buffer&& m);

	    bool ok() const {
	    	return (s);
	    }

	    bool isNull() const {
	    	return !(s);
	    }
	    
	    void decref();
	    void addref();
	    void init();
	    
	    size_t size() const;
	    const char* data() const;

	    const zstr_mgr& operator=(const zstr_mgr& rc);
	    const zstr_mgr& operator=(zend_string* rc);
	    const zstr_mgr& operator=(zval* rc);
		 
		const zstr_mgr& operator=(zstr_buffer&& m);

	    zstr_mgr& operator=(zval_mgr&& rc);
	    zstr_mgr& operator=(zstr_mgr&& rc);

	    void move_zv(zval* ret);

	    void adopt(zend_string* rc);

	    operator zend_string*() const { return (zend_string*) s; }

	    zstr_mgr& operator=(zstr_user&&	rc);
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

		operator zend_string*() const { return (zend_string*) s; }
	};



}; // namespace Php

//zstr_mgr.h
#endif