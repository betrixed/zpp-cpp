#ifndef ZSTR_BUFFER_H
#define ZSTR_BUFFER_H

#ifndef ZSTR_OUTPUT_H
#include "str_out.h"
#endif

extern "C" {
	#include <Zend/zend_smart_str.h>
};


namespace zpp {


//! str_buf with memory layout as "smart_string"
	class str_buf : public str_out {
	protected:
		smart_str    buf;

		static zend_string* init_zs(const char* c, size_t slen);

		friend class str_rc;

		void initbuf();

		void lose();

		// for use by friends
		zend_string* finalize();
	public:

		str_buf();

		str_buf(zval *v);

		str_buf(zend_string* w);

		str_buf (const std::string_view& cs);

		str_buf (const std::string& cs);

		str_buf (const char* c, size_t slen);

		str_buf (const char* c);

		virtual ~str_buf();

		void append(const char* c, size_t slen) override;

	    void append(char c) override;

		size_t capacity() const { return buf.a; }

		str_buf& operator=(const char* c);

		
		// Finalize, 0-terminate, return as std::string, 
		std::string str();

		// Finalize, 0-terminate give away
		str_rc zstr();

		// view of content so far, does not finalize
		std::string_view vstr() const;

		void reset(); // release string, start again

		const char* data() const;
		size_t size() const;
	};

	
};

//str_buf.h
#endif