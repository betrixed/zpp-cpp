#ifndef BIRETURN_H
#define BIRETURN_H

#ifndef STR_BUF_H
#include "str_buf.h"
#endif

namespace zpp {

struct serror {
	str_buf*  errors_;

	serror() : errors_(nullptr) {}

	serror(serror&& m) {
		errors_ = m.errors_;
		m.errors_ = nullptr;
	}

	const serror& operator=(serror&& m)
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

	~serror() {
		del_errors();
	}

};


template <typename T>
struct bireturn  : public serror 
{
	T   	  value_;

	bireturn() : serror()
	{
	}

	bireturn(bireturn&& m) : serror()
	{
		errors_ = m.errors_;
		m.errors_ = nullptr;
		value_ = m.value_;
		m.value_ = T();
	}

	bireturn(T&& v) : serror()
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

	const bireturn& operator=(serror&& e)
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