#ifndef SESSION_USERDATA_CPP
#define SESSION_USERDATA_CPP

#ifndef SESSION_USERDATA_H
#include "userdata.h"
#endif

#ifndef WCC_CONFIG_H
#include "config.h"
#endif

#ifndef WCC_DEBUG_LOG_H
#include "debuglog.h"
#endif

#ifndef USERDATA_ARGINFO_H
#define USERDATA_ARGINFO_H
extern "C" {
	#include "stub/userdata_arginfo.h"
}
#endif


namespace wcc {

base_obj_mgr<UserData> UserData::omg;


UserDataInit UDi;

void UserDataInit::init()
{
	lines_str = "lines";
	text_str = "text";
	status_str = "status";
	config_str = "config";

	user_p = "user";
	userName_p = "userName";
	name_p = "name";
	roles_p = "roles";
	email_p = "email";
	id_p = "id";
	memberid_p = "memberid";
	status_p = "status";
	keys_p = "keys";

	flash_str = "flash";
	OK_str = "OK";
	status_info = "info";

	admin_user = "admin";
	Admin_str = "Admin";
	Editor_str = "Editor";
	Guest_str = "Guest";
	User_str = "User";

	getadapter_str = "getadapter";
	gc_str = "gc";
	session_str = "session";
	destroy_fn = "destroy";
	start_fn = "start";
	userData_p = "userData";

	doWrite = "doWrite";
	wasRead = "wasRead";

	TTLroles = "TTLBoostRoles";
	TTLtime = "TTLBoostTime";
}

void 
UserData::init()
{
	obj_ptr self(self_);
	
	htab_ptr ea = htab_ptr::empty_array();
	obj_rc user = Config::omg.new_zobj();

	self.property(UDi.user_p, user);
	self.property(UDi.flash_str, ea);
	self.property(UDi.keys_p, ea);
	self.property(UDi.roles_p, ea);
}

void 
UserData::construct()
{
	obj_ptr self(self_);
	val_ptr user = self.property_ptr(UDi.user_p);

	if (!user.isObject())
	{
		init();
	}		
}

obj_rc 
UserData::newobj()
{
	obj_rc result = UserData::omg.new_zobj();
	UserData* ud = zobj_toc<UserData>(result);
	ud->construct();
	return result;
}

bool   
UserData::hasAnyRole(htab_ptr rolelist)
{
	if (!rolelist.size())
	{
		return false;
	}

	obj_ptr self(self_);
	htab_rc myroles = self.array_property(UDi.roles_p);

	if (!myroles.size())
	{
		return false;
	}
	htab_walk wk;
	auto rname = wk.value();
	for(wk.start(rolelist); wk.ok(); wk.next())
	{
		if (myroles.value_index(rname.zstr()) >= 0)
		{
			return true;
		}
	}
	return false;
}

bool   
UserData::hasRole(str_ptr role)
{
	obj_ptr self(self_);
	htab_rc myroles = self.array_property(UDi.roles_p);

	if (!myroles.size())
	{
		return false;
	}
	htab_walk wk;
	auto rname = wk.value();
	for(wk.start(myroles); wk.ok(); wk.next())
	{
		if (zs_cmp(rname.zstr(), role)==0)
		{
			return true;
		}
	}
	return false;
}

bool
UserData::isLoggedIn(val_ptr role)
{
	if (role.isString())
	{
		return hasRole(role.zstr());
	}
	if (role.isArray())
	{
		return hasAnyRole(role.zarray());
	}
	return false;
}

obj_rc   
UserData::getUser()
{
	obj_rc result;
	obj_ptr self(self_);
	result = self.obj_property(UDi.user_p);
	if (!result.ok())
	{
		result = Config::omg.new_zobj();
		self.property(UDi.user_p, result);
	}
	return result;
}

/*

void
UserData::__unserialize(htab_ptr htab)
{
	val_ptr temp;
	DebugLog* log = DebugLog::instance();

	if (log)
	{
		log->dump("UserData", htab);
	}

	//htab_own showme;
	//debug_info(showme);
	//showme.show_data("unserialized");
}*/

} //wcc namespace

using namespace wcc;
using namespace zpp;

ZEND_METHOD(Wcc_Session_UserData, __construct)
{
	ZEND_PARSE_PARAMETERS_NONE();

	UserData* cobj = zval_toc<UserData>(ZEND_THIS);
	cobj->construct();
}


ZEND_METHOD(Wcc_Session_UserData, hasAnyRole)
{
	zarg_rd args(execute_data);
	htab_ptr rolelist = args.htab(args.need(0));
	if (!args.throw_errors(__FUNCTION__))
	{
		UserData* cobj = zval_toc<UserData>(ZEND_THIS);
		bool result = cobj->hasAnyRole(rolelist);
		RETURN_BOOL(result);
	}
}

ZEND_METHOD(Wcc_Session_UserData, hasRole)
{
	zarg_rd args(execute_data);
	str_ptr role = args.str(args.need(0));
	if (!args.throw_errors(__FUNCTION__))
	{
		UserData* cobj = zval_toc<UserData>(ZEND_THIS);
		bool result = cobj->hasRole(role);
		RETURN_BOOL(result);
	}
}


ZEND_METHOD(Wcc_Session_UserData, isLoggedIn)
{
	zarg_rd args(execute_data);
	val_ptr role = args.string_or_array(args.need(0));
	if (!args.throw_errors(__FUNCTION__))
	{
		UserData* cobj = zval_toc<UserData>(ZEND_THIS);
		bool result = cobj->isLoggedIn(role);
		RETURN_BOOL(result);
	}
}


ZEND_METHOD(Wcc_Session_UserData, getUser)
{
	zarg_rd args(execute_data);
	if (!args.throw_errors(__FUNCTION__))
	{
		UserData* cobj = zval_toc<UserData>(ZEND_THIS);
		obj_rc result = cobj->getUser();
		result.move_zv(return_value);
	}
}

/*
PHP_METHOD(Wcc_Session_UserData, __unserialize)
{
	zval* data;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ARRAY(data)
	ZEND_PARSE_PARAMETERS_END();

	htab_ptr hr(data);

	Route* cobj = zval_toc<Route>(ZEND_THIS);

	cobj->__unserialize(hr);

}*/

PHP_MINIT_FUNCTION(Session_UserData_reg)
{
	auto ce = register_class_Wcc_Session_UserData();

	UserData::omg.classEntry(ce);

	STATE_INIT_ADD(UDi)
	
	return SUCCESS;
}

#endif
