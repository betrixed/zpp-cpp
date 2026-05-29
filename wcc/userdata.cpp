#ifndef SESSION_USERDATA_CPP
#define SESSION_USERDATA_CPP

#ifndef SESSION_FLASH_H
#include "flash.h"
#endif

#ifndef SESSION_USERDATA_H
#include "userdata.h"
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

	user_p = "user";
	userName_p = "userName";
	roles_p = "roles";
	email_p = "email";
	id_p = "id";
	memberid_p = "memberid";
	status_p = "status";
	keys_p = "keys";

	guest_str = "Guest";
	flash_str = "flash";
	OK_str = "OK";

	Admin_str = "Admin";
	Editor_str = "Editor";
}

void 
UserData::construct()
{
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
UserData::hasUser()
{

	obj_ptr self(self_);
	zend_long id = self.int_property(UDi._p);

	return (id!=0);
}


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
	if (!args.throw_errors())
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
	if (!args.throw_errors())
	{
		UserData* cobj = zval_toc<UserData>(ZEND_THIS);
		bool result = cobj->hasRole(role);
		RETURN_BOOL(result);
	}
}

ZEND_METHOD(Wcc_Session_UserData, hasUser)
{
	zarg_rd args(execute_data);
	if (!args.throw_errors())
	{
		UserData* cobj = zval_toc<UserData>(ZEND_THIS);
		bool result = cobj->hasUser();
		RETURN_BOOL(result);
	}
}


PHP_MINIT_FUNCTION(Session_UserData_reg)
{
	auto ce = register_class_Wcc_Session_UserData();

	UserData::omg.classEntry(ce);

	return SUCCESS;
}

#endif
