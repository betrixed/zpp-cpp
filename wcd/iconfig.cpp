#ifndef WCD_ICONFIG_CPP
#define WCD_ICONFIG_CPP

#ifndef WCD_ICONFIG_H
#include "iconfig.h"
#endif

#ifndef REFLECT_CACHE_H
#include "wcc/reflect_cache.h"
#endif

#ifndef WCC_SERVICES_H
#include "wcc/services.h"
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
using namespace wcc;

CfgInit ICS;

base_obj_mgr<IConfig> IConfig::omg;


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
IConfig::debug_info(htab_rw di)
{
	di.set(ICS.mykey, mykey_);
	di.set(ICS.data_str, data_);
	di.set(ICS.cfg_str, cfg_);
}

// transform raw values (if any) into final form,
// make error if missing and required is true
error_return
IConfig::set_data(str_ptr key, val_ptr values, bool required, val_ptr ifnot)
{
	error_return result;

	htab_rw hw(data_);
	//showarray("data_ - ", data_);
	val_return dput = getValue(values, required, ifnot);
	if (!dput.has_errors())
	{
		hw.set(key, dput.value_);
	}
	else {
		result = std::move(dput);
	}
	return result;
}

error_return 
IConfig::assign(htab_ptr cfg)
{
	error_return result;

	cfg_ = cfg;
	val_rc null_val;
	val_return vret;

	val_rc   keys_mgr;   // if passing list of values
	htab_rw keys(keys_mgr); // do not reassign to keys_mgr!!

	val_rc   skey; // if passing single value

	//showarray("assign - ", sarray);

	keys.push_items(ICS.k_driver, ICS.k_adapter);

	//showarray("items - ", sarray);

	//arg.push_back(ICS.k_driver);
	//arg.push_back(ICS.k_adapter);

	result = set_data(ICS.k_driver, keys_mgr, true, null_val);
	if (result.has_errors())
	{
		return result;
	}

	keys.clear();
	keys.push_items(ICS.k_host, ICS.k_hostname);

	null_val = ICS.k_localhost;
	// not required
	set_data(ICS.k_host, keys_mgr, false, null_val);

	skey = ICS.k_port;
	null_val.set_null();

	// not required
	vret = getValue(skey, false, null_val);
	if (vret.value_.isLong() || vret.value_.isString())
	{
		vret.value_.toLong();
		set(ICS.k_port, vret.value_);
	}

	keys.clear();
	keys.push_items(ICS.k_dbname, ICS.k_database);
	
	set_data(ICS.k_dbname, keys_mgr, true, null_val);

	keys.clear();
	keys.push_items(ICS.k_username, ICS.k_user);

	null_val = str_rc::empty_str();
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
	null_val = str_ptr(IBuild::omg.class_name());
	set_data(ICS.k_dml_class, skey, false, null_val);


	skey = ICS.k_model_ns;
	null_val = ICS.db_models_ns;
	set_data(ICS.k_model_ns, skey, false, null_val);

	return result;
}

val_return
IConfig::getValue(str_ptr key, bool required, val_ptr ifnot)
{
	val_return result;
	val_ptr test = cfg_.get(key);
	if (!test.isNull())
	{
		result.value_ = test;
	}
	else if (!required)
	{
		result.value_ = ifnot;
	}
	else {
		result.error() << "IConfig needs value for : '" << key << '\'';
	}
	return result;
}

val_return 
IConfig::getValue(val_ptr keys, bool required, val_ptr ifnot)
{
	val_return result;

	bool asString = keys.isString();
	val_ptr test;

	if (asString)
	{
		test = cfg_.get(keys.zstr());
		if (!test.isNull())
		{
			result.value_ = test;
			return result;
		}
	}
	else if (keys.isArray())
	{
		htab_walk wk;

		auto keyval = wk.value();
		for(wk.start(keys.zarray()); wk.ok(); wk.next())
		{
			test = cfg_.get(keyval.zstr());
			if (!test.isNull())
			{
				result.value_ = test;
				return result;
			}
		}
	}
	else {
		return result;
	}
	if (!required)
	{
		result.value_ = ifnot;
		return result;
	}

	result.error() << "Db IConfig needs : ";

	if (!asString)
	{
		val_rc cat = implode(ICS.msg_or, keys);
		result.error() << val_ptr(cat).zstr();
	}
	else {
		result.error() << keys.zstr();
	}

	return result;
}

