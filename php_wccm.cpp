/* wccm extension for PHP */
#define ZPP_BUILD_ALL

#include "php_wccm.h"

#include "zpp/base.h"
#include "zpp/show_zpp.h"
#include "md4c/markhtml.cpp"
#include "toml/toml_php.cpp"


PHP_MINIT_FUNCTION(wccm)
{	
#ifdef DAYTIME_CPP
	PHP_MINIT(Wcc_Day24_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef TOML_PHP_CPP
	PHP_MINIT(Wcc_Toml_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#  ifdef MARKHTML_CPP
	PHP_MINIT(Wcc_MarkToHtml_reg)(INIT_FUNC_ARGS_PASSTHRU);
#  endif
	
	return SUCCESS;
}

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(wccm)
{
#if defined(ZTS) && defined(COMPILE_DL_WCCM)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(wccm)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "wccm support", "enabled");
	php_info_print_table_end();
}
/* }}} */

static const zend_module_dep wccm_deps[] = { /* {{{ */
	ZEND_MOD_REQUIRED("wccz")
	ZEND_MOD_END
};

/* {{{ wccm_module_entry */
zend_module_entry wccm_module_entry = {
	STANDARD_MODULE_HEADER_EX,
	nullptr,
	wccm_deps,
	"wccm",						/* Extension name */
	nullptr,					/* zend_function_entry */
	PHP_MINIT(wccm),							/* PHP_MINIT - Module initialization */
	NULL,							/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(wccm),			/* PHP_RINIT - Request initialization */
	NULL,							/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(wccm),			/* PHP_MINFO - Module info */
	PHP_WCCM_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_WCCM
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(wccm)
#endif
