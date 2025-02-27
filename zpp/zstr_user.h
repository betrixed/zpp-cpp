 /*  
  *  PHP extension C++ classes - zpp 
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  */

#ifndef ZSTR_USER_H
#define ZSTR_USER_H

namespace zpp {

	class zstr_mgr;
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

	    zstr_user(const zstr_user &rc)
	    {
	        s = rc.s;
	    }

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


	};
};


//zstr_user.h
#endif
