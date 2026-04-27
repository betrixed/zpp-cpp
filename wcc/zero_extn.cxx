#ifndef CORE_EXTN_CXX
#define CORE_EXTN_CXX


#include "wcc/replace.cpp"
#include "wcc/config.cpp"

#include "wcc/reflect_cache.cpp"
#include "wcc/services.cpp"

#include "wcc/strfns.cpp"
//#include "wcc/persistent.cpp"

void register_zero_extn(INIT_FUNC_ARGS)
{

#ifdef WCC_CONFIG_CPP
	PHP_MINIT(wcc_replace_reg)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(Wcc_Config_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif


#ifdef WCC_SERVICES_CPP
	PHP_MINIT(wc_services_md)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(Wcc_ReflectCache)(INIT_FUNC_ARGS_PASSTHRU);
#endif

/*
#ifdef PERSISTENT_WCC_CPP
	PHP_MINIT(wcc_persistent_md);
#endif
*/
	
#ifdef STRFNS_CPP
	PHP_MINIT(Strfns_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif
}


#endif