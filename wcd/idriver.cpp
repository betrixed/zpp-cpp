#ifndef WCD_IDRIVER_CPP
#define WCD_IDRIVER_CPP

#ifndef WCD_IDRIVER_H
#include "idriver.h"
#endif

#ifndef DB_ARGINFO_H
#define DB_ARGINFO_H
extern "C" {
     #include "stub/db_arginfo.h"
}
#endif

#ifndef ICACHE_H
#include "wcc/icache.h"
#endif

#ifndef WCC_SERVICES_H
#include "wcc/services.h"
#endif

#ifndef WCD_RUNSQL_H
#include "runsql.h"
#endif

namespace wcd {

base_obj_mgr<IDriver> IDriver::omg;


class DBSInit : public state_init {
public:

	zstr_intern  query_str;
	zstr_intern  fetch_str;
	zstr_intern  close_cursor;
	zstr_intern  pdo_prefix;

	DBSInit() : state_init() {}
		

	void init() override;
};


void DBSInit::init() {	
		query_str = "query";
		fetch_str = "fetch";
		close_cursor = "closecursor";
		pdo_prefix = "pdo_";
	}

DBSInit DBS;

void 
IDriver::construct(zobj_user icfgobj, zstr_user name)
{
	icfg_ = icfgobj;
	cfg_name_ = name;
	IConfig* cfg = icfg();
	db_name_ = cfg->getDatabase();
	isql_ = cfg->newSql();
}

zobj_mgr  
IDriver::iconfig()
{
	return icfg_;
}

IConfig*    
IDriver::icfg()
{
	return zobj_toc<IConfig>(icfg_);
}

void 
IDriver::destruct()
{
	if (isConnected())
	{
		close();
	}

	schema_def_.init();
	table_models_.init();
	isql_.init();
	icfg_.init();
}

zstr_mgr 
IDriver::getDSN()
{
	zstr_mgr dname = icfg()->getDriverName();
	dname = dname.to_lower();

	if (dname.starts_with(DBS.pdo_prefix)) {
		dname = dname.substr(4);
	}
	else {
		zend_throw_error(zend_ce_error,"PDO Driver names need to begin with 'pdo_'");
		return dname;
	}
	IConfig* cfg = icfg();
	zstr_mgr host = cfg->getHost();
	zstr_mgr dbname = cfg->getDatabase();

	zstr_buffer buf;

	buf << dname << ':' << "host=" << host
	    << ";dbname=" << dbname;

	return buf.zstr();
}

htab_mgr 
IDriver::getConnectOptions()
{
	htab_mgr result;

	htab_write options(result);

	options.push_back(PDO_ATTR_ERRMODE);
	options.push_back(PDO_ERRMODE_EXCEPTION);
	return result;
}

void 
IDriver::connect()
{
	if (handle_.ok())
	{
		return;
	}

	zstr_mgr dsn = getDSN();


}

void //virtual
IDriver::afterConnect()
{

}

zval_mgr 
IDriver::handle()
{
	if (handle_.ok())
	{
		return handle_;
	}

}

bool 
IDriver::begin()
{

}

zobj_mgr 
IDriver::newDmlBuild()
{
	IConfig* cfg = icfg();
	return cfg->newDmlBuild(this->vobj());
}

zobj_mgr 
IDriver::getSchema()
{
	
	ICache*  cache = nullptr;
	zstr_mgr name;

	if (schema_def_.ok())
	{
		return schema_def_;
	}
	zobj_user server_mgr = Services::getOne(IServer::omg.class_name());
	IServer* isv = zobj_toc<IServer>(server_mgr);
	zobj_mgr cache_mgr = isv->getDataCache();
	if (cache_mgr.ok())
	{
		cache = zobj_toc<ICache>(cache_mgr);
		name = icfg()->getDatabase();
		schema_def_ = cache->get(name); 
	}
	if (!schema_def_.ok())
	{
		schema_def_ = readSchema();
		if (cache)
		{
			zval_mgr data(schema_def_);
			cache->set(name, data);
		}
	}
	return schema_def_;


}

zval_mgr 
IDriver::querySingle(zstr_user query)
{
	zobj_mgr pdo(handle_);

	zval_mgr arg1(query);

	zobj_mgr stmt = pdo.call(DBS.query_str, arg1);

	arg1 = (zend_long) ifetch_;
	zval_mgr result = stmt.call(DBS.fetch_str, arg1);

	stmt.call(DBS.close_cursor);

	return result;
}

};//namespace

