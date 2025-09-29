/* wccr extension for PHP */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#define ZPP_BUILD_ALL

#include "php_wccr.h"

#include "zpp/base.h"
#include "zpp/show_zpp.h"

// compile 
#include "wcc/icache.cpp"
#include "wcc/icachedata.cpp"
#include "wcc/file_upload.cpp"
#include "wcc/global_response.cpp"
#include "wcc/pair.cpp"
#include "wcc/request_globals.cpp"
#include "wcc/route.cpp"
#include "wcc/route_match.cpp"
#include "wcc/route_set.cpp"
#include "wcc/target.cpp"


PHP_MINIT_FUNCTION(wccr)
{
#ifdef GLOBAL_RESPONSE_CPP
	PHP_MINIT(Wcc_Response_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif
#ifdef REQUEST_GLOBALS_CPP
	PHP_MINIT(RequestGlobals_reg)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(FileUpload_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCC_PAIR_CPP
PHP_MINIT(wcc_pair_d)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCC_ROUTE_CPP
	PHP_MINIT(wcc_route_d)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef ROUTE_MATCH_CPP
	PHP_MINIT(route_match_d)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCC_ROUTESET_CPP
	PHP_MINIT(wcc_routeset_d)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCC_TARGET_CPP
		PHP_MINIT(wcc_target)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef ICACHE_CPP
	PHP_MINIT(Wcc_ICacheData_reg)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(Wcc_ICache_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif
	return SUCCESS;
}

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(wccr)
{
#if defined(ZTS) && defined(COMPILE_DL_WCCR)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(wccr)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "wccr support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ wccr_module_entry */
zend_module_entry wccr_module_entry = {
	STANDARD_MODULE_HEADER,
	"wccr",					/* Extension name */
	nullptr,					/* zend_function_entry */
	PHP_MINIT(wccr),			/* PHP_MINIT - Module initialization */
	nullptr,					/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(wccr),			/* PHP_RINIT - Request initialization */
	nullptr,					/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(wccr),			/* PHP_MINFO - Module info */
	PHP_WCCR_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_WCCR
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(wccr)
#endif
