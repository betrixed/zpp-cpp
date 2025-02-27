#ifndef ZSTR_BUFFER_H
#define ZSTR_BUFFER_H

#ifndef ZSTR_MGR_H
#include "zstr_mgr.h"
#endif

#ifndef ZSTR_USER_H
#include "zstr_user.h"
#endif

namespace zpp {

//! Same memory layout as "smart_string"

class zstr_buffer : public zstr_mgr
	{
	protected:
		size_t  a;  

		static zend_string* init_zs(const char* c, size_t slen);

	public:

		zstr_buffer() : zstr_mgr(), a(0) {}

		zstr_buffer(zval *v);

		zstr_buffer(zend_string* w);

		zstr_buffer (const std::string_view& cs);


		zstr_buffer (const std::string& cs);


		zstr_buffer (const char* c, size_t slen);


		zstr_buffer (const char* c);

		zstr_buffer& append(zend_string* s);

		zstr_buffer& append(zstr_user w) {
			return append((zend_string*) w);
		}

		size_t len() const { return a; }

		//size_t items() const { return listct_; }

		zstr_buffer& operator=(const char* c)
		{
			reset();
			return append(c, strlen(c));
		}	

		//zstr_buffer& operator<<(const bfmt& bf);


		zstr_buffer& operator<<(zstr_user w)
		{
			return append(w);
		}

		zstr_buffer& operator<<(zend_string* s)
		{
			return append(s);
		}

		zstr_buffer& operator<<(const char* c)
		{
			return append(c, strlen(c));
		}
		/*
		void item_sep(char c = ',') 
		{
			sep_ = c;
		}
		*/

		zstr_buffer& operator<<(int iv);

		zstr_buffer& append(const char* c, size_t slen);

		zstr_buffer& operator<<(char c);

		zstr_buffer& operator<<(const std::string_view &v);

		// requires non-inline
		zstr_buffer& operator<<(zval* zv);

		// Finalize, 0-terminate, return std::string copy, 
		std::string str();

		// Finalize, give away the zend_string* (result needs adoption)
		zend_string* zstr();

		void reset(); // release string, start again
	};

};

//zstr_buffer.h
#endif