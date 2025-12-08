#ifndef WCD_ISERVER_CPP
#define WCD_ISERVER_CPP

#ifndef WCD_ISERVER_H
#include "iserver.h"
#endif

#ifndef WCD_ICONFIG_H
#include "iconfig.h"
#endif

#ifndef WCD_IDRIVER_H
#include "idriver.h"
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

	str_intern  wcd_sql_mysql;
	str_intern  wcd_sql_postgres;
	str_intern  wcd_sql_sqlite;
	str_intern  wcd_sql_firebird;

	str_intern  wcd_ext_mysql;
	str_intern  wcd_ext_postgres;
	str_intern  wcd_ext_sqlite;
	str_intern  wcd_ext_firebird;

	str_intern  pdo_mysql;
	str_intern  pdo_pgsql;
	str_intern  pdo_sqlite;
	str_intern  pdo_firebird;

	str_intern  default_name;

	str_intern  sqls_key;
	str_intern  drivers_key;
	str_intern  db_config;
	str_intern  cache_mgr;
	str_intern  sql_cache;
	str_intern  get_cache;

	str_intern  svckey_str;
	str_intern  active_str;
	str_intern  config_str;
		
	str_intern  active_cfg;

	str_intern  alias_str;
	str_intern  dbcache_str;

	str_intern  sql_classes;
	str_intern  ext_classes;

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
		cache_mgr = "cache_mgr";
		sql_cache = "sql_cache";
		get_cache = "getcache";

		svckey_str = "activekey";
		active_str = "connect";
		config_str = "configure";
		active_cfg = "activecfg";

		alias_str = "alias";
		dbcache_str = "dbcache";
		sql_classes = "sqlClasses";
		ext_classes = "driverClasses";
	}
};

ISVinit ISV;

void 
IServer::debug_info(htab_rw di)
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
IServer::construct(str_ptr svckey)
{
	svc_key_ = svckey;

	htab_rw sw(sqlClasses_);

	sw.set(ISV.pdo_mysql, ISV.wcd_sql_mysql);
	sw.set(ISV.pdo_pgsql, ISV.wcd_sql_postgres);
	sw.set(ISV.pdo_sqlite, ISV.wcd_sql_sqlite);
	sw.set(ISV.pdo_firebird, ISV.wcd_sql_firebird);

	htab_rw dw(driverClasses_);

	dw.set(ISV.pdo_mysql, ISV.wcd_ext_mysql);
	dw.set(ISV.pdo_pgsql, ISV.wcd_ext_postgres);
	dw.set(ISV.pdo_sqlite, ISV.wcd_ext_sqlite);
	dw.set(ISV.pdo_firebird, ISV.wcd_ext_firebird);
}

void 
IServer::destruct()
{
	active_.init();
	config_.init();
	alias_.init();
	sqlClasses_.init();
	driverClasses_.init();
	dbCache_.init();
}

void 
IServer::initDone()
{
	svc_key_.init();
}

obj_rc 
IServer::getDataCache()
{
	if (dbCache_.ok())
	{
		return dbCache_;
	}
	obj_rc cache_mgr = Services::service(ISV.cache_mgr);
	val_rc arg1(ISV.sql_cache);
	dbCache_ = cache_mgr.call(ISV.get_cache, arg1);
	return dbCache_;
}

obj_return 
IServer::activate(str_ptr name)
{
	obj_return cfg = needConfig(name);

	obj_return result;

	if (cfg.has_errors())
	{	
		result = std::move(cfg);
		return result;
	}
	IConfig* ic = zobj_toc<IConfig>(cfg.value_);

	result = ic->newConnect(name);
	if (result.value_.ok())
	{
		htab_rw hw(active_);
		hw.set(name, result.value_);
	}
	return result;
}

obj_rc 
IServer::getConfig(str_ptr name)
{
	//showstr("config name", name);
	obj_rc result(config_.get(name));

	if (!result.ok()) {
		str_ptr alias = alias_.get(name);
		//showstr("alias", alias);
		if (alias.ok())
		{

			result = config_.get(alias);
			//showobj("result alias getConfig", result);
		}
	}


	return result;
}

