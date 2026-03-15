/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Michael Rynn                                                 |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif



#include <vector>

#include "php_wcc.h"

extern "C" {


/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif
	
};

//core wcc

#include "zpp/show_zpp.h"


#include "wcc/routes_extn.cxx"
#include "wcc/html_extn.cxx"
#include "wcc/database_extn.cxx"
#include "wcc/more_extn.cxx"

DEFINE_STATE_LIST

PHP_MSHUTDOWN_FUNCTION(wcc)
{  
	STATE_MOD_END
	return (zend_result) SUCCESS;
}

PHP_MINIT_FUNCTION(wcc)
{

	register_routes_extn(INIT_FUNC_ARGS_PASSTHRU);
	register_html_extn(INIT_FUNC_ARGS_PASSTHRU);
	register_database_extn(INIT_FUNC_ARGS_PASSTHRU);
	register_more_extn(INIT_FUNC_ARGS_PASSTHRU);
	
	STATE_MOD_INIT

	return SUCCESS;
}

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(wcc)
{
#if defined(ZTS) && defined(COMPILE_DL_WCC)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	STATE_REQ_INIT
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(wcc)
{
	STATE_REQ_END
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(wcc)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "Wcc support", "enabled");
	php_info_print_table_row(2, "Wcc Version", WCC_VERSION);
	php_info_print_table_end();
}
/* }}} */

static const zend_module_dep wccm_deps[] = { /* {{{ */
	ZEND_MOD_REQUIRED("wccz")
	ZEND_MOD_END
};
/* {{{ wcc_module_entry */



#ifdef __cplusplus
// declare "C" linkages
extern "C" {
#endif

static const zend_module_dep wcc_deps[] = { /* {{{ */
   ZEND_MOD_REQUIRED("wccz")
	ZEND_MOD_REQUIRED("intl")
	ZEND_MOD_END
};

zend_module_entry wcc_module_entry = {
	STANDARD_MODULE_HEADER_EX,
	nullptr,
	wcc_deps,
	"Wcc",					/* Extension name */
	nullptr,					// no functions, only classes 
	PHP_MINIT(wcc),		/* PHP_MINIT - Module initialization */
	PHP_MSHUTDOWN(wcc),	/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(wcc),		/* PHP_RINIT - Request initialization */
	PHP_RSHUTDOWN(wcc),	/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(wcc),		/* PHP_MINFO - Module info */
	WCC_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};

/* }}} */

#ifdef COMPILE_DL_WCC
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif

ZEND_GET_MODULE(wcc)
#endif

#ifdef __cplusplus
}
#endif