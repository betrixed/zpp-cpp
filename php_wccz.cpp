/* wccz extension for PHP */




/* include zpp classes, dump info support, and the state_init auto initialize */
#include "php_wccz.h"

#include "zpp/base.cpp"

#include "zpp/show_zpp.cpp"

#include "zpp/state_init.cpp"

#include "wcc/zero_extn.cxx"

DEFINE_STATE_LIST


PHP_MINIT_FUNCTION(wccz)
{
#ifdef DEBUG_EXTRA
	dump_info::run_state_ = true;
#endif

	register_base_init();
	register_fn_calls();
	register_datetime();
	
	register_zero_extn(INIT_FUNC_ARGS_PASSTHRU);

	STATE_MOD_INIT

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(wccz)
{
	STATE_MOD_END

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
	STATE_REQ_INIT

	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(wccz)
{
	STATE_REQ_END

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
	ext_functions,				/* zend_function_entry */
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
