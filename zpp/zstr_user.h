 /*  
  *  PHP extension C++ classes - zpp 
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  */

#ifndef ZSTR_USER_H
#define ZSTR_USER_H

namespace zpp {

	class zstr_mgr;
	class zval_user;

	class ZPP_EXPORT zstr_user {
	protected:
	    zend_string* s;

	    friend class zstr_mgr;

	public:
		enum {
			LTRIM = 1,
			RTRIM = 2,
			LRTRIM = 3
		};

		static const char* empty_zstr;

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

	    zstr_user(const zstr_mgr& mgr)
	    {
	        s = mgr.s;
	    }

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

		zstr_mgr trim(const char* what = (const char*) nullptr,
		 				int mode = LRTRIM) const;

		zstr_mgr strtr(const char* from, const char* to) const;
		
		int find(char c, size_t pos=0) const;

		void return_zv(zval* ret);

		bool starts_with(zstr_user match);
		
		const zstr_user& operator=(zval* rc);


		
	};

	int zs_cmp(zend_string* a, zend_string* b);
	int zs_cmp_ci(zend_string* a, zend_string* b);
};


//zstr_user.h
#endif
