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

//#define DBG_ERROR_RETURN

#ifdef DBG_ERROR_RETURN
#ifndef WCC_DEBUGLOG_H
#include "wcc/debuglog.h"
#endif
#endif

namespace zpp {


void  
error_return::on_zend_exception(zend_object* zex)
{
	if (zex)
	{
		last_exception = zex;

		#ifdef DBG_ERROR_RETURN
		wcc::DebugLog* log = wcc::DebugLog::cpp_global();
		if (log) {
			log->dump("zend_exception", zex);
		}
		#endif

		had_zend_exception = true;
	}
	if (original_zthrow_hook != nullptr)
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
// See if zend engine exception hook has message waiting..
bool 
error_return::did_zend_throw()
{
	#ifdef DBG_ERROR_RETURN
	wcc::DebugLog* log = wcc::DebugLog::cpp_global();
	#endif

	if (!had_zend_exception)
	{
		#ifdef DBG_ERROR_RETURN
		if (log) {
			log->line("NO zend_exception");
		}
		#endif
		return false;
	}
	#ifdef DBG_ERROR_RETURN
	if (log) {
			log->dump("YES! zend_exception", last_exception);
	}
	#endif

	fn_call excall(FTAB.getmessage_fn, last_exception);
	fn_noparams fn(excall);
	str_rc err_msg = fn.str();

	error() << " Zend threw: " << err_msg;
	// TODO: check on contents of these?
	has_thrown_ = true;
	last_exception.init();
	had_zend_exception = false;

	return true;

}
bool error_return::has_errors() 
{ 
	return (errors_) ? true : did_zend_throw();
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

// return a copy of appended string buffer 
str_rc 
error_return::error_str()
{
	str_rc result;

	if (errors_)
	{	
		result = str_rc(errors_->data(), errors_->size());
	}
	return result;
}


bool 
error_return::throw_errors(const char* fncstr)
{
	if (errors_ || did_zend_throw())
	{
		//zend_prin  tf("\nThrow errors \n");
		if (!has_thrown_)
		{
			
			*errors_ << "\n<br> error_return " << fncstr;
			str_rc s = errors_->zstr();
		//zend_printf("\nThrow errors %s\n", s.data());
			zend_throw_error(zend_ce_error,"%s", s.data());


			// problem, of sorts, creating
			// this exception, will cause it to appear 
			// on own exception hook function,
			// Clear any new intercepted exception.
			last_exception.init();
			had_zend_exception = false;
			has_thrown_ = true;
		} // else do not gazump thrown exception



		return true;
	}
	return false;
}

}//namespace
#endif