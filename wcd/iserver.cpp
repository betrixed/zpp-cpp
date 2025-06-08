#ifndef WCD_ISERVER_CPP
#define WCD_ISERVER_CPP

#ifndef WCD_ISERVER_H
#include "iserver.h"
#endif

#ifndef WCD_ICONFIG_H
#include "iconfig.h"
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
	}
};

ISVinit ISV;

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

zobj_mgr IServer::getDataCache()
{
	if (dbCache_.ok())
	{
		return dbCache_;
	}

	
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
	zobj_mgr result = config_.get(name);
	if (!result.ok()) {
		zstr_user alias = alias_.get(name);
		if (alias.ok())
		{
			result = config_.get(alias);
		}
	}
	return result;
}

IConfig*
IServer::needConfig(zstr_user name)
{
	zobj_mgr cfg = config_.get(name);
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

	zval_mgr conn_mgr = active_.get(name);
	zval_user conn(conn_mgr);

	if (conn.isObject())
	{
		return conn.zobject();
	}

	zstr_user alias = alias_.get(name);
	if (alias.ok())
	{
		conn = active_.get(alias);
		if (conn.isObject())
		{
			return conn.zobject();
		}
		return activate(alias);
	}
	return activate(name);

}

zobj_mgr 
IServer::Connect(zstr_user name)
{
	if (!name.ok())
	{
		name = ISV.default_name;
	}

	zobj_user me = Services::getOne(IServer::omg.class_name());

	IServer* s = zobj_toc<IServer>(me);

	return s->getConnect(name);
}

zstr_user 
IServer::getSqlClass(zstr_user dkey)
{
	return sqlClasses_.get(dkey);
}

zstr_user 
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

	cfg->setMyKey(key);
	htab_write(config_).set(name, iconfig);
}

void 
IServer::setAlias(zstr_user alias, zstr_user name)
{
	htab_write(alias_).set(alias, name);
}

};



#endif