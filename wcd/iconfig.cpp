#ifndef WCD_ICONFIG_CPP
#define WCD_ICONFIG_CPP

#ifndef WCD_ICONFIG_H
#include "iconfig.h"
#endif

namespace wcd {

using namespace zpp;

CfgInit ICS;


CfgInit::CfgInit() : state_init() {}

void 
CfgInit::init();
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

	zstr_mgr sval = getValue(arg, true, null_val);
	hw.set(ICS.k_driver, sval);

	arg.reset();
	arg.push_back(ICS.k_host);
	arg.push_back(ICS.k_hostname);
	sval = getValue(arg, false, null_val);
	hw.set(ICS.k_host, sval);

}


zval_mgr 
IConfig::getValue(zval_user keys, bool required, zval_user ifnot)
{
	zval_mgr result;

	if (keys.isString())
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
	if (!required)
	{
		result = ifnot;
		return result;
	}
	zstr_buffer msg;

	buf << "Db IConfig needs : ";

	if (keys.isArray())
	{
		zval_mgr cat = implode(ICS.msg_or, keys);
		buf << cat.zstr();
	}
	else {
		buf << keys.zstr();
	}
	zstr_mgr msg = buf.zstr();
	zend_throw_error(zend_cd_error, "%s", msg.data());
	return result;
}



};

#endif
