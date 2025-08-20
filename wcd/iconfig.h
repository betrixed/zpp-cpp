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

class IConfig : public base_d
{
public:
	static base_obj_mgr<IConfig> omg;

	virtual void debug_info(htab_rw di);

	void assign(htab_ptr cfg);
	val_rc getValue(val_ptr keys, bool required, val_ptr ifnot);
	val_rc getValue(str_ptr key, bool required, val_ptr ifnot);
	
	str_ptr getMyKey();
	void      setMyKey(str_ptr key);
	
	obj_rc newConnect(str_ptr name);
	obj_rc newSql();
	//obj_rc newDmlBuild(obj_rc idriver);
	
	
	val_rc get(str_ptr name);
	void     set(str_ptr name, val_ptr value);
	void     set(str_ptr name, str_ptr value);
	
	htab_ptr getArray();
	str_rc getCharset();
	str_rc getCollation();
	str_rc getDriverClass();
	str_rc getDmlBuildClass();

	str_rc stringVal(str_ptr key);

	str_rc getDriverName();
	str_rc getPassword();
	str_rc getUsername();
	int      getPort();
	str_rc getHost();
	str_rc getDatabase();

	str_rc getSqlClass();


protected:

	void set_data(str_ptr key, val_ptr values, bool required, val_ptr ifnot);
	
	htab_rc data_;
	htab_rc cfg_;
	str_rc mykey_;


};

class CfgInit : public state_init {
public:
	CfgInit();

	str_intern msg_or;
	str_intern k_driver;
	str_intern k_host;
	str_intern k_port;
	str_intern k_database;
	str_intern k_dbname;

	str_intern k_username;
	str_intern k_password;
	str_intern k_charset;
	str_intern k_collation;
	str_intern k_sql_class;
	str_intern k_dml_class;
	str_intern k_rel_class;
	str_intern k_model_ns;
	str_intern k_model;
	str_intern k_processor;
	str_intern k_hostname;
	str_intern k_localhost;
	str_intern k_adapter;
	str_intern k_user;

	str_intern k_prepare;
	str_intern k_execute;
	str_intern db_models_ns;

	str_intern mykey;
	str_intern data_str;
	str_intern cfg_str;

	void init() override;
};

extern CfgInit ICS;

}; // namespace wcd

#endif
