#ifndef WCD_ISERVER_CPP
#define WCD_ISERVER_CPP

#ifndef WCD_ISERVER_H
#include "iserver.h"
#endif

#ifndef WCD_ICONFIG_H
#include "iconfig.h"
#endif

#ifndef ICONFIG_ARGINFO_H
#define ICONFIG_ARGINFO_H
extern "C" {
#include "stub/iconfig_arginfo.h"
}
#endif

namespace wcd {

base_obj_mgr<IServer> IServer::omg;

class ISVinit : public state_init {
public:
	ISVinit() : state_init() {}

	zstr_intern  wcd_sql_mysql;
	zstr_intern  wcd_sql_postgres;
	zstr_intern  wcd_sql_sqlite;
	zstr_intern  wcd_sql_firebird;

	zstr_intern  wcd_ext_mysql;
	zstr_intern  wcd_ext_postgres;
	zstr_intern  wcd_ext_sqlite;
	zstr_intern  wcd_ext_firebird;

	zstr_intern  pdo_mysql;
	zstr_intern  pdo_pgsql;
	zstr_intern  pdo_sqlite;
	zstr_intern  pdo_firebird;

	zstr_intern  default_name;

	zstr_intern  sqls_key;
	zstr_intern  drivers_key;
	zstr_intern  db_config;
	zstr_intern  cache_all;
	zstr_intern  sql_cache;
	zstr_intern  get_cache;

	zstr_intern  svckey_str;
	zstr_intern  active_str;
	zstr_intern  config_str;

	zstr_intern  alias_str;
	zstr_intern  dbcache_str;

	zstr_intern  sql_classes;
	zstr_intern  ext_classes;

