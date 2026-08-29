#ifndef BIRETURN_CPP
#define BIRETURN_CPP

/**
 * @file zpp/bireturn.cpp
 * @author Michael Rynn <michael.rynn.500@gmail.com>
 * @brief Return types with error report propagation
 * @copyright 2025 Michael Rynn
 * @license BSD 3-Clause License
 */

#ifndef BIRETURN_H
#include "bireturn.h"
#endif



namespace zpp {

bool  error_return::had_zend_exception = false;
void  (*error_return::original_zthrow_hook)(zend_object *zex) = NULL;
str_rc error_return::last_msg = str_rc();

void  
error_return::on_zend_exception(zend_object* zex)
{
	
	if (zex)
	{
		str_rc msg("getmessage");
		fn_call excall(msg, zex);
		fn_noparams fn(excall);
		last_msg = fn.str();
		had_zend_exception = true;
	}
	if (original_zthrow_hook != NULL)
	{
		original_zthrow_hook(zex);
	}
}

void 
error_return::init_exception_hook()
{
	original_zthrow_hook = zend_throw_exception_hook;
	zend_throw_exception_hook = on_zend_exception;
	had_zend_exception = false;
}

void     
error_return::del_errors()
	{
		if (errors_)
		{
			//zend_printf("del_errors %s %lx\n", errors_->data(), (uint64_t) this);
			delete errors_;
			errors_ = nullptr;
		}
	}

bool error_return::has_errors() const 
{ 
	return bool(errors_) || had_zend_exception; 
}

str_buf& 
error_return::error()
{
	if (!errors_)
	{
		errors_ = new str_buf();
	}
	return *errors_;
}

str_rc 
error_return::get_errors()
{
	str_rc result;

	if (errors_)
	{	
		result =  errors_->zstr();
		delete errors_;
		errors_ = nullptr;
	}
	return result;
}

bool 
error_return::throw_errors(const char* fncstr)
{
	if (had_zend_exception)
	{
		// zend engine has already thrown something.
		error() << " Zend threw: " << last_msg;
		last_msg.init();
		had_zend_exception = false;
	}
	if (errors_)
	{
		//zend_printf("\nThrow errors \n");
		
		*errors_ << "\n<br> error_return " << fncstr;
		str_rc s = errors_->zstr();
		//zend_printf("\nThrow errors %s\n", s.data());
		zend_throw_error(zend_ce_error,"%s", s.data());
		delete errors_;
		errors_ = nullptr;
		return true;
	}
	return false;
}

}//namespace
#endif