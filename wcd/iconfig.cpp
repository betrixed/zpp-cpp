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

namespace wcd {

using namespace zpp;

CfgInit ICS;


CfgInit::CfgInit() : state_init() {}

void 
CfgInit::init()
{
	msg_or = " | ";
	k_driver = "driver";
	k_host = "host";
	k_port = "port";
	k_database = "database";
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
}

void 
IConfig::assign(htab_read cfg)
{
	htab_write hw(data_);

	htab_mgr   sarray;

	htab_write arg(sarray);
	arg.push_items(ICS.k_driver, ICS.k_adapter);

	//arg.push_back(ICS.k_driver);
	//arg.push_back(ICS.k_adapter);

	zval_mgr null_val;
	zval_mgr keys(arg);

	zstr_mgr sval = getValue(keys, true, null_val);
	hw.set(ICS.k_driver, sval);

	arg.clear();

	arg.push_back(ICS.k_host);
	arg.push_back(ICS.k_hostname);


	sval = getValue(keys, false, null_val);
	hw.set(ICS.k_host, sval);

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

};//namespace wcd

using namespace wcd;

PHP_MINIT_FUNCTION(Wcd_IConfig_reg)
{
	IConfig::omg.classEntry(register_class_Wcd_IConfig());

	return SUCCESS;
}

#endif
