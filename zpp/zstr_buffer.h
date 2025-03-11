#ifndef ZSTR_BUFFER_H
#define ZSTR_BUFFER_H

#ifndef ZSTR_MGR_H
#include "zstr_mgr.h"
#endif

#ifndef ZSTR_USER_H
#include "zstr_user.h"
#endif

namespace zpp {


enum Numf {
	DEC,
	HEX
};

class iform {
public:
	Numf value_;

	iform()
	{
		value_ = Numf::DEC;
	}

	iform(Numf nf) : value_(nf) 
	{

	}
};

//! zstr_buffer with memory layout as "smart_string"
	class zstr_buffer 
	{
	protected:
		zend_string* s;
		size_t  	 a;  
		iform   	 nf_;

		static zend_string* init_zs(const char* c, size_t slen);

		friend class zstr_mgr;

	public:

		zstr_buffer() : s(nullptr), a(0) {}

		zstr_buffer(zval *v);

		zstr_buffer(zend_string* w);

		zstr_buffer (const std::string_view& cs);

		zstr_buffer (const std::string& cs);

		zstr_buffer (const char* c, size_t slen);

		zstr_buffer (const char* c);

		virtual ~zstr_buffer();

		virtual void append(const char* c, size_t slen);

		virtual void append(zend_string* s);

		virtual void append(char c);

		size_t len() const { return a; }

		void quote_name(const char* name);

		zstr_buffer& operator=(const char* c);

		zstr_buffer& operator<<(const iform& form);

		zstr_buffer& operator<<(const zstr_mgr &w);
		
		zstr_buffer& operator<<(zstr_user w);

		zstr_buffer& operator<<(zend_string* s);

		zstr_buffer& operator<<(const char* c);

		zstr_buffer& operator<<(size_t nn);

		zstr_buffer& operator<<(void* vp);

		zstr_buffer& operator<<(double d);

		zstr_buffer& operator<<(int iv);

		zstr_buffer& operator<<(long iv);

		zstr_buffer& operator<<(char c);

		zstr_buffer& operator<<(const std::string_view &v);

		// requires non-inline
		zstr_buffer& operator<<(zval* zv);

		// Finalize, 0-terminate, return std::string copy, 
		std::string str();

		// Finalize, give away the zend_string* (result needs adoption)
		zend_string* zstr();

		void reset(); // release string, start again

		size_t size() 
		{
			if (s) {
				return ZSTR_LEN(s);
			}
			else {
				return 0;
			}
		}
	};

	class zstr_output : public zstr_buffer {
	public:
		virtual void append(const char* c, size_t slen);

		virtual void append(zend_string* s);

		virtual void append(char c);

	};
};

//zstr_buffer.h
#endif