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

void 
UserData::construct()
{
	obj_ptr self(self_);
#ifdef UVALPTR
	userName_v = self.property_ptr(UDi.userName_p);
	roles_v = self.property_ptr(UDi.roles_p);
	email_v = self.property_ptr(UDi.email_p);
	id_v = self.property_ptr(UDi.id_p);
	memberid_v = self.property_ptr(UDi.memberid_p);
	status_v = self.property_ptr(UDi.status_p);
	keys_v = self.property_ptr(UDi.keys_p);
#endif
}

obj_rc 
UserData::addFlash(str_ptr msg, str_ptr status)
{
#ifdef UVALPTR
	htab_rc keys = keys_v.zarray();
	obj_rc fobj = keys.get(UDi.flash_str);
	if (!fobj.ok())
	{
		fobj = Flash::omg.new_zobj();
		val_rc temp(fobj);
		setKey(UDi.flash_str, temp);
	}
	
#else
	obj_ptr self(self_);
	val_ptr keyprop = self.property_ptr(UDi.keys_p);

	htab_ptr keys = keyprop.zarray();

	obj_rc fobj = keys.get(UDi.flash_str);
	if (!fobj.ok())
	{
		fobj = Flash::omg.new_zobj();
		val_rc temp(fobj);

		htab_rw hw(keyprop);

		hw.set(UDi.flash_str, fobj);
	}
#endif
	Flash* fl = zobj_toc<Flash>(fobj);
	fl->add(msg, status);
	return fobj;
}

bool   
UserData::auth(val_ptr role)
{
	int ztype = role.ref_type();
	str_ptr rs;

	switch(ztype)
	{
	case IS_STRING:
		rs = role.zstr();
		if (rs.ok())
		{
			return hasRole(rs);
		}
		break;
	case IS_ARRAY:
		return hasAnyRole(role.zarray());
	default:
		break;
	}
	return false;
}

val_rc 
UserData::getKey(str_ptr key, val_ptr defval)
{
	val_rc result;
#ifdef UVALPTR
	htab_rc data = keys_v.zarray(UDi.keys_p);
	if (data.size())
	{
		result = data.get(key);
	}
#else
	obj_ptr self(self_);
	htab_rc data = self.array_property(UDi.keys_p);
	if (data.size()) {
		result = data.get(key);
	}
#endif
	if (result.isNull())
	{
		result = defval;
	}
	return result;
}

bool   
UserData::hasAnyRole(htab_ptr rolelist)
{
	if (!rolelist.size())
	{
		return false;
	}
	#ifdef UVALPTR
	htab_ptr myroles = roles_v.zarray();
	#else
	obj_ptr self(self_);
	htab_rc myroles = self.array_property(UDi.roles_p);
	#endif
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
	#ifdef UVALPTR
	htab_ptr myroles = roles_v.zarray();
	#else
	obj_ptr self(self_);
	htab_rc myroles = self.array_property(UDi.roles_p);
	#endif
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
	#ifdef UVALPTR
	zend_long id = id_v.zlong();
	#else
	obj_ptr self(self_);
	zend_long id = self.int_property(UDi.id_p);
	#endif
	return (id!=0);
}

void   
UserData::init()
{
	str_ptr e = str_ptr::empty_str();
	val_rc zero((zend_long)0);
	htab_ptr ea = htab_ptr::empty_array();

	#ifdef UVALPTR
	id_v.set_zlong(0);
	memberid_v.set_zlong(0);

	
	status_v.set_zstr(e);
	email_v.set_zstr(e);
	userName_v.set_zstr(e);

	
	roles_v.set_htab(ea);
	keys_v.set_htab(ea);
	#else 
	obj_ptr self(self_);
	
	self.property(UDi.id_p,zero);
	self.property(UDi.memberid_p,zero);

	self.property(UDi.status_p,e);
	self.property(UDi.email_p,e);
	self.property(UDi.userName_p,e);

	self.property(UDi.roles_p,ea);
	self.property(UDi.keys_p,ea);
	#endif

}

bool   
UserData::isGuest()
{
	return (id_v.zlong() == 0);
}

void   
UserData::setGuest()
{
	htab_rc rolelist;

	htab_rw list(rolelist);

	list.push_back(UDi.guest_str);

	setValidUser(UDi.guest_str,rolelist);
}

void   
UserData::setKey(str_ptr key, val_ptr value)
{
#ifdef UVALPTR
	DebugLog* dlog = DebugLog::cpp_global();
	if(dlog)
	{
	dlog->showmem("keys1", keys_v);
	}
	htab_rc kcopy = keys_v.zarray();
	htab_rw keys(kcopy);

	keys.set(key, value);

	keys_v.set_htab(keys);

	if (dlog){
		dlog->dump("keys2", keys_v);
	}
#else
	obj_ptr self(self_);
	val_ptr keyprop = self.property_ptr(UDi.keys_p);
	htab_rw keys(keyprop);
	keys.set(key, value);
#endif

}

