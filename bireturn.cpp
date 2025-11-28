#ifndef BIRETURN_CPP
#define BIRETURN_CPP

#ifndef BIRETURN_H
#include "bireturn.h"
#endif

namespace zpp {



void     
error_return::del_errors()
	{
		if (errors_)
		{
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
		*errors_ << "*** ERROR " << fncstr;
		str_rc s = errors_->zstr();
		zend_throw_error(zend_ce_error,"%s", s.data());
		delete errors_;
		errors_ = nullptr;
		return true;
	}
	return false;
}

}//namespace
#endif