#ifndef BIRETURN_H
#define BIRETURN_H

#ifndef STR_BUF_H
#include "str_buf.h"
#endif

namespace zpp {

//TODO: replace str_buf, has_thrown_ with a Throwable object
struct error_return {

	inline static thread_local bool  had_zend_exception = false;
	inline static thread_local void (*original_zthrow_hook)(zend_object *zex) = nullptr;

	inline static thread_local  obj_rc last_exception  = obj_rc();

	static void  on_zend_exception(zend_object* zex);
	static void init_exception_hook();

	str_buf*  errors_;
	bool      has_thrown_;

	error_return() : errors_(nullptr), has_thrown_(false) {}

	bool did_zend_throw();

	error_return(error_return&& m) {
		errors_ = m.errors_;
		has_thrown_ = m.has_thrown_;
		m.has_thrown_ = false;
		m.errors_ = nullptr;
	}

	error_return& operator=(error_return&& m)
	{
		del_errors();
		errors_ = m.errors_;
		has_thrown_ = m.has_thrown_;
		m.has_thrown_ = false;
		m.errors_ = nullptr;
		return *this;
	}

	bool has_errors();

	str_buf& error();

	str_rc   error_str();

	error_return&& move_error() {
		return std::move(*this);
	}

	bool     throw_errors(const char* fncstr = nullptr);

	void     del_errors();

	~error_return() {
		del_errors();
	}

};


template <typename T>
struct bireturn  : public error_return 
{
	T   	  value_ {};

	bireturn() : error_return()
	{
	}

	bireturn(bireturn&& m) : error_return()
	{
		errors_ = m.errors_;
		has_thrown_ = m.has_thrown_;
		m.errors_ = nullptr;
		m.has_thrown_ = false;

		value_ = m.value_;
		m.value_ = T();
	}

	bireturn(T&& v) : error_return()
	{	
		value_ = v;
	}
/*
	operator T& () {
		return value_;
	}
*/
	const bireturn& operator=(const T& c)
	{
		value_ = c;
		return *this;
	}

	bool operator==(const T& c)
	{
		return (value_ == c);
	}

	bireturn& operator=(error_return&& e)
	{
		del_errors();
		errors_ = e.errors_;
		has_thrown_ = e.has_thrown_;
		e.errors_ = nullptr;
		return *this;
	}

	T&& move_value()
	{
		return std::move(value_);
	}
	
	bireturn& operator=(bireturn&& m)
	{
		del_errors();
		errors_ = m.errors_;
		has_thrown_ = m.has_thrown_;
		m.errors_ = nullptr;
		value_ = m.value_;
		m.value_ = T();
		return *this;
	}
};

}//namespace

#endif