obj_return
IServer::needConfig(str_ptr name)
{
	obj_return result;

	result.value_ = config_.get(name);

	if (!result.value_.ok())
	{
		result.error() << "No configuration named " << name;
	}
	return result;
}

obj_return
IServer::getConnect(str_ptr name)
{
	obj_return result;

	if (svc_key_.ok())
	{
		str_rc key = svc_key_;
		// only use once
		svc_key_.init();

		// call keyed activation function
		Services::service(key);
	}


	class_data cd(IServer::omg.class_entry_);

	if (!name.ok())
	{
		str_rc name_mgr = cd.static_property(ISV.active_cfg);
		name = name_mgr;
	}

	obj_rc conn(active_.get(name));

	if (conn.ok())
	{
		result.value_ = conn;
	}
	else {
		str_rc alias = alias_.get(name);
		if (alias.ok())
		{
			conn = active_.get(alias);
			if (conn.ok())
			{
				result.value_ = conn;
			}
			else {
				result = activate(alias);
			}
		}
		else {
			result = activate(name);
		}
	}
	obj_ptr test = result.value_;
	if (test.ok())
	{

		IDriver* db = zobj_toc<IDriver>(test);
		result.value_ = db->selfRef();
		//showobj("activate return", result.value_);
	}

	return result;

}

obj_return //static 
IServer::connect(str_ptr name)
{
	obj_ptr me = Services::getOne(IServer::omg.class_name());

	IServer* s = zobj_toc<IServer>(me);

	class_data cd(IServer::omg.class_entry_);
	
	str_rc conkey;

	if (!name.ok())
	{	
		conkey = cd.static_property(ISV.active_cfg);
	}
	else {
		
		val_rc value(name);
		cd.static_property(ISV.active_cfg, value);
		conkey = name;
	}

	return s->getConnect(conkey);
}

str_rc 
IServer::getSqlClass(str_ptr dkey)
{
	return sqlClasses_.get(dkey);
}

str_rc 
IServer::getDriverClass(str_ptr dkey)
{
	str_rc result = driverClasses_.get(dkey);
	//showstr("Driver class", result);
	return result;
}


htab_ptr 
IServer::getSqlClasses()
{
	return sqlClasses_;
}

htab_ptr 
IServer::getDriverClasses()
{
	return driverClasses_;
}

void 
IServer::config(htab_ptr data)
{
	val_ptr clist = data.get(ISV.sqls_key);

	if (clist.isArray())
	{
		htab_rw(sqlClasses_).merge(clist.zarray());
	}

	clist = data.get(ISV.drivers_key);
	if (clist.isArray())
	{
		htab_rw(driverClasses_).merge(clist.zarray());
	}
	clist = data.get(ISV.db_config);
	if (clist.isArray())
	{
		htab_walk wk;
		auto name = wk.key();
		auto cfg = wk.value();

		for(wk.start(clist.zarray()); wk.ok(); wk.next())
		{
			obj_rc dbc_mgr = IConfig::omg.new_zobj();
			IConfig* dbc = zobj_toc<IConfig>(dbc_mgr);
			//showmem("Name", name);
			//showmem("Values-", cfg);
			dbc->assign(cfg);
			addConfig(dbc_mgr, name);
		}
	}

}

error_return 
IServer::addConfig(obj_rc iconfig, str_ptr name)
{
	IConfig* cfg = zobj_toc<IConfig>(iconfig);
	str_ptr key = cfg->getMyKey();

	error_return result;

	if (key.ok())
	{
		result.error() << "IConfig object was already assigned to " << key;
		return result;
	}

	cfg->setMyKey(name);
	htab_rw(config_).set(name, iconfig);
	return result;
}

void 
IServer::setAlias(str_ptr alias, str_ptr name)
{
	htab_rw(alias_).set(alias, name);
}

htab_ptr 
IServer::getAliases()
{
	return alias_;
}

htab_rc  
IServer::getConfigNames()
{
	htab_rc result = htab_rc::getKeys(config_);
	return result;
}

};