using namespace wcd;

//void construct(zobj_user icfg, zstr_user name);
ZEND_METHOD(Wcd_IDriver, __construct)
{
	zval* config;
	zend_string*   name;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_OBJECT_OF_CLASS(config, IConfig::omg.classEntry())
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	db->construct(config, name);

}

ZEND_METHOD(Wcd_IDriver, __destruct)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	db->destruct();
}



ZEND_METHOD(Wcd_IDriver, begin)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	RETURN_BOOL(db->begin());


}

ZEND_METHOD(Wcd_IDriver, bind){}
ZEND_METHOD(Wcd_IDriver, close){}
ZEND_METHOD(Wcd_IDriver, closeStmt){}
ZEND_METHOD(Wcd_IDriver, commit){}
ZEND_METHOD(Wcd_IDriver, connect){}
ZEND_METHOD(Wcd_IDriver, escape){}
ZEND_METHOD(Wcd_IDriver, execute){}
ZEND_METHOD(Wcd_IDriver, fetchAllRows){}
ZEND_METHOD(Wcd_IDriver, fetchRow){}
ZEND_METHOD(Wcd_IDriver, getAttribute){}
ZEND_METHOD(Wcd_IDriver, getColumnNames){}
ZEND_METHOD(Wcd_IDriver, getConnectOptions){}
ZEND_METHOD(Wcd_IDriver, getDSN){}
ZEND_METHOD(Wcd_IDriver, getDatabaseName){}
ZEND_METHOD(Wcd_IDriver, getFetch){}
ZEND_METHOD(Wcd_IDriver, getSchema){}
ZEND_METHOD(Wcd_IDriver, getSchemaClass){}
ZEND_METHOD(Wcd_IDriver, getTableColumns){}
ZEND_METHOD(Wcd_IDriver, getTableModel){}
ZEND_METHOD(Wcd_IDriver, getTableNames){}
ZEND_METHOD(Wcd_IDriver, handle){}
ZEND_METHOD(Wcd_IDriver, iConfig){}
ZEND_METHOD(Wcd_IDriver, iSql){}
ZEND_METHOD(Wcd_IDriver, inTransaction){}
ZEND_METHOD(Wcd_IDriver, isAutoCommit){}
ZEND_METHOD(Wcd_IDriver, isConnected){}
ZEND_METHOD(Wcd_IDriver, lastInsertId){}
ZEND_METHOD(Wcd_IDriver, lastSeqValue){}
ZEND_METHOD(Wcd_IDriver, log){}
ZEND_METHOD(Wcd_IDriver, modelClassName){}
ZEND_METHOD(Wcd_IDriver, newDmlBuild){}
ZEND_METHOD(Wcd_IDriver, param){}
ZEND_METHOD(Wcd_IDriver, prepare){}
ZEND_METHOD(Wcd_IDriver, prepareQuery){}
ZEND_METHOD(Wcd_IDriver, query){}
ZEND_METHOD(Wcd_IDriver, querySingle){}
ZEND_METHOD(Wcd_IDriver, quoteName){}
ZEND_METHOD(Wcd_IDriver, readSchema){}
ZEND_METHOD(Wcd_IDriver, rollback){}
ZEND_METHOD(Wcd_IDriver, setAttribute){}
ZEND_METHOD(Wcd_IDriver, setFetch){}
ZEND_METHOD(Wcd_IDriver, transaction){}


PHP_MINIT_FUNCTION(Wcd_IDriver_reg)
{
	IDriver::omg.classEntry(register_class_Wcd_IDriver());
	

	return SUCCESS;
}
#endif