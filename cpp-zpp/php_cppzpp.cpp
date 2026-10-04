/* cppzpp extension for PHP */




/* include zpp classes, dump info support, and the state_init auto initialize */
#include "php_cppzpp.h"

#include "zpp/base.cpp"

#include "zpp/show_zpp.cpp"

#include "zpp/state_init.cpp"

#include "wcc/zppcpp_extn.cxx"

DEFINE_STATE_LIST

PHP_MINIT_FUNCTION(cppzpp)
{
#ifdef DEBUG_EXTRA
	dump_info::run_state_ = true;
#endif

	register_base_init();
	register_fn_calls();
	register_datetime();
	
	register_zppcpp_extn(INIT_FUNC_ARGS_PASSTHRU);

	STATE_MOD_INIT

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(cppzpp)
{
	STATE_MOD_END

#ifdef DEBUG_EXTRA
	//dump_info::run_state_ = false;
#endif
	return SUCCESS;
}

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(cppzpp)
{
#if defined(ZTS) && defined(COMPILE_DL_CPPZPP)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	STATE_REQ_INIT

	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(cppzpp)
{
	STATE_REQ_END

#ifdef BASE_DEBUG
	zpp::mgr_link::report();
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(cppzpp)
{
	php_info_print_table_start();
	php_info_print_table_colspan_header(2, "Low level C++ class library functions for for wccXX extensions");
	php_info_print_table_header(2, "Version Dependency", "<b>" PHP_CPPZPP_VERSION "</b>");
	php_info_print_table_row(2, "Author", PHP_CPPZPP_AUTHOR);
	php_info_print_table_end();

}
/* }}} */

static const zend_module_dep cppzpp_deps[] = { /* {{{ */
	ZEND_MOD_REQUIRED("intl")
	ZEND_MOD_END
};

/* {{{ cppzpp_module_entry */
zend_module_entry cppzpp_module_entry = {
	STANDARD_MODULE_HEADER_EX,
	nullptr,
	cppzpp_deps,
	"cppzpp",					/* Extension name */
	ext_functions,				/* zend_function_entry */
	PHP_MINIT(cppzpp),		/* PHP_MINIT - Module initialization */
	PHP_MSHUTDOWN(cppzpp),	/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(cppzpp),		/* PHP_RINIT - Request initialization */
	PHP_RSHUTDOWN(cppzpp),	/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(cppzpp),		/* PHP_MINFO - Module info */
	PHP_CPPZPP_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_CPPZPP
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(cppzpp)
#endif
