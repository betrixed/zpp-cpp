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

	class zstr_output {
	protected:
		iform   	 nf_;
	public:
		virtual void append(const char* c, size_t slen);

		virtual void append(char c);

		virtual ~zstr_output() {}

		void append(zend_string* s);

		zstr_output& operator<<(const iform& form);

		zstr_output& operator<<(const zstr_mgr &w);
		
		zstr_output& operator<<(zstr_user w);

		zstr_output& operator<<(zend_string* s);

		zstr_output& operator<<(const char* c);

		zstr_output& operator<<(size_t nn);

		zstr_output& operator<<(void* vp);

		zstr_output& operator<<(double d);

		zstr_output& operator<<(int iv);

		zstr_output& operator<<(long iv);

		zstr_output& operator<<(char c);

		zstr_output& operator<<(const std::string_view &v);

		zstr_output& operator<<(zval* zv);

		void quote_name(const char* name);
	};

//! zstr_buffer with memory layout as "smart_string"
	class zstr_buffer : public zstr_output {
	protected:
		smart_str    buf;

		static zend_string* init_zs(const char* c, size_t slen);

		friend class zstr_mgr;

		void initbuf()
		{
			buf.s = nullptr;
			buf.a = 0;
		}

		void lose();

	public:

		zstr_buffer();

		zstr_buffer(zval *v);

		zstr_buffer(zend_string* w);

		zstr_buffer (const std::string_view& cs);

		zstr_buffer (const std::string& cs);

		zstr_buffer (const char* c, size_t slen);

		zstr_buffer (const char* c);

		virtual ~zstr_buffer();

		virtual void append(const char* c, size_t slen);

		virtual void append(char c);

		size_t capacity() const { return buf.a; }

		zstr_buffer& operator=(const char* c);

		
		// Finalize, 0-terminate, return std::string copy, 
		std::string str();

		// Finalize, give away the zend_string* (result needs adoption)
		zend_string* zstr();

		void reset(); // release string, start again

		size_t size() 
		{
			if (buf.s) {
				return ZSTR_LEN(buf.s);
			}
			else {
				return 0;
			}
		}
	};

	
};

//zstr_buffer.h
#endif