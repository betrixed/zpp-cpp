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

//#define HWALK_DEBUG 1

#include "php.h"
#include "ext/standard/info.h"

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

extern "C" {

#include "php_wcc.h"
#include "stub/wcc_arginfo.h"

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif
	
};

#include "zpp/base.cpp"

#define DEBUG_XTRA
#include "zpp/show_zpp.cpp"
#include "src/pair.cpp"
#include "src/target.cpp"
#include "src/route.cpp"
#include "src/route_set.cpp"
#include "src/route_match.cpp"
#include "src/route_add.cpp"

//#include "src/zpp/all.cpp"

//#include "src/str_intern.cpp"

 //SQL_IPART_CPP
//#include "src/sql_ipart.cpp"
//#include "src/sql_isql.cpp"


//TOML_PHP_CPP
//#include "src/toml_php.cpp"
//#include "src/daytime.cpp"

/* // TOML_STREAM_CPP
#include "src/ucode8.cpp"
#include "src/str8_obj.cpp"
#include "src/toml_stream.cpp"

*/

//GLOBAL_RESPONSE_CPP
 //#include "src/global_response.cpp"
 //#include "src/headers_response.cpp"




//FILTER_WCF_CPP // not actually used much, so not included
//#include "src/filter_wcf.cpp"


//#include "src/wcc_config.cpp" // not adequatedly functional

// REQUEST_GLOBALS_CPP

/*
#include "src/request_globals.cpp"
#include "src/file_upload.cpp"


// HTMLGEM_CPP

#include "src/htmlgem.cpp"
#include "src/money_fmt.cpp"
#include "src/service_access.cpp"
#include "src/htmlplates.cpp"

#include "src/search_list.cpp"
#include "src/plate_wcp.cpp"
#include "src/plate_engine.cpp"
*/

//WCC_SERVICES_CPP
#include "src/reflect_cache.cpp"
#include "src/services.cpp"

#define XMLREAD_CODE

#ifdef XMLREAD_CODE
#include "src/xmlread.cpp"
#endif

 //ICACHE_DATA_CPP
#include "src/icachedata.cpp"
#include "src/icache.cpp"

/*
#include "src/reflect_cache.cpp"
#include "src/finder.cpp"
#include "src/target.cpp"

#include "src/route_set.cpp"

#include "src/route_match.cpp"
#include "src/route_add.cpp"

*/
#include "zpp/strfns.cpp"

PHP_FUNCTION(Wcc_init_globals)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
   ZEND_PARSE_PARAMETERS_END();

	zobj_mgr wrap;
	printf("init_globals\n");

	wrap = ReflectCache::instance();
	showobj("zval_own new_wrap", wrap);

}


PHP_MSHUTDOWN_FUNCTION(wcc)
{

	zpp::state_init::end_all();

#ifdef DEBUG_XTRA
	dump_info::run_state_ = false;
#endif
	return (zend_result) SUCCESS;
}

PHP_MINIT_FUNCTION(wcc)
{
#ifdef DEBUG_XTRA
	dump_info::run_state_ = true;
#endif

	zpp::state_init::init_all();
	
	// init status code map
#ifdef GLOBAL_RESPONSE_CPP
	PHP_MINIT(Wcc_Response_reg)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(Wcc_Headers_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif
	//REGISTER_INI_ENTRIES();
	//PHP_MINIT(phiz_str8)(INIT_FUNC_ARGS_PASSTHRU);
	//PHP_MINIT(phiz_carray)(INIT_FUNC_ARGS_PASSTHRU);

#ifdef FILTER_WCF_CPP
	PHP_MINIT(FilterObj_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef REQUEST_GLOBALS_CPP
	PHP_MINIT(RequestGlobals_reg)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(FileUpload_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCC_FINDER_CPP
	PHP_MINIT(Wcc_Finder_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCC_CONFIG_CPP
	PHP_MINIT(Wcc_Config_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef PLATE_ENGINE_CPP
	PHP_MINIT(ServiceAccess_reg)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(SearchList_reg)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(Wcc_PlateEngine_reg)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(Wcc_Plate_reg)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(Wcc_HtmlPlates_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef HTMLGEM_CPP
	PHP_MINIT(Wcc_Money_reg)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(Wcc_HtmlGem_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WC_CARRAY_CPP
	PHP_MINIT(Wcc_CArray_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef EMPTY_TEST_CPP
	PHP_MINIT(Wcc_EmptyTest_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef XMLREAD_CPP
	PHP_MINIT(Wcc_XmlRead_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCC_SERVICES_CPP
PHP_MINIT(wc_services_md)(INIT_FUNC_ARGS_PASSTHRU);
PHP_MINIT(Wcc_ReflectCache)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCC_PAIR_CPP
PHP_MINIT(wcc_pair_d)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCC_ROUTE_CPP
	PHP_MINIT(wcc_route_d)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCC_TARGET_CPP
		PHP_MINIT(wcc_target)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCC_ROUTESET_CPP
	PHP_MINIT(wcc_routeset_d)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef ROUTE_MATCH_CPP
	PHP_MINIT(route_match_d)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef ROUTE_ADD_CPP
PHP_MINIT(wcc_route_add)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef ICACHE_CPP
	PHP_MINIT(Wcc_ICacheData_reg)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(Wcc_ICache_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef DAYTIME_CPP
	PHP_MINIT(Day24_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef TOML_PHP_CPP
	PHP_MINIT(Toml_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef TOML_STREAM_CPP
	PHP_MINIT(TomlReader_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef SQL_IPART_CPP
	PHP_MINIT(SqlIPart_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

	return SUCCESS;
}

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(wcc)
{
#if defined(ZTS) && defined(COMPILE_DL_WCC)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(wcc)
{
#ifdef BASE_DEBUG
	wcc::mgr_link::report();
#endif

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

/* {{{ wcc_module_entry */



#ifdef __cplusplus
// declare "C" linkages
extern "C" {
#endif

zend_module_entry wcc_module_entry = {
	STANDARD_MODULE_HEADER,
	"Wcc",					/* Extension name */
	ext_functions,			/* defined in wcc_arginfo.h */
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