#ifndef WCD_ICONFIG_CPP
#define WCD_ICONFIG_CPP

#ifndef WCD_ICONFIG_H
#include "iconfig.h"
#endif

#ifndef ICONFIG_ARGINFO_H
#define ICONFIG_ARGINFO_H
extern "C" {
#include "stub/iconfig_arginfo.h"
}
#endif

#ifndef WCD_ISERVER_H
#include "iserver.h"
#endif

#ifndef WCD_IBUILD_H
#include "ibuild.h"
#endif

namespace wcd {

using namespace zpp;

CfgInit ICS;

base_obj_mgr<IConfig> IConfig::omg;


CfgInit::CfgInit() : state_init() {}

void 
CfgInit::init()
{
	msg_or = " | ";
	k_driver = "driver";
	k_host = "host";
	k_port = "port";
	k_database = "database";
	k_dbname = "dbname";

	k_username = "username";
	k_password = "password";
	k_charset = "charset";
	k_collation = "collation";
	k_sql_class = "sqlclass";
	k_dml_class = "dmlclass";
	k_rel_class = "relclass";
	k_model_ns = "model_ns";
	k_model = "model";
	k_processor = "processor";
	k_hostname = "hostname";
	k_localhost = "localhost";
	k_adapter = "adapter";
	k_user = "user";

	k_prepare = "prepare";
	k_execute = "execute";
	db_models_ns = "Db\\Models";


	 mykey = "mykey";
	 data_str = "data";
	 cfg_str = "cfg_";
}

void 
IConfig::debug_info(htab_write di)
{
	di.set(ICS.mykey, mykey_);
	di.set(ICS.data_str, data_);
	di.set(ICS.cfg_str, cfg_);
}

void
IConfig::set_data(zstr_user key, zval_user values, bool required, zval_user ifnot)
{
	htab_write hw(data_);
	//showarray("data_ - ", data_);
	zval_mgr dput = getValue(values, required, ifnot);
	hw.set(key, dput);
	//showarray("data_ - ", data_);
}
void 
IConfig::assign(htab_read cfg)
{
	cfg_ = cfg;
	zval_mgr null_val;
	zval_mgr value;

	zval_mgr   keys_mgr;   // if passing list of values
	htab_write keys(keys_mgr); // do not reassign to keys_mgr!!

	zval_mgr   skey; // if passing single value
	//showarray("assign - ", sarray);

	keys.push_items(ICS.k_driver, ICS.k_adapter);

	//showarray("items - ", sarray);

	//arg.push_back(ICS.k_driver);
	//arg.push_back(ICS.k_adapter);

	set_data(ICS.k_driver, keys_mgr, true, null_val);

	keys.clear();
	keys.push_items(ICS.k_host,ICS.k_hostname);

	null_val = ICS.k_localhost;
	set_data(ICS.k_host, keys_mgr, false, null_val);

	skey = ICS.k_port;
	null_val.set_null();
	value = getValue(skey, false, null_val);
	value.toLong();
	set_data(ICS.k_port, skey, false, null_val);

	keys.clear();
	keys.push_items(ICS.k_dbname, ICS.k_database);
	set_data(ICS.k_dbname, keys_mgr, true, null_val);

	keys.clear();
	keys.push_items(ICS.k_username, ICS.k_user);

	null_val = zstr_mgr::empty_str();
	set_data(ICS.k_username, keys_mgr, false, null_val);

	skey = ICS.k_password;
	set_data(ICS.k_password, skey, false,  null_val);

	skey = ICS.k_charset;
	set_data(ICS.k_charset, skey, false, null_val);

	skey = ICS.k_collation;
	set_data(ICS.k_collation, skey, false, null_val);

	skey = ICS.k_processor;
	null_val.set_null();
	set_data(ICS.k_processor, skey, false, null_val);

	skey = ICS.k_dml_class;
	null_val = zstr_user(IBuild::omg.class_name());
	set_data(ICS.k_dml_class, skey, false, null_val);


	skey = ICS.k_model_ns;
	null_val = ICS.db_models_ns;
	set_data(ICS.k_model_ns, skey, false, null_val);
}

zval_mgr 
IConfig::getValue(zstr_user key, bool required, zval_user ifnot)
{
	zval_mgr result = cfg_.get(key);
	if (!result.isNull())
	{
		return result;
	}
	if (!required)
	{
		result = ifnot;
		return result;
	}
	zstr_buffer buf;
	buf << "IConfig needs value for : '" << key << '\'';
	zend_throw_error(zend_ce_error, buf.data());
	return result;
}

zval_mgr 
IConfig::getValue(zval_user keys, bool required, zval_user ifnot)
{
	zval_mgr result;

	bool asString = keys.isString();

	if (asString)
	{
		zval_user value = cfg_.get(keys.zstr());
		if (!value.isNull())
		{
			result = value;
			return result;
		}
	}
	else if (keys.isArray())
	{
		htab_walk wk;

		auto keyval = wk.value();
		for(wk.start(keys.zarray()); wk.ok(); wk.next())
		{
			zval_user value = cfg_.get(keyval.zstr());
			if (!value.isNull())
			{
				result = value;
				return result;
			}
		}
	}
	else {
		return result;
	}
	if (!required)
	{
		result = ifnot;
		return result;
	}
	zstr_buffer buf;

	buf << "Db IConfig needs : ";

	if (!asString)
	{
		zval_mgr cat = implode(ICS.msg_or, keys);
		buf << zval_user(cat).zstr();
	}
	else {
		buf << keys.zstr();
	}
	zstr_mgr msg = buf.zstr();
	zend_throw_error(zend_ce_error, "%s", msg.data());
	return result;
}

void      
IConfig::setMyKey(zstr_user key)
{
	mykey_ = key;
}

zstr_user 
IConfig::getMyKey()
{
	return mykey_;
}


zval_mgr 
IConfig::get(zstr_user name)
{
	return data_.get(name);
}

void     
IConfig::set(zstr_user name, zval_user value)
{
	htab_write hw(data_);

	hw.set(name, value);
}

void     
IConfig::set(zstr_user name, zstr_user value)
{
	htab_write hw(data_);

	hw.set(name, value);
}

htab_read 
IConfig::getArray()
{
	return data_;
}

zstr_mgr 
IConfig::getCharset()
{
	return getValue(ICS.k_charset, false, zval_mgr::empty_str());
}

zstr_mgr 
IConfig::getCollation()
{
	return getValue(ICS.k_collation, false, zval_mgr::empty_str());
}

zobj_mgr 
IConfig::newConnect(zstr_user name)
{
	zstr_mgr dclass(getDriverClass());
	showstr("dclass", dclass);

	htab_mgr args_mgr;
	htab_write args(args_mgr);
	args.push_back(vobj());
	args.push_back(name);
	showdata("dclass args", args_mgr);

	zobj_mgr result(ReflectCache::staticInstanceArgs(dclass, args_mgr));
	showobj("Result ", result);
	return result;
}

zstr_mgr 
IConfig::stringVal(zstr_user key)
{
	zval_user result = data_.get(key);
	if (result.isNull())
	{
		return zstr_mgr::empty_str();
	}
	else {
		return result;
	}
}

zstr_mgr
IConfig::getDriverName()
{
	return stringVal(ICS.k_driver);
}

zstr_mgr
IConfig::getDriverClass()
{
	zstr_mgr dname = getDriverName();
	zobj_user servers = Services::getOne(IServer::omg.class_name());
	IServer* sv = zobj_toc<IServer>(servers);
	return sv->getDriverClass(dname);
}

zstr_mgr 
IConfig::getDmlBuildClass()
{
	return stringVal(ICS.k_dml_class);
}

zstr_mgr 
IConfig::getPassword()
{
	return stringVal(ICS.k_password);
}

zstr_mgr 
IConfig::getUsername()
{
	return stringVal(ICS.k_username);
}

int 
IConfig::getPort()
{
	zval_mgr result = getValue(ICS.k_port,false,zval_mgr());
	return result.zlong();
}

zstr_mgr 
IConfig::getHost()
{
	return stringVal(ICS.k_host);
}

zstr_mgr 
IConfig::getDatabase()
{
	zval_mgr list_mgr;
	htab_write list(list_mgr);
	list.push_items(ICS.k_dbname, ICS.k_database);

	zval_mgr result = getValue(list_mgr, true, zval_mgr::empty_str());
	return result;
}

zobj_mgr
IConfig::newSql()
{
	zstr_mgr obclass = getSqlClass();

	return ReflectCache::staticInstance(obclass);
}

zstr_mgr 
IConfig::getSqlClass()
{
	zstr_mgr pc = stringVal(ICS.k_processor);
	if (pc.size())
	{
		return pc;
	}
	zobj_user servers = Services::getOne(IServer::omg.class_name());

	zstr_mgr dname = getDriverName();
	IServer* sv = zobj_toc<IServer>(servers);
	pc = sv->getSqlClass(dname);
	set(ICS.k_processor, pc);
	return pc;
}

};//namespace wcd