void      
IConfig::setMyKey(str_ptr key)
{
	mykey_ = key;
}

str_ptr 
IConfig::getMyKey()
{
	return mykey_;
}


val_rc 
IConfig::get(str_ptr name)
{
	return data_.get(name);
}

void     
IConfig::set(str_ptr name, val_ptr value)
{
	htab_rw hw(data_);

	hw.set(name, value);
}

void     
IConfig::set(str_ptr name, str_ptr value)
{
	htab_rw hw(data_);

	hw.set(name, value);
}

htab_ptr 
IConfig::getArray()
{
	return data_;
}

str_rc 
IConfig::getCharset()
{
	val_return result = getValue(ICS.k_charset, false, val_rc::empty_str());
	if (result.value_.isString())
	{
		return result.value_.zstr();
	}
	else {
		return str_rc();
	}
}

str_rc 
IConfig::getCollation()
{
	val_return result =  getValue(ICS.k_collation, false, val_rc::empty_str());
	if (result.value_.isString())
	{
		return result.value_.zstr();
	}
	else {
		return str_rc();
	}
}

obj_return 
IConfig::newConnect(str_ptr name)
{
	str_rc dclass(getDriverClass());
	htab_rc args_mgr;
	htab_rw args(args_mgr);
	args.push_back(vobj());
	args.push_back(name);

	obj_return result;
	result.value_ = ReflectCache::staticInstanceArgs(dclass, args_mgr);
	
	if (!result.value_.ok())
	{
		result.error() << "Unable to create class ", dclass.data();
	}
	return result;
}

str_rc 
IConfig::stringVal(str_ptr key)
{
	val_ptr result = data_.get(key);
	if (result.isNull())
	{
		return str_rc::empty_str();
	}
	else {
		//showmem("StringVal result", result);
		return result;
	}
}

str_rc
IConfig::getDriverName()
{
	str_rc result = stringVal(ICS.k_driver);
	return result;
}

str_rc
IConfig::getDriverClass()
{
	str_rc dname = getDriverName();
	//showstr("Driver Name", dname);

	obj_ptr servers = Services::getOne(IServer::omg.class_name());
	IServer* sv = zobj_toc<IServer>(servers);
	return sv->getDriverClass(dname);
}

str_rc 
IConfig::getDmlBuildClass()
{
	return stringVal(ICS.k_dml_class);
}

str_rc 
IConfig::getPassword()
{
	return stringVal(ICS.k_password);
}

str_rc 
IConfig::getUsername()
{
	return stringVal(ICS.k_username);
}

int 
IConfig::getPort()
{
	val_return result = getValue(ICS.k_port,false,val_rc());
	return result.value_.zlong();
}

str_rc 
IConfig::getHost()
{
	return stringVal(ICS.k_host);
}

str_rc 
IConfig::getDatabase()
{
	val_rc list_mgr;
	htab_rw list(list_mgr);
	list.push_items(ICS.k_dbname, ICS.k_database);

	val_return result = getValue(list_mgr, true, val_rc::empty_str());
	return result.value_.zstr();
}

obj_rc
IConfig::newSql()
{
	str_rc obclass = getSqlClass();

	return ReflectCache::staticInstance(obclass);
}