void   
UserData::setValidUser(str_ptr name, htab_ptr roles)
{
#ifdef UVALPTR
	id_v.set_zlong(-1);
	status_v.set_zstr(UDi.OK_str);
	userName_v.set_zstr(name);
	roles_v.set_htab(roles);

	memberid_v.set_zlong(0);
	keys_v.set_htab(htab_ptr::empty_array());
	email_v.set_zstr(str_ptr::empty_str());
#else
	val_rc temp((zend_long)-1);
	obj_ptr self(self_);
	str_ptr e = str_ptr::empty_str();
	htab_ptr ea = htab_ptr::empty_array();

	self.property(UDi.id_p,temp);
	
	temp = (zend_long) 0;
	self.property(UDi.memberid_p,temp);

	self.property(UDi.userName_p, name);
	self.property(UDi.roles_p,roles);

	self.property(UDi.status_p,e);
	self.property(UDi.email_p,e);
	self.property(UDi.keys_p,ea);
#endif

}

void   
UserData::unsetKey(str_ptr key)
{
	#ifdef UVALPTR
	htab_rw keys(keys_v);
	keys.unset(key);
	#else
	obj_ptr self(self_);
	htab_rc kcopy = self.array_property(UDi.keys_p);
	htab_rw keys(kcopy);
	keys.unset(key);
	self.property(UDi.keys_p, kcopy);
	#endif
}

void   
UserData::wipekeys()
{
	#ifdef UVALPTR
	htab_ptr ea = htab_ptr::empty_array();
	keys_v.set_htab(ea);
	#else
	obj_ptr self(self_);
	htab_rc kcopy = self.array_property(UDi.keys_p);
	kcopy.init();
	self.property(UDi.keys_p, kcopy);
	#endif
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

ZEND_METHOD(Wcc_Session_UserData, addFlash)
{
	zarg_rd args(execute_data);
	str_ptr msg = args.str(args.need(0));
	str_ptr status = args.str(args.need(1));

	if (!args.throw_errors())
	{
		UserData* cobj = zval_toc<UserData>(ZEND_THIS);
		obj_rc result = cobj->addFlash(msg,  status);
		result.move_zv(return_value);
	}
}

ZEND_METHOD(Wcc_Session_UserData, auth)
{
	zarg_rd args(execute_data);
	val_ptr role = args.string_or_array(args.need(0));
	if (!args.throw_errors())
	{
		UserData* cobj = zval_toc<UserData>(ZEND_THIS);
		bool result = cobj->auth(role);
		RETURN_BOOL(result);
	}
}


//val_rc  UserData::getKey(str_ptr key, val_ptr defval)

ZEND_METHOD(Wcc_Session_UserData, getKey)
{
	zarg_rd args(execute_data);
	str_ptr key = args.str(args.need(0));
	val_ptr adef = args.option(1);
	if (!args.throw_errors())
	{
		UserData* cobj = zval_toc<UserData>(ZEND_THIS);
		val_rc result = cobj->getKey(key, adef);
		result.move_zv(return_value);
	}
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
	str_ptr role = args.str(args.need(0));
	if (!args.throw_errors())
	{
		UserData* cobj = zval_toc<UserData>(ZEND_THIS);
		bool result = cobj->hasRole(role);
		RETURN_BOOL(result);
	}
}

ZEND_METHOD(Wcc_Session_UserData, init)
{
	ZEND_PARSE_PARAMETERS_NONE();

	UserData* cobj = zval_toc<UserData>(ZEND_THIS);
	cobj->init();
}

ZEND_METHOD(Wcc_Session_UserData, isGuest)
{
	ZEND_PARSE_PARAMETERS_NONE();

	UserData* cobj = zval_toc<UserData>(ZEND_THIS);
	bool result = cobj->isGuest();
	RETURN_BOOL(result);
}

ZEND_METHOD(Wcc_Session_UserData, setGuest)
{
	ZEND_PARSE_PARAMETERS_NONE();

	UserData* cobj = zval_toc<UserData>(ZEND_THIS);
	cobj->setGuest();
}

ZEND_METHOD(Wcc_Session_UserData, setKey)
{
	zarg_rd args(execute_data);
	str_ptr key = args.str(args.need(0));
	val_ptr value = args.need(1);

	if (!args.throw_errors())
	{
		UserData* cobj = zval_toc<UserData>(ZEND_THIS);
		cobj->setKey(key, value);
	}
}

ZEND_METHOD(Wcc_Session_UserData, setValidUser)
{
	zarg_rd args(execute_data);
	str_ptr name = args.str(args.need(0));
	htab_ptr roles = args.htab(args.need(1));

	if (!args.throw_errors())
	{
		UserData* cobj = zval_toc<UserData>(ZEND_THIS);
		cobj->setValidUser(name, roles);
	}	
}

ZEND_METHOD(Wcc_Session_UserData, unsetKey)
{
	zarg_rd args(execute_data);
	str_ptr key = args.str(args.need(0));

	if (!args.throw_errors())
	{
		UserData* cobj = zval_toc<UserData>(ZEND_THIS);
		cobj->unsetKey(key);
	}	
}

ZEND_METHOD(Wcc_Session_UserData, wipekeys)
{
	ZEND_PARSE_PARAMETERS_NONE();
	UserData* cobj = zval_toc<UserData>(ZEND_THIS);
	cobj->wipekeys();
}

PHP_MINIT_FUNCTION(Session_UserData_reg)
{
	auto ce = register_class_Wcc_Session_UserData();

	UserData::omg.classEntry(ce);

	return SUCCESS;
}

#endif
