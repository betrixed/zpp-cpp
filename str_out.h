#ifndef STR_OUT_H
#define STR_OUT_H

/**
 * @file zpp/str_out.h
 * @author Michael Rynn <michael.rynn.500@gmail.com>
 * @brief str_out class, string buffer with zend_string memory layout.	
 * @copyright Copyright (c) 2025 Michael Rynn
 * @license BSD 3-Clause License
 */


#ifndef STR_RC_H
#include "str_rc.h"
#endif

#ifndef STR_PTR_H
#include "str_ptr.h"
#endif

namespace zpp {
	
	enum Numf {
		DEC,
		HEX
	};

	/**
	 * @class iform
	 * @brief  format numeric base for numbers output
	 */
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

	/**
	 * @class fm_endl
	 * @brief  class to signal output of end line character.
	 */
	class fm_endl {

	};

	/** 
	 * @brief Instance to signal ouptut of end line character.
	 */
	extern fm_endl endl;
	
	/**
	 * @class str_out
	 * @brief Output text string to PHP write handler.
	 * @details Use with output operator <<.d
	 */

	class str_out {
	protected:
		iform   	 nf_;
	public:
		virtual void append(const char* c, size_t slen);

		virtual void append(char c);

		virtual ~str_out() {}

		void append(zend_string* s);

		str_out& operator<<(const iform& form);

		str_out& operator<<(const fm_endl& el);

		str_out& operator<<(const str_rc &w);
		
		str_out& operator<<(val_ptr zval);

		str_out& operator<<(str_ptr w);

		str_out& operator<<(zend_string* s);

		str_out& operator<<(const char* c);

		str_out& operator<<(size_t nn);

		str_out& operator<<(void* vp);

		str_out& operator<<(double d);

		str_out& operator<<(int iv);

		str_out& operator<<(unsigned int iv);

		str_out& operator<<(long iv);

		str_out& operator<<(char c);

		str_out& operator<<(const std::string_view &v);

		str_out& operator<<(zval* zv);

		void quote_name(const char* name);
		void quote_name(zend_string* name);
	};

}; // namespace
#endif