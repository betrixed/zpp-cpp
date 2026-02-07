/* wccr extension for PHP */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#define ZPP_BUILD_ALL

#define DIRECT_XML

#include "php_wccr.h"

#include "zpp/base.h"
#include "zpp/show_zpp.h"

// compile in binary

#include "wcc/finder.cpp"
#include "wcc/loader.cpp"

//#include "wcc/reflect_cache.cpp"
//#include "wcc/services.cpp"
#include "wcc/service_access.cpp"

#include "wcc/hmap.cpp"
//#include "wcc/replace.cpp"


#include "wcc/icachedata.cpp"
#include "wcc/dircache.cpp"
// ICache handles registration
#include "wcc/icache.cpp"


#include "wcc/cachemgr.cpp"


#include "wcc/file_upload.cpp"
#include "wcc/global_response.cpp"
#include "wcc/pair.cpp"
#include "wcc/request_globals.cpp"

#include "wcc/route.cpp"
#include "wcc/route_match.cpp"
#include "wcc/route_set.cpp"
#include "wcc/target.cpp"

// CacheMgr wants one of these
#ifdef DIRECT_XML
#include "tinyxml/dxmlread.cpp"
#else
#include "wcc/xmlread.cpp"
#endif

PHP_MINIT_FUNCTION(wccr)
{

#ifdef WCC_LOADER_CPP
	PHP_MINIT(wcc_loader_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCC_FINDER_CPP
	PHP_MINIT(Wcc_Finder_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCC_CONFIG_CPP
	PHP_MINIT(wcc_replace_reg)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(Wcc_Config_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCC_HMAP_CPP
	PHP_MINIT(Wcc_Hmap_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCC_SERVICES_CPP
PHP_MINIT(wc_services_md)(INIT_FUNC_ARGS_PASSTHRU);
PHP_MINIT(Wcc_ReflectCache)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef SERVICE_ACCESS_CPP
	PHP_MINIT(ServiceAccess_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

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
	PHP_MINIT(Wcc_ICache_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCC_CACHEMGR_CPP
	PHP_MINIT(Wcc_CacheMgr_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif


#	ifdef DXMLREAD_CPP
	PHP_MINIT(Wcc_XmlRead_reg)(INIT_FUNC_ARGS_PASSTHRU);
#	endif

#	ifdef XMLREAD_CPP
	PHP_MINIT(Wcc_XmlRead_reg)(INIT_FUNC_ARGS_PASSTHRU);
#	endif


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

static const zend_module_dep wccr_deps[] = { /* {{{ */
	ZEND_MOD_REQUIRED("intl")
	ZEND_MOD_REQUIRED("wccz")
	ZEND_MOD_END
};

/* {{{ wccr_module_entry */
zend_module_entry wccr_module_entry = {
	STANDARD_MODULE_HEADER_EX,
	nullptr,
	wccr_deps,
	"wccr",						/* Extension name */
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