	void init() override {

		pdo_mysql = "pdo_mysql";
		pdo_pgsql = "pdo_pgsql";
		pdo_sqlite = "pdo_sqlite";
		pdo_firebird = "pdo_firebird";

		wcd_sql_mysql = R"(Wcd\Sql\Mysql)";
		wcd_sql_postgres = R"(Wcd\Sql\Postgres)";
		wcd_sql_sqlite = R"(Wcd\Sql\Sqlite)";
		wcd_sql_firebird = R"(Wcd\Sql\Firebird)";

		wcd_ext_mysql = R"(Wcd\Ext\PdoMysql)";
		wcd_ext_postgres = R"(Wcd\Ext\PdoPgsql)";
		wcd_ext_sqlite = R"(Wcd\Ext\PdoSqlite)";
		wcd_ext_firebird = R"(Wcd\Ext\PdoFirebird)";

		default_name = "default";

		sqls_key = "sqls";
		drivers_key = "drivers";
		db_config = "db-config";
		cache_all = "cache_all";
		sql_cache = "sql_cache";
		get_cache = "getcache";

		svckey_str = "activekey";
		active_str = "connect";
		config_str = "configure";

		alias_str = "alias";
		dbcache_str = "dbcache";
		sql_classes = "sqlClasses";
		ext_classes = "driverClasses";
	}
};

ISVinit ISV;

void 
IServer::debug_info(htab_write di)
{
	di.set(ISV.svckey_str, svc_key_);


	di.set(ISV.active_str, active_);


	di.set(ISV.config_str, config_);

	di.set(ISV.alias_str, alias_);

	di.set(ISV.dbcache_str, dbCache_);


	di.set(ISV.sql_classes, sqlClasses_);

	di.set(ISV.ext_classes, driverClasses_);

}


void 
IServer::construct(zstr_user svckey)
{
	svc_key_ = svckey;

	htab_write sw(sqlClasses_);

	sw.set(ISV.pdo_mysql, ISV.wcd_sql_mysql);
	sw.set(ISV.pdo_pgsql, ISV.wcd_sql_postgres);
	sw.set(ISV.pdo_sqlite, ISV.wcd_sql_sqlite);
	sw.set(ISV.pdo_firebird, ISV.wcd_sql_firebird);

	htab_write dw(driverClasses_);

	dw.set(ISV.pdo_mysql, ISV.wcd_ext_mysql);
	dw.set(ISV.pdo_pgsql, ISV.wcd_ext_postgres);
	dw.set(ISV.pdo_sqlite, ISV.wcd_ext_sqlite);
	dw.set(ISV.pdo_firebird, ISV.wcd_ext_firebird);
}

void 
IServer::initDone()
{
	svc_key_.init();
}

zobj_mgr 
IServer::getDataCache()
{
	if (dbCache_.ok())
	{
		return dbCache_;
	}
	zobj_mgr cache_all = Services::service(ISV.cache_all);
	zval_mgr arg1(ISV.sql_cache);
	dbCache_ = cache_all.call(ISV.get_cache, arg1);
	return dbCache_;
}

zobj_mgr 
IServer::activate(zstr_user name)
{
	IConfig* cfg = needConfig(name);
	zobj_mgr result;

	if (!cfg)
	{
		return result;
	}

	result = cfg->newConnect(name);
	if (result.ok())
	{
		htab_write hw(active_);
		hw.set(name, result);
	}
	else {
	// TODO:: else throw exeception
		zend_throw_error(zend_ce_error,"No connection named %s", name.data());
	}
	return result;
}

zobj_mgr 
IServer::getConfig(zstr_user name)
{
	//showstr("config name", name);
	zobj_mgr result(config_.get(name));

	if (!result.ok()) {
		zstr_user alias = alias_.get(name);
		//showstr("alias", alias);
		if (alias.ok())
		{

			result = config_.get(alias);
			//showobj("result alias getConfig", result);
		}
	}


	return result;
}

IConfig*
IServer::needConfig(zstr_user name)
{
	zobj_mgr cfg(config_.get(name));
	if (!cfg.ok())
	{
		zend_throw_error(zend_ce_error,"No configuration named %s", name.data());
	}
	return zobj_toc<IConfig>(cfg);
}


zobj_mgr 
IServer::getConnect(zstr_user name)
{
	if (svc_key_.ok())
	{
		zstr_mgr key = svc_key_;
		// only use once
		svc_key_.init();

		// call keyed activation function
		Services::service(key);
	}

	if (!name.ok())
	{
		name = ISV.default_name;
	}

	zobj_mgr conn(active_.get(name));

	if (conn.ok())
	{
		return conn;
	}

	zstr_user alias = alias_.get(name);
	if (alias.ok())
	{
		conn = active_.get(alias);
		if (conn.ok())
		{
			return conn;
		}
		return activate(alias);
	}
	return activate(name);

}

zobj_mgr //static 
IServer::connect(zstr_user name)
{
	zobj_user me = Services::getOne(IServer::omg.class_name());

	IServer* s = zobj_toc<IServer>(me);

	return s->getConnect(name);
}

zstr_mgr 
IServer::getSqlClass(zstr_user dkey)
{
	return sqlClasses_.get(dkey);
}

zstr_mgr 
IServer::getDriverClass(zstr_user dkey)
{
	return driverClasses_.get(dkey);
}


htab_read 
IServer::getSqlClasses()
{
	return sqlClasses_;
}

htab_read 
IServer::getDriverClasses()
{
	return driverClasses_;
}

void 
IServer::config(htab_read data)
{
	zval_user clist = data.get(ISV.sqls_key);

	if (clist.isArray())
	{
		htab_write(sqlClasses_).merge(clist.zarray());
	}

	clist = data.get(ISV.drivers_key);
	if (clist.isArray())
	{
		htab_write(driverClasses_).merge(clist.zarray());
	}
	clist = data.get(ISV.db_config);
	if (clist.isArray())
	{
		htab_walk wk;
		auto name = wk.key();
		auto cfg = wk.value();

		for(wk.start(clist.zarray()); wk.ok(); wk.next())
		{
			zobj_mgr dbc_mgr = IConfig::omg.new_zobj();
			IConfig* dbc = zobj_toc<IConfig>(dbc_mgr);
			//showmem("Name", name);
			//showmem("Values-", cfg);
			dbc->assign(cfg);
			addConfig(dbc_mgr, name);
		}
	}

}

void 
IServer::addConfig(zobj_mgr iconfig, zstr_user name)
{
	IConfig* cfg = zobj_toc<IConfig>(iconfig);
	zstr_user key = cfg->getMyKey();

	if (key.ok())
	{
		zend_throw_error(zend_ce_error,"IConfig object was already assigned to %s", key.data());
		return;
	}

	cfg->setMyKey(name);
	htab_write(config_).set(name, iconfig);
}

void 
IServer::setAlias(zstr_user alias, zstr_user name)
{
	htab_write(alias_).set(alias, name);
}

htab_read 
IServer::getAliases()
{
	return alias_;
}

htab_mgr  
IServer::getConfigNames()
{
	htab_mgr result = htab_mgr::getKeys(config_);
	return result;
}

};

using namespace wcd;

//static zobj_mgr connect(zstr_user name);
ZEND_METHOD(Wcd_IServer, Connect)
{
	zend_string* name = nullptr;

	ZEND_PARSE_PARAMETERS_START(0,1)
	Z_PARAM_OPTIONAL
	Z_PARAM_STR_OR_NULL(name)
	ZEND_PARSE_PARAMETERS_END();

	zobj_mgr result = IServer::connect(name);

	result.move_zv(return_value);
}


//void construct(zstr_user svckey);
ZEND_METHOD(Wcd_IServer, __construct)
{
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR_OR_NULL(name)
	ZEND_PARSE_PARAMETERS_END();

	IServer* cobj = zval_toc<IServer>(ZEND_THIS);
	cobj->construct(name);	
}

//void addConfig(zobj_mgr iconfig, zstr_user name);
ZEND_METHOD(Wcd_IServer, addConfig)
{
	zval* config;
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_OBJECT_OF_CLASS(config, IConfig::omg.classEntry())
	Z_PARAM_STR_OR_NULL(name)
	ZEND_PARSE_PARAMETERS_END();

	IServer* cobj = zval_toc<IServer>(ZEND_THIS);
	cobj->addConfig(config, name);	
}

//zobj_mgr getConnect(zstr_user name);
ZEND_METHOD(Wcd_IServer, getConnect)
{
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR_OR_NULL(name)
	ZEND_PARSE_PARAMETERS_END();

	IServer* cobj = zval_toc<IServer>(ZEND_THIS);

	zobj_mgr result = cobj->getConnect(name);
	result.move_zv(return_value);
}

//void config(htab_read data);
ZEND_METHOD(Wcd_IServer, config)
{
	zval* data;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(data)
	ZEND_PARSE_PARAMETERS_END();

	IServer* cobj = zval_toc<IServer>(ZEND_THIS);

	cobj->config(data);
}

//htab_read IServer::getAliases()
ZEND_METHOD(Wcd_IServer, getAliases)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IServer* cobj = zval_toc<IServer>(ZEND_THIS);

	htab_read result = cobj->getAliases();

	result.return_zv(return_value);
}

//zobj_mgr getConfig(zstr_user name);
ZEND_METHOD(Wcd_IServer, getConfig)
{
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	IServer* cobj = zval_toc<IServer>(ZEND_THIS);
	zobj_mgr result = cobj->getConfig(name);	

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IServer, getConfigNames)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IServer* cobj = zval_toc<IServer>(ZEND_THIS);

	htab_mgr result = cobj->getConfigNames();

	result.move_zv(return_value);
}

//zobj_mgr getDataCache();
ZEND_METHOD(Wcd_IServer, getDataCache)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IServer* cobj = zval_toc<IServer>(ZEND_THIS);

	zobj_mgr result = cobj->getDataCache();

	result.move_zv(return_value);
}

//zstr_user getDriverClass(zstr_user dkey);
ZEND_METHOD(Wcd_IServer, getDriverClass)
{
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	IServer* cobj = zval_toc<IServer>(ZEND_THIS);
	zstr_mgr result = cobj->getDriverClass(name);	

	result.move_zv(return_value);
}

//htab_read getDriverClasses();
ZEND_METHOD(Wcd_IServer, getDriverClasses)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IServer* cobj = zval_toc<IServer>(ZEND_THIS);

	htab_read result = cobj->getDriverClasses();

	result.return_zv(return_value);
}

ZEND_METHOD(Wcd_IServer, getSqlClass)
{
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	IServer* cobj = zval_toc<IServer>(ZEND_THIS);
	zstr_mgr result = cobj->getSqlClass(name);	

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IServer, getSqlClasses)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IServer* cobj = zval_toc<IServer>(ZEND_THIS);

	htab_read result = cobj->getSqlClasses();

	result.return_zv(return_value);
}

//void initDone();
ZEND_METHOD(Wcd_IServer, initDone)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IServer* cobj = zval_toc<IServer>(ZEND_THIS);

	cobj->initDone();
}

//void setAlias(zstr_user alias, zstr_user name);
ZEND_METHOD(Wcd_IServer, setAlias)
{
	zend_string* alias;
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_STR(alias)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	IServer* cobj = zval_toc<IServer>(ZEND_THIS);

	cobj->setAlias(alias, name);
}

/*



*/


PHP_MINIT_FUNCTION(Wcd_IServer_reg)
{
	IServer::omg.classEntry(register_class_Wcd_IServer());

	/* // defined in stub - arginfo.h
	 class_data cdata(IServer::omg.class_entry_);

	cdata.add_constant("DEFAULT_NAME", ISV.default_name );
	*/

	return SUCCESS;
}

#endif