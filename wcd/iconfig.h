#ifndef WCD_ICONFIG_H
#define WCD_ICONFIG_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif



namespace wcd {

	using namespace zpp;
	using namespace wcc;

/** To properly inherit Hmap property access and array access, 
 *   also have to override own special object handlers
 *   with its Hmap_php static functions
 */
class IConfig;

class IConfig : public base_d
{
public:
	static base_obj_mgr<IConfig> omg;

	void assign(htab_read cfg);
	zval_mgr getValue(zval_user keys, bool required, zval_user ifnot);
	zval_mgr getValue(zstr_user key, bool required, zval_user ifnot);
	
	zstr_user getMyKey();
	void      setMyKey(zstr_user key);
	
	zobj_mgr newConnect(zstr_user name);
	zobj_mgr newSql();
	zobj_mgr newDmlBuild(zobj_mgr idriver);
	
	
	zval_mgr get(zstr_user name);
	void     set(zstr_user name, zval_user value);
	void     set(zstr_user name, zstr_user value);
	
	htab_read getArray();
	zstr_mgr getCharset();
	zstr_mgr getCollation();
	zstr_mgr getDriverClass();

	zstr_mgr stringVal(zstr_user key);

	zstr_mgr getDriverName();
	zstr_mgr getPassword();
	zstr_mgr getUsername();
	int      getPort();
	zstr_mgr getHost();
	zstr_mgr getDatabase();

	zstr_mgr getSqlClass();


protected:

	void set_data(zstr_user key, zval_user values, bool required, zval_user ifnot);
	
	htab_mgr data_;
	htab_mgr cfg_;
	zstr_mgr mykey_;


};

class CfgInit : public state_init {
public:
	CfgInit();

	zstr_intern msg_or;
	zstr_intern k_driver;
	zstr_intern k_host;
	zstr_intern k_port;
	zstr_intern k_database;
	zstr_intern k_dbname;

	zstr_intern k_username;
	zstr_intern k_password;
	zstr_intern k_charset;
	zstr_intern k_collation;
	zstr_intern k_sql_class;
	zstr_intern k_dml_class;
	zstr_intern k_rel_class;
	zstr_intern k_model_ns;
	zstr_intern k_model;
	zstr_intern k_processor;
	zstr_intern k_hostname;
	zstr_intern k_localhost;
	zstr_intern k_adapter;
	zstr_intern k_user;

	zstr_intern k_prepare;
	zstr_intern k_execute;
	zstr_intern db_models_ns;

	void init() override;
};

extern CfgInit ICS;

}; // namespace wcd

#endif
