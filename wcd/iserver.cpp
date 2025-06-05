#ifndef WCD_ISERVER_CPP
#define WCD_ISERVER_CPP

#ifndef WCD_ISERVER_H
#include "iserver.h"
#endif

namespace wcd {

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
	}
}

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

};



#endif