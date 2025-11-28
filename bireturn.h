#ifndef BIRETURN_H
#define BIRETURN_H

#ifndef STR_BUF_H
#include "str_buf.h"
#endif

namespace zpp {

struct error_return {
	str_buf*  errors_;

	error_return() : errors_(nullptr) {}

	error_return(error_return&& m) {
		errors_ = m.errors_;
		m.errors_ = nullptr;
	}

	const error_return& operator=(error_return&& m)
	{
		errors_ = m.errors_;
		m.errors_ = nullptr;
		return *this;
	}

	bool has_errors() const { return (errors_); }

	str_buf& error();

	str_rc   get_errors();

	bool     throw_errors(const char* fncstr = nullptr);

	void     del_errors();

	~error_return() {
		del_errors();
	}

};


template <typename T>
struct bireturn  : public error_return 
{
	T   	  value_;

	bireturn() : error_return()
	{
	}

	bireturn(bireturn&& m) : error_return()
	{
		errors_ = m.errors_;
		m.errors_ = nullptr;
		value_ = m.value_;
		m.value_ = T();
	}

	bireturn(T&& v) : error_return()
	{	
		value_ = v;
	}

	operator T& () {
		return value_;
	}

	const bireturn& operator=(const T& c)
	{
		value_ = c;
		return *this;
	}

	bool operator==(const T& c)
	{
		return (value_ == c);
	}

	const bireturn& operator=(error_return&& e)
	{
		errors_ = e.errors_;
		e.errors_ = nullptr;
		return *this;
	}

	const bireturn& operator=(bireturn&& m)
	{
		errors_ = m.errors_;
		m.errors_ = nullptr;
		value_ = m.value_;
		m.value_ = T();
		return *this;
	}
};

}//namespace

#endif