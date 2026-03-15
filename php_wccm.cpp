/* wccm extension for PHP */
#define ZPP_BUILD_ALL

#include "php_wccm.h"
#include "zpp/show_zpp.h"


#include "wcc/more_extn.cxx"

DEFINE_STATE_LIST

PHP_MINIT_FUNCTION(wccm)
{	
	register_more_extn(INIT_FUNC_ARGS_PASSTHRU);

	STATE_MOD_INIT

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(wccm)
{
	STATE_MOD_END

	return SUCCESS;
}

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(wccm)
{
#if defined(ZTS) && defined(COMPILE_DL_WCCM)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	STATE_REQ_INIT
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(wccm)
{
	STATE_REQ_END

	
	return SUCCESS;
}


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
	PHP_MSHUTDOWN(wccm),							/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(wccm),			/* PHP_RINIT - Request initialization */
	PHP_RSHUTDOWN(wccm),							/* PHP_RSHUTDOWN - Request shutdown */
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
