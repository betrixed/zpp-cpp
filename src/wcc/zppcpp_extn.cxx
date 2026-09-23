#ifndef ZPPCPP_EXTN_CXX
#define ZPPCPP_EXTN_CXX

#include "wcc/strfns.cpp"
#include "wcc/debuglog.cpp"


void register_zppcpp_extn(INIT_FUNC_ARGS)
{

#ifdef WCC_DEBUGLOG_CPP
	PHP_MINIT(zpp_debuglog_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif
	
	error_return::init_exception_hook();

}

#endif