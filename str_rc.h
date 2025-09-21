 /*  
  *  @file str_rc.h
  *  @brief str_rc class, reference counted zend_string pointer.	
  *  @license Artistic License 2.0
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2025 Michael Rynn
  */

#ifndef STR_RC_H
#define STR_RC_H

#ifndef STR_PTR_H
#include "str_ptr.h"
#endif

namespace zpp {

	class str_ptr;
	class val_rc;
	class val_ptr;
	
	class str_buf;
	class str_temp;
	
	class str_rc  : public str_ptr
	{
	protected:

	    void own();
	    void lose();
	    void bind(zend_string* rc);

	    friend class str_ptr;
	    friend class val_rc;

	public:

		enum {
			LTRIM = 1,
			RTRIM = 2,
			LRTRIM = 3
		};

		static void try_addref(zend_string* zs)
		{
			if (zs->gc.u.type_info & IS_STR_INTERNED)
			{
				return;
			}
			++zs->gc.refcount;
		}

		static void try_decref(zend_string* zs)
     	{
			zend_string_release(zs);
		}

		~str_rc();

	    str_rc() : str_ptr()
	    {   
	    }
		
	    str_rc(zend_string* p) : str_ptr(p)
	    {
	        own();
	    }

		str_rc(str_rc&& rc) : str_ptr(rc.s)
	    {
	        rc.s = nullptr;
	    }

		str_rc(const str_rc& rc) : str_ptr(rc.s)
	    {
	        own();
	    }

	    str_rc(const str_ptr& su) : str_ptr(su.s)
	    {
	    	own();
	    }

	    str_rc(zval* copy);

	    str_rc(const val_ptr& rc);

	    str_rc(val_rc&& rc);

	    str_rc(zend_long ival);
	    
	    str_rc(str_buf&& m);

	   

	    bool ok() const {
	    	return (s);
	    }


	    bool isNull() const {
	    	return !(s);
	    }
	    
	    void init();
	    
	    
	    
	    size_t size() const;
	    const char* data() const;

	    const str_rc& operator=(const str_rc& rc);
	    const str_rc& operator=(zend_string* rc);
	    const str_rc& operator=(zval* rc);
		 
		const str_rc& operator=(str_buf&& m);

	    str_rc& operator=(val_rc&& rc);
	    str_rc& operator=(str_rc&& rc);
	    str_rc& operator=(str_temp&& rc);

	    void move_zv(zval* ret);

		void return_zv(zval* ret);

	    void adopt(zend_string* rc);

	    operator zend_string*() const { return (zend_string*) s; }

	    const str_rc& operator=(const str_ptr& rc);

	    static str_rc base64_decode(const unsigned char* c, size_t slen);
	    static str_rc base64_encode(const unsigned char* c, size_t slen);

	    void  lowercase();
	    void  uppercase();

		str_rc to_lower();
		str_rc to_upper();

		str_rc trim(const char* what = (const char*) nullptr, int mode = LRTRIM);

	    void  trim_self(const char* what = (const char*) nullptr, int mode = LRTRIM);
	};

	class str_empty : public str_rc
	{
	public:
		str_empty();
	};
	
	/** A "persistent" string, not using emalloc and efree */
	class str_perm : public str_rc {
	public:
		str_perm() : str_rc() {}
		str_perm(const char* c, size_t slen = 0);

		operator zend_string*() const { return (zend_string*) s; }
	};

	/** A "temporary" string, during a request, uses emalloc and efree */
	class str_temp : public str_rc {
	public:
		str_temp() : str_rc() {}
		str_temp(const char* c, size_t slen = 0);

		operator zend_string*() const { return (zend_string*) s; }
	};
	
	/** A "persistent" string stored as "interned", for module/class initialize */
	class str_intern : public str_rc {
	public:
		str_intern() : str_rc() {}
		str_intern(const char* c, size_t slen = 0);

		~str_intern() { 
			// let PHP take care of it
			s = nullptr; 
		}

		const str_intern& operator=(const char* cp);
		
		operator zend_string*() const { return (zend_string*) s; }
	};



}; // namespace Php

//str_rc.h
#endif