using namespace wcd;

//void assign(htab_read cfg);
ZEND_METHOD(Wcd_IConfig, assign)
{
	zval* data;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(data)
	ZEND_PARSE_PARAMETERS_END();

	IConfig* cobj = zval_toc<IConfig>(ZEND_THIS);
	cobj->assign(data);
}

//zval_mgr get(zstr_user name);
ZEND_METHOD(Wcd_IConfig, get)
{
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	IConfig* cobj = zval_toc<IConfig>(ZEND_THIS);
	zval_mgr result = cobj->get(name);	

	result.move_zv(return_value);
}

//zval_mgr get(zstr_user name);
ZEND_METHOD(Wcd_IConfig, set)
{
	zend_string* name;
	zval*        data;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_STR(name)
	Z_PARAM_ZVAL(data)
	ZEND_PARSE_PARAMETERS_END();

	IConfig* cobj = zval_toc<IConfig>(ZEND_THIS);
	cobj->set(name, zval_user(data));	
}

ZEND_METHOD(Wcd_IConfig, getArray)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IConfig* cobj = zval_toc<IConfig>(ZEND_THIS);

	htab_read result = cobj->getArray();

	result.return_zv(return_value);
}

//zstr_mgr getCharset()
ZEND_METHOD(Wcd_IConfig, getCharset)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IConfig* cobj = zval_toc<IConfig>(ZEND_THIS);

	zstr_mgr result = cobj->getCharset();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IConfig, getCollation)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IConfig* cobj = zval_toc<IConfig>(ZEND_THIS);

	zstr_mgr result = cobj->getCollation();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IConfig, getSqlClass)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IConfig* cobj = zval_toc<IConfig>(ZEND_THIS);

	zstr_mgr result = cobj->getSqlClass();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IConfig, getDmlBuildClass)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IConfig* cobj = zval_toc<IConfig>(ZEND_THIS);

	zstr_mgr result = cobj->getDmlBuildClass();

	result.move_zv(return_value);
}