str_rc 
IConfig::getSqlClass()
{
	str_rc pc = stringVal(ICS.k_processor);
	if (pc.size())
	{
		return pc;
	}
	obj_ptr servers = Services::getOne(IServer::omg.class_name());

	str_rc dname = getDriverName();
	IServer* sv = zobj_toc<IServer>(servers);
	pc = sv->getSqlClass(dname);
	set(ICS.k_processor, pc);
	return pc;
}

};//namespace wcd

using namespace wcd;

//void assign(htab_ptr cfg);
ZEND_METHOD(Wcd_IConfig, assign)
{
	zval* data;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(data)
	ZEND_PARSE_PARAMETERS_END();

	IConfig* cobj = zval_toc<IConfig>(ZEND_THIS);
	cobj->assign(data);
}

//val_rc get(str_ptr name);
ZEND_METHOD(Wcd_IConfig, get)
{
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	IConfig* cobj = zval_toc<IConfig>(ZEND_THIS);
	val_rc result = cobj->get(name);	

	result.move_zv(return_value);
}

//val_rc get(str_ptr name);
ZEND_METHOD(Wcd_IConfig, set)
{
	zend_string* name;
	zval*        data;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_STR(name)
	Z_PARAM_ZVAL(data)
	ZEND_PARSE_PARAMETERS_END();

	IConfig* cobj = zval_toc<IConfig>(ZEND_THIS);
	cobj->set(name, val_ptr(data));	
}

ZEND_METHOD(Wcd_IConfig, getArray)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IConfig* cobj = zval_toc<IConfig>(ZEND_THIS);

	htab_ptr result = cobj->getArray();

	result.copy_zv(return_value);
}

//str_rc getCharset()
ZEND_METHOD(Wcd_IConfig, getCharset)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IConfig* cobj = zval_toc<IConfig>(ZEND_THIS);

	str_rc result = cobj->getCharset();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IConfig, getCollation)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IConfig* cobj = zval_toc<IConfig>(ZEND_THIS);

	str_rc result = cobj->getCollation();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IConfig, getSqlClass)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IConfig* cobj = zval_toc<IConfig>(ZEND_THIS);

	str_rc result = cobj->getSqlClass();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IConfig, getDmlBuildClass)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IConfig* cobj = zval_toc<IConfig>(ZEND_THIS);

	str_rc result = cobj->getDmlBuildClass();

	result.move_zv(return_value);
}


ZEND_METHOD(Wcd_IConfig, newConnect)
{
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	IConfig* cobj = zval_toc<IConfig>(ZEND_THIS);
	obj_return result = cobj->newConnect(name);	
	result.throw_errors();

	result.value_.move_zv(return_value);	
}

ZEND_METHOD(Wcd_IConfig, getDriverClass)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IConfig* cobj = zval_toc<IConfig>(ZEND_THIS);

	str_rc result = cobj->getDriverClass();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IConfig, getPassword)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IConfig* cobj = zval_toc<IConfig>(ZEND_THIS);

	str_rc result = cobj->getPassword();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IConfig, getUsername)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IConfig* cobj = zval_toc<IConfig>(ZEND_THIS);

	str_rc result = cobj->getUsername();

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

	str_rc result = cobj->getHost();

	result.move_zv(return_value);	
}

ZEND_METHOD(Wcd_IConfig, getDatabase)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IConfig* cobj = zval_toc<IConfig>(ZEND_THIS);

	str_rc result = cobj->getDatabase();

	result.move_zv(return_value);	
}

ZEND_METHOD(Wcd_IConfig, getMyKey)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IConfig* cobj = zval_toc<IConfig>(ZEND_THIS);

	str_ptr result = cobj->getMyKey();

	result.copy_zv(return_value);	
}

//void setMyKey(str_ptr key)
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

	obj_rc result = cobj->newSql();

	result.move_zv(return_value);
}


PHP_MINIT_FUNCTION(Wcd_IConfig_reg)
{
	IConfig::omg.classEntry(register_class_Wcd_IConfig());

	STATE_INIT_ADD(ICS)
	
	return SUCCESS;
}

#endif
