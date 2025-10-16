 /*  @file str_ptr.h
  *  @brief str_ptr class, holds a zend_string pointer.	

  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2025 Michael Rynn
  *  @license BSD 3-Clause License
  */

#ifndef STR_PTR_H
#define STR_PTR_H

#ifndef PHP_EXTERN_H
#include "php_extern.h"
#endif

namespace zpp {

	class str_rc;
	class val_ptr;

	class str_intern;

	/**
	 * @class str_ptr
	 * @brief str_ptr class, holds a zend_string pointer. Not reference counted.
	 * 
	 */
	class str_ptr {
	protected:
	    zend_string* s;

	    friend class str_rc;
	    friend class val_rc;
	public:

		static const char* empty;

		str_ptr() : s( (zend_string*) nullptr) {}

	    str_ptr(zend_string* p)
	    {
	        s = p;
	    }   

	    str_ptr(zval* p);

	    str_ptr(const str_ptr& rc)
	    {
	        s = rc.s;
	    }

	    //! Can't declare inline here
	    str_ptr(const val_ptr& rc);

	    str_ptr(const str_rc& mgr);

	    str_ptr(const str_intern& zs);

	    //const str_ptr& operator=(const str_rc& zm);

	    operator zend_string*() const { return (zend_string*) s; }

		const char* data() const;

		size_t size() const;

		double getDouble() const;

		long getLong(int base = 10) const;

		std::string cstr() const;

		std::string_view vstr() const;	

		bool ok() const { return (s); }
		bool isNull() const { return !(s); }

		std::string_view  subview(int offset, int len) const;

		str_rc substr(int offset, int len=INT_MAX) const;

		str_rc uncamel(const char* sep = nullptr);

		str_rc strtr(const char* from, const char* to);
		
		str_rc ucfirst();

		int find(char c, size_t pos=0) const;
		int rfind(char c, size_t pos = INT_MAX) const;
		int find(const std::string_view& needle, size_t pos) const;

		int strpos(str_ptr needle);
		
		bool contains(str_ptr needle);

		void return_zv(zval* ret) const;

		bool starts_with(str_ptr match) const;

		bool ends_with(str_ptr match) const;

		str_rc duplicate() const;

		int  strcmp(const char* bstr) const;
		
		const str_ptr& operator=(zval* rc);

		static str_rc json_encode(val_ptr value, int flags);

		static str_ptr empty_str();
	};

	int zs_cmp(zend_string* a, zend_string* b);
	int zs_cmp_ci(zend_string* a, zend_string* b);

	str_rc str_replace( str_ptr mstr, str_ptr rstr, str_ptr subject );

	str_rc str_replace(const std::string_view& match, 
						 const std::string_view& replace,
						 str_ptr subject);

	str_rc operator+(str_ptr lhs, str_ptr rhs);

};



//str_ptr.h
#endif
