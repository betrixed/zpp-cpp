#ifndef DATABASE_EXTN_CXX
#define DATABASE_EXTN_CXX
// compile 
#include "wcd/iconfig.cpp"
#include "wcd/iserver.cpp"

#include "wcd/ibuild.cpp"
#include "wcd/pdo_pgsql.cpp"
#include "wcd/pdo_mysql.cpp"
#include "wcd/pdodriver.cpp"
#include "wcd/pgsqlfn.cpp"
// This is undone for now. 
#include "wcd/mysqlfn.cpp"

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

void register_database_extn(INIT_FUNC_ARGS)
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
}
#endif
