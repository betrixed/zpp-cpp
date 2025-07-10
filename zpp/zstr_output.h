#ifndef ZSTR_OUTPUT_H
#define ZSTR_OUTPUT_H

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

	class fm_endl {

	};

	extern fm_endl endl;
	
	class zstr_output {
	protected:
		iform   	 nf_;
	public:
		virtual void append(const char* c, size_t slen);

		virtual void append(char c);

		virtual ~zstr_output() {}

		void append(zend_string* s);

		zstr_output& operator<<(const iform& form);

		zstr_output& operator<<(const fm_endl& el);

		zstr_output& operator<<(const zstr_mgr &w);
		
		zstr_output& operator<<(zstr_user w);

		zstr_output& operator<<(zend_string* s);

		zstr_output& operator<<(const char* c);

		zstr_output& operator<<(size_t nn);

		zstr_output& operator<<(void* vp);

		zstr_output& operator<<(double d);

		zstr_output& operator<<(int iv);

		zstr_output& operator<<(unsigned int iv);

		zstr_output& operator<<(long iv);

		zstr_output& operator<<(char c);

		zstr_output& operator<<(const std::string_view &v);

		zstr_output& operator<<(zval* zv);

		void quote_name(const char* name);
		void quote_name(zend_string* name);
	};

}; // namespace
#endif