/* wccr extension for PHP */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "php_wccr.h"

#include "zpp/show_zpp.h"

#include "wcc/routes_extn.cxx"

DEFINE_STATE_LIST


PHP_MINIT_FUNCTION(wccr)
{
	register_routes_extn(INIT_FUNC_ARGS_PASSTHRU);

	STATE_MOD_INIT

	return SUCCESS;
}


PHP_MSHUTDOWN_FUNCTION(wccr)
{
	STATE_MOD_END
	return SUCCESS;
}


/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(wccr)
{

#if defined(ZTS) && defined(COMPILE_DL_WCCR)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	STATE_REQ_INIT

	return SUCCESS;
}


PHP_RSHUTDOWN_FUNCTION(wccr)
{
	STATE_REQ_END

	return SUCCESS;
}

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
	PHP_MSHUTDOWN(wccr),					/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(wccr),			/* PHP_RINIT - Request initialization */
	PHP_RSHUTDOWN(wccr),					/* PHP_RSHUTDOWN - Request shutdown */
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
