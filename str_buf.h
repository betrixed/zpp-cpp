#ifndef STR_BUF_H
#define STR_BUF_H

/**
 * @file zpp/str_buf.h 
 * @author Michael Rynn <michael.rynn.500@gmail.com>
 * @brief str_buf class, string buffer with zend_string memory layout.	
 * @copyright Copyright (c) 2025 Michael Rynn
 * @license BSD 3-Clause License
 */

#ifndef STR_OUT_H
#include "str_out.h"
#endif

#include <cstdarg>
#include <string>


extern "C" {
	#include <Zend/zend_smart_str.h>
};


namespace zpp {

	/**
	 * @class str_buf
	 * @brief String buffer using output operator <<
	 * @details Implementation uses PHP smart_str structure and functions.
	 * Overrides str_out append methods to add to buffer.
	 * Finalize method creates a zend_string from the buffer.
	 * The buffer is then reset to empty.
	 * The buffer is always null terminated.
	 */
//! str_buf with memory layout as "smart_string"
	class str_buf : public str_out {
	protected:
		// temporarystore final, for suspected reference drops
		str_rc 		 final_; 
		smart_str    buf;

		static zend_string* init_zs(const char* c, size_t slen);

		friend class str_rc;

		void initbuf();

		void lose();

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

		void printf(const char *format, ...);

		// Finalize, 0-terminate, return as std::string, 
		std::string str();

		// Finalize, 0-terminate give away
		str_ptr zstr();

		// view of content so far, does not finalize
		std::string_view vstr() const;

		void reset(); // release string, start again

		const char* data() const;
		size_t size() const;
	};

	
};

//str_buf.h
#endif