#ifndef WCD_ISERVER_H
#define WCD_ISERVER_H




#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif


namespace wcd {

	using namespace zpp;

	class IDriver;
	class IConfig;

	class IServer : public base_d {
	protected:	

		str_rc svc_key_;

		htab_rc active_;

		htab_rc config_;

		htab_rc alias_;

		obj_rc dbCache_;

		htab_rc sqlClasses_;

		htab_rc driverClasses_;


		obj_return activate(str_ptr name);
		obj_return needConfig(str_ptr name);

	public:

		static base_obj_mgr<IServer> omg;

		virtual void debug_info(htab_rw di);
		
		void construct(str_ptr svckey);
		void destruct();
		
		void initDone();

		obj_rc getDataCache();

		static wref_return connect(str_ptr name);

		wref_return getConnect(str_ptr name);
		
		obj_rc getConfig(str_ptr name);
		
		str_rc getSqlClass(str_ptr dkey);
		str_rc getDriverClass(str_ptr dkey);

		htab_ptr getSqlClasses();
		htab_ptr getDriverClasses();

		error_return config(htab_ptr data);

		error_return addConfig(obj_rc iconfig, str_ptr name);

		void setAlias(str_ptr alias, str_ptr name);
		htab_ptr getAliases();
		htab_rc  getConfigNames();

	};

	class ISVinit : public state_init {
	public:

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

		void init() override;
	};

extern ISVinit ISV;
}; // namespace wcd



#endif