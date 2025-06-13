 /*  
  *  PHP extension C++ classes - zpp 
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  */

#ifndef ZSTR_USER_H
#define ZSTR_USER_H

#ifndef PHP_EXTERN_H
#include "php_extern.h"
#endif

namespace zpp {

	class zstr_mgr;
	class zval_user;

	class zstr_intern;

	class zstr_user {
	protected:
	    zend_string* s;

	    friend class zstr_mgr;
	    friend class zval_mgr;
	public:

		static const char* empty;

		enum {
			LTRIM = 1,
			RTRIM = 2,
			LRTRIM = 3
		};

		zstr_user() : s( (zend_string*) nullptr) {}

	    zstr_user(zend_string* p)
	    {
	        s = p;
	    }   

	    zstr_user(zval* p);

	    zstr_user(const zstr_user& rc)
	    {
	        s = rc.s;
	    }

	    //! Can't declare inline here
	    zstr_user(const zval_user& rc);

	    zstr_user(const zstr_mgr& mgr);

	    zstr_user(const zstr_intern& zs);

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

		zstr_mgr substr(int offset, int len=INT_MAX) const;

		zstr_mgr to_lower() const;

		zstr_mgr to_upper() const;

		zstr_mgr uncamel(const char* sep = nullptr) const;

		zstr_mgr trim(const char* what = (const char*) nullptr,
		 				int mode = LRTRIM) const;

		zstr_mgr strtr(const char* from, const char* to) const;
		
		int find(char c, size_t pos=0) const;
		int rfind(char c, size_t pos = INT_MAX) const;
		int find(const std::string_view& needle, size_t pos) const;

		int strpos(zstr_user needle);
		
		bool contains(zstr_user needle);

		void return_zv(zval* ret);

		bool starts_with(zstr_user match) const;

		bool ends_with(zstr_user match) const;
		
		const zstr_user& operator=(zval* rc);

		static zstr_mgr json_encode(zval_user value, int flags);
	};

	int zs_cmp(zend_string* a, zend_string* b);
	int zs_cmp_ci(zend_string* a, zend_string* b);

	zstr_mgr str_replace( zstr_user mstr, zstr_user rstr, zstr_user subject );

	zstr_mgr str_replace(const std::string_view& match, 
						 const std::string_view& replace,
						 zstr_user subject);

};


//zstr_user.h
#endif
