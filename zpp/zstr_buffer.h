#ifndef ZSTR_BUFFER_H
#define ZSTR_BUFFER_H

#ifndef ZSTR_OUTPUT_H
#include "zstr_output.h"
#endif

extern "C" {
	#include <Zend/zend_smart_str.h>
};


namespace zpp {


//! zstr_buffer with memory layout as "smart_string"
	class zstr_buffer : public zstr_output {
	protected:
		smart_str    buf;

		static zend_string* init_zs(const char* c, size_t slen);

		friend class zstr_mgr;

		void initbuf();

		void lose();

		// for use by friends
		zend_string* finalize();
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

		
		// Finalize, 0-terminate, return as std::string, 
		std::string str();

		// Finalize, 0-terminate give away
		zstr_mgr zstr();

		// view of content so far, does not finalize
		std::string_view vstr() const;

		void reset(); // release string, start again

		const char* data() const;
		size_t size() const;
	};

	
};

//zstr_buffer.h
#endif