ZEND_METHOD(Wcd_IConfig, newConnect)
{
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	IConfig* cobj = zval_toc<IConfig>(ZEND_THIS);
	zobj_mgr result = cobj->newConnect(name);	

	result.move_zv(return_value);	
}

ZEND_METHOD(Wcd_IConfig, getDriverClass)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IConfig* cobj = zval_toc<IConfig>(ZEND_THIS);

	zstr_mgr result = cobj->getDriverClass();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IConfig, getPassword)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IConfig* cobj = zval_toc<IConfig>(ZEND_THIS);

	zstr_mgr result = cobj->getPassword();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IConfig, getUsername)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IConfig* cobj = zval_toc<IConfig>(ZEND_THIS);

	zstr_mgr result = cobj->getUsername();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IConfig, getPort)
{
 	ZEND_PARSE_PARAMETERS_NONE();

	IConfig* cobj = zval_toc<IConfig>(ZEND_THIS);

	zend_long result = cobj->getPort();

	RETURN_LONG(result);
}

ZEND_METHOD(Wcd_IConfig, getHost)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IConfig* cobj = zval_toc<IConfig>(ZEND_THIS);

	zstr_mgr result = cobj->getHost();

	result.move_zv(return_value);	
}

ZEND_METHOD(Wcd_IConfig, getDatabase)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IConfig* cobj = zval_toc<IConfig>(ZEND_THIS);

	zstr_mgr result = cobj->getDatabase();

	result.move_zv(return_value);	
}

ZEND_METHOD(Wcd_IConfig, getMyKey)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IConfig* cobj = zval_toc<IConfig>(ZEND_THIS);

	zstr_user result = cobj->getMyKey();

	result.return_zv(return_value);	
}

//void setMyKey(zstr_user key)
ZEND_METHOD(Wcd_IConfig, setMyKey)
{
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	IConfig* cobj = zval_toc<IConfig>(ZEND_THIS);
	cobj->setMyKey(name);
}

ZEND_METHOD(Wcd_IConfig, newSql)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IConfig* cobj = zval_toc<IConfig>(ZEND_THIS);

	zobj_mgr result = cobj->newSql();

	result.move_zv(return_value);
}


PHP_MINIT_FUNCTION(Wcd_IConfig_reg)
{
	IConfig::omg.classEntry(register_class_Wcd_IConfig());

	return SUCCESS;
}

#endif
