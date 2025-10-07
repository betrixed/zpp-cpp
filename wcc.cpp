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

#ifndef WCC_ARGINFO_H
#define WCC_ARGINFO_H
extern "C" {
#include "stub/wcc_arginfo.h"
}
#endif

extern "C" {

#include "php_wcc.h"

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif
	
};


//#define DEBUG_EXTRA

//core wcc
#include "zpp/base.cpp"
#include "zpp/show_zpp.cpp"
#include "wcc/strfns.cpp"
#include "wcc/service_access.cpp"
#include "wcc/finder.cpp"
#include "wcc/reflect_cache.cpp"
#include "wcc/services.cpp"
#include "wcc/replace.cpp"
#include "wcc/config.cpp"
#include "wcc/hmap.cpp"
#include "wcc/icachedata.cpp"
#include "wcc/icache.cpp"
#include "wcc/cachemgr.cpp"

// route, wccr

#include "wcc/pair.cpp"
#include "wcc/target.cpp"
#include "wcc/route.cpp"
#include "wcc/route_set.cpp"
#include "wcc/route_match.cpp"
#include "wcc/request_globals.cpp"
#include "wcc/file_upload.cpp"
#include "wcc/global_response.cpp"

// html generation, wcch
#include "wcc/assets.cpp"
#include "wcc/money_fmt.cpp"

#include "wcc/htmlgem.cpp"
#include "wcc/search_list.cpp"
#include "wcc/plate.cpp"
#include "wcc/plate_engine.cpp"
#include "wcc/htmlplates.cpp"



//WCC_SERVICES_CPP
// miscellaneous, wccm
#include "toml/toml_php.cpp"
#include "wcc/xmlread.cpp"

#include "wcc/run.cpp"
#include "wcc/loader.cpp"

 //ICACHE_DATA_CPP

//Wcd classes, wccd
#include "wcd/iconfig.cpp"
#include "wcd/iserver.cpp"
#include "wcd/idriver.cpp" 
#include "wcd/ibuild.cpp"

#include "wcd/raw.cpp"
#include "wcd/model.cpp"
#include "wcd/irow.cpp"

#include "wcd/sql_ipart.cpp"
#include "wcd/sql_isql.cpp"
#include "wcd/bindings.cpp"
#include "wcd/runsql.cpp"
#include "wcd/simple.cpp"


#include "wcd/operation.cpp"
#include "wcd/select.cpp"
#include "wcd/update.cpp"
#include "wcd/insert.cpp"
#include "wcd/delete.cpp"

PHP_MSHUTDOWN_FUNCTION(wcc)
{

	zpp::state_init::end_all();

#ifdef DEBUG_EXTRA
	dump_info::run_state_ = false;
#endif
	return (zend_result) SUCCESS;
}

PHP_MINIT_FUNCTION(wcc)
{
#ifdef DEBUG_EXTRA
	dump_info::run_state_ = true;
#endif

	//so interned strings like property names
	//are created before classes.
	zpp::state_init::init_all();
	
	// init status code map
#ifdef GLOBAL_RESPONSE_CPP
	PHP_MINIT(Wcc_Response_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif
	
	//REGISTER_INI_ENTRIES();
	//PHP_MINIT(phiz_str8)(INIT_FUNC_ARGS_PASSTHRU);
	//PHP_MINIT(phiz_carray)(INIT_FUNC_ARGS_PASSTHRU);

#ifdef FILTER_WCF_CPP
	PHP_MINIT(FilterObj_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef STRFNS_CPP
	PHP_MINIT(Strfns_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif




#ifdef REQUEST_GLOBALS_CPP
	PHP_MINIT(RequestGlobals_reg)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(FileUpload_reg)(INIT_FUNC_ARGS_PASSTHRU);
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

#ifdef PLATE_ENGINE_CPP

	PHP_MINIT(SearchList_reg)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(Wcc_PlateEngine_reg)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(Wcc_Plate_reg)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(Wcc_HtmlPlates_reg)(INIT_FUNC_ARGS_PASSTHRU);
	
#endif

#ifdef MONEY_FMT_CPP
	PHP_MINIT(Wcc_Money_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef HTMLGEM_CPP
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

#ifdef SERVICE_ACCESS_CPP
	PHP_MINIT(ServiceAccess_reg)(INIT_FUNC_ARGS_PASSTHRU);
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

#ifdef WCC_ASSETS_CPP
PHP_MINIT(wcc_assets_reg)(INIT_FUNC_ARGS_PASSTHRU);

#endif

#ifdef ICACHE_CPP
	PHP_MINIT(Wcc_ICacheData_reg)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(Wcc_ICache_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCC_CACHEMGR_CPP
	PHP_MINIT(Wcc_CacheMgr_reg)(INIT_FUNC_ARGS_PASSTHRU);
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

#ifdef WCD_MODEL_CPP
	PHP_MINIT(Wcd_Model_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCD_IDRIVER_CPP
	PHP_MINIT(Wcd_IDriver_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCD_ISERVER_CPP
	PHP_MINIT(Wcd_IServer_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCD_ICONFIG_CPP
	PHP_MINIT(Wcd_IConfig_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCD_SQL_RAW_CPP
	PHP_MINIT(Sql_Raw_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCD_SIMPLE_CPP
	PHP_MINIT(Wcd_Simple_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCD_OPERATION_CPP
	PHP_MINIT(Wcd_Operation_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif
	
#ifdef WCD_IBUILD_CPP
	PHP_MINIT(Wcd_IBuild_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif
#ifdef WCC_RUN_CPP
	PHP_MINIT(wcc_run_reg)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(wcc_loader_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif


	return SUCCESS;
}

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(wcc)
{
#if defined(ZTS) && defined(COMPILE_DL_WCC)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	zpp::state_init::init_request();
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(wcc)
{

	zpp::state_init::end_request();

#ifdef BASE_DEBUG
	zpp::mgr_link::report();
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
	ext_functions,			
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