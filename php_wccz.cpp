/* wccz extension for PHP */

/* include zpp classes, dump info support, and the state_init auto initialize */
#include "php_wccz.h"

#include "zpp/base.cpp"
#include "zpp/state_init.cpp"

// for dump_info
#include "zpp/show_zpp.cpp"


//#include "wcc/replace.cpp"
//#include "wcc/config.cpp"
//#include "wcc/finder.cpp"
//#include "wcc/hmap.cpp"
//#include "wcc/reflect_cache.cpp"
//#include "wcc/services.cpp"
//#include "wcc/service_access.cpp"
#include "wcc/strfns.cpp"




/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

PHP_MINIT_FUNCTION(wccz)
{
#ifdef DEBUG_EXTRA
	dump_info::run_state_ = true;
#endif
	zpp::state_init::init_all();

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

#ifdef STRFNS_CPP
	PHP_MINIT(Strfns_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(wccz)
{

	zpp::state_init::end_all();

#ifdef DEBUG_EXTRA
	dump_info::run_state_ = false;
#endif
	return SUCCESS;
}

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(wccz)
{
#if defined(ZTS) && defined(COMPILE_DL_WCCZ)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	zpp::state_init::init_request();
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(wccz)
{
	zpp::state_init::end_request();

#ifdef BASE_DEBUG
	zpp::mgr_link::report();
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(wccz)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "C++ zpp and wcc core support", "enabled");
	php_info_print_table_end();
}
/* }}} */

static const zend_module_dep wccz_deps[] = { /* {{{ */
	ZEND_MOD_REQUIRED("intl")
	ZEND_MOD_END
};

/* {{{ wccz_module_entry */
zend_module_entry wccz_module_entry = {
	STANDARD_MODULE_HEADER_EX,
	nullptr,
	wccz_deps,
	"wccz",					/* Extension name */
	ext_functions,			/* zend_function_entry */
	PHP_MINIT(wccz),		/* PHP_MINIT - Module initialization */
	PHP_MSHUTDOWN(wccz),	/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(wccz),		/* PHP_RINIT - Request initialization */
	PHP_RSHUTDOWN(wccz),	/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(wccz),		/* PHP_MINFO - Module info */
	PHP_WCCZ_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_WCCZ
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(wccz)
#endif
