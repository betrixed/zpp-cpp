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
	if (errors_)
	{
		//zend_printf("\nThrow errors \n");

		*errors_ << "\n*** ERROR " << fncstr;
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