using namespace wcd;

//static obj_rc connect(str_ptr name);
ZEND_METHOD(Wcd_IServer, Connect)
{
	zend_string* name = nullptr;

	ZEND_PARSE_PARAMETERS_START(0,1)
	Z_PARAM_OPTIONAL
	Z_PARAM_STR_OR_NULL(name)
	ZEND_PARSE_PARAMETERS_END();

	obj_return result = IServer::connect(name);
	result.throw_errors();
	result.value_.move_zv(return_value);
}


//void construct(str_ptr svckey);
ZEND_METHOD(Wcd_IServer, __construct)
{
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR_OR_NULL(name)
	ZEND_PARSE_PARAMETERS_END();

	IServer* cobj = zval_toc<IServer>(ZEND_THIS);
	cobj->construct(name);	
}

//void construct(str_ptr svckey);
ZEND_METHOD(Wcd_IServer, __destruct)
{
	ZEND_PARSE_PARAMETERS_NONE();
	IServer* cobj = zval_toc<IServer>(ZEND_THIS);
	cobj->destruct();	
}

//void addConfig(obj_rc iconfig, str_ptr name);
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

//obj_rc getConnect(str_ptr name);
ZEND_METHOD(Wcd_IServer, getConnect)
{
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR_OR_NULL(name)
	ZEND_PARSE_PARAMETERS_END();

	IServer* cobj = zval_toc<IServer>(ZEND_THIS);

	obj_return result = cobj->getConnect(name);
	result.throw_errors();
	result.value_.move_zv(return_value);
}

//void config(htab_ptr data);
ZEND_METHOD(Wcd_IServer, config)
{
	zval* data;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(data)
	ZEND_PARSE_PARAMETERS_END();

	IServer* cobj = zval_toc<IServer>(ZEND_THIS);

	cobj->config(data);
}

//htab_ptr IServer::getAliases()
ZEND_METHOD(Wcd_IServer, getAliases)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IServer* cobj = zval_toc<IServer>(ZEND_THIS);

	htab_ptr result = cobj->getAliases();

	result.return_zv(return_value);
}

//obj_rc getConfig(str_ptr name);
ZEND_METHOD(Wcd_IServer, getConfig)
{
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	IServer* cobj = zval_toc<IServer>(ZEND_THIS);
	obj_rc result = cobj->getConfig(name);	

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IServer, getConfigNames)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IServer* cobj = zval_toc<IServer>(ZEND_THIS);

	htab_rc result = cobj->getConfigNames();

	result.move_zv(return_value);
}

//obj_rc getDataCache();
ZEND_METHOD(Wcd_IServer, getDataCache)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IServer* cobj = zval_toc<IServer>(ZEND_THIS);

	obj_rc result = cobj->getDataCache();

	result.move_zv(return_value);
}

//str_ptr getDriverClass(str_ptr dkey);
ZEND_METHOD(Wcd_IServer, getDriverClass)
{
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	IServer* cobj = zval_toc<IServer>(ZEND_THIS);
	str_rc result = cobj->getDriverClass(name);	

	result.move_zv(return_value);
}

//htab_ptr getDriverClasses();
ZEND_METHOD(Wcd_IServer, getDriverClasses)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IServer* cobj = zval_toc<IServer>(ZEND_THIS);

	htab_ptr result = cobj->getDriverClasses();

	result.return_zv(return_value);
}

ZEND_METHOD(Wcd_IServer, getSqlClass)
{
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	IServer* cobj = zval_toc<IServer>(ZEND_THIS);
	str_rc result = cobj->getSqlClass(name);	

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IServer, getSqlClasses)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IServer* cobj = zval_toc<IServer>(ZEND_THIS);

	htab_ptr result = cobj->getSqlClasses();

	result.return_zv(return_value);
}

//void initDone();
ZEND_METHOD(Wcd_IServer, initDone)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IServer* cobj = zval_toc<IServer>(ZEND_THIS);

	cobj->initDone();
}

//void setAlias(str_ptr alias, str_ptr name);
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