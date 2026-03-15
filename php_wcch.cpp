/* wccr extension for PHP */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#define ZPP_BUILD_ALL

#include "php_wcch.h"

#include "zpp/show_zpp.h"

#include "wcc/html_extn.cxx"

DEFINE_STATE_LIST

PHP_MINIT_FUNCTION(wcch)
{
	register_html_extn(INIT_FUNC_ARGS_PASSTHRU);

	STATE_MOD_INIT

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(wcch)
{
	STATE_MOD_END

	return SUCCESS;
}

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(wcch)
{
#if defined(ZTS) && defined(COMPILE_DL_WCCH)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	STATE_REQ_INIT

	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(wcch)
{
	STATE_REQ_END

	return SUCCESS;
}


/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(wcch)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "wcch support", "enabled");
	php_info_print_table_end();
}
/* }}} */

static const zend_module_dep wcch_deps[] = { /* {{{ */
	ZEND_MOD_REQUIRED("wccz")
	ZEND_MOD_END
};

/* {{{ wcch_module_entry */
zend_module_entry wcch_module_entry = {
	STANDARD_MODULE_HEADER_EX,
	nullptr,
	wcch_deps,
	"wcch",					/* Extension name */
	nullptr,					/* zend_function_entry */
	PHP_MINIT(wcch),							/* PHP_MINIT - Module initialization */
	PHP_MSHUTDOWN(wcch),							/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(wcch),			/* PHP_RINIT - Request initialization */
	PHP_RSHUTDOWN(wcch),							/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(wcch),			/* PHP_MINFO - Module info */
	PHP_WCCH_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_WCCH
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(wcch)
#endif
