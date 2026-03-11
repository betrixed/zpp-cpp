/* wccd extension for PHP */


#include "php_wccd.h"

#include "zpp/show_zpp.h"

// compile 
#include "wcd/iconfig.cpp"
#include "wcd/iserver.cpp"

#include "wcd/ibuild.cpp"
#include "wcd/pdo_pgsql.cpp"
#include "wcd/pdo_mysql.cpp"
#include "wcd/pdodriver.cpp"
#include "wcd/pgsqlfn.cpp"
#include "wcd/idriver.cpp" 

#include "wcd/raw.cpp"
#include "wcd/model.cpp"
#include "wcd/irow.cpp"

#include "wcd/sql_ipart.cpp"
#include "wcd/sql_isql.cpp"
#include "wcd/isql.cpp"
#include "wcd/postgres.cpp"

#include "wcd/bindings.cpp"
#include "wcd/runsql.cpp"
#include "wcd/simple.cpp"


#include "wcd/operation.cpp"
#include "wcd/select.cpp"
#include "wcd/update.cpp"
#include "wcd/insert.cpp"
#include "wcd/delete.cpp"

DEFINE_STATE_LIST

// register
PHP_MINIT_FUNCTION(wccd)
{

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

	STATE_MOD_INIT

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(wccd)
{
	STATE_MOD_END

	return SUCCESS;
}

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(wccd)
{
#if defined(ZTS) && defined(COMPILE_DL_WCCD)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	STATE_REQ_INIT

	return SUCCESS;
}


PHP_RSHUTDOWN_FUNCTION(wccd)
{
	STATE_REQ_END

#ifdef BASE_DEBUG
	zpp::mgr_link::report();
#endif

	return SUCCESS;
}

/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(wccd)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "wccd support", "enabled");
	php_info_print_table_end();
}
/* }}} */

static const zend_module_dep wccd_deps[] = { /* {{{ */
	ZEND_MOD_REQUIRED("wccr")
	ZEND_MOD_END
};

/* {{{ wccd_module_entry */
zend_module_entry wccd_module_entry = {
	STANDARD_MODULE_HEADER_EX,
	nullptr,
	wccd_deps,
	"wccd",					/* Extension name */
	nullptr,					/* zend_function_entry */
	PHP_MINIT(wccd),							/* PHP_MINIT - Module initialization */
	PHP_MSHUTDOWN(wccd),							/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(wccd),			/* PHP_RINIT - Request initialization */
	PHP_RSHUTDOWN(wccd),							/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(wccd),			/* PHP_MINFO - Module info */
	PHP_WCCD_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_WCCD
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(wccd)
#endif
