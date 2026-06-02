#ifndef WCC_USERSESSION_CPP
#define WCC_USERSESSION_CPP

#ifndef WCC_USERSESSION_H
#include "usersession.h"
#endif

#ifndef WCC_SERVICES_H
#include "services.h"
#endif

#ifndef WCC_CONFIG_H
#include "config.h"
#endif

#ifndef  PHP_SESSION_H
extern "C" {
	#include <ext/session/php_session.h>
};
#endif

#ifndef USERSESSION_ARGINFO_H
#define USERSESSION_ARGINFO_H
extern "C" {
	#include "stub/usersession_arginfo.h"
}
#endif

namespace wcc {

using namespace zpp;

base_obj_mgr<UserSession> UserSession::omg;



void UserSession::debug_info(htab_rw hw)
{
	hw.set(UDi.session_str, session_);
	hw.set(UDi.userData_p, data_);
	hw.setbool(UDi.doWrite, doWrite_);
	hw.setbool(UDi.wasRead, wasRead_);
}


UserData* 
UserSession::ud_cpp()
{
	return zobj_toc<UserData>(data_);
}

obj_rc 
UserSession::getAdapter()
{
	obj_rc result;

	if (session_.ok())
	{
		val_rc test = session_.call(UDi.getadapter_str);
		result = test.zobject();
	}
	return result;
}

void 
UserSession::construct()
{
	
	doWrite_ = false;
	wasRead_ = false;
	ended_ = false;
	//session_ = null;
	data_ = UserData::newobj();

}

obj_rc  
UserSession::activate()
{
	doWrite_  = true;
	this->write();
	return obj_rc(self_);
}


void 
UserSession::addFlash(str_ptr text, str_ptr status)
{
	zval* flash = data_.property_ptr(UDi.flash_str);
	htab_rw w(flash);  

	htab_rc line;
	htab_rw wline(line);

	wline.push_back(text);
	wline.push_back(status);

	w.push_back(line);
}


void 
UserSession::addUserRoles(htab_ptr roles)
{
	getUserRoles();  

	val_ptr rlist = data_.property_ptr(UDi.roles_p);
	htab_rw myroles(rlist);

	auto before_ct = myroles.size();
	myroles.merge(roles);
	auto after_ct =  myroles.size();

	if (after_ct > before_ct)
	{
		doWrite_ = true;
	}
}


int  
UserSession::adjustExpiry()
{
	int result = 0;

	if (session_.ok())
	{
		if (data_.ok())
		{
			UserData* ud = ud_cpp();
			if (ud->hasRole(UDi.Admin_str) || ud->hasRole(UDi.Editor_str) )
			{

				obj_rc adapter = getAdapter();
				if (adapter.ok())
				{
					val_rc tsec(24*60*60);

					val_rc test = adapter.call(UDi.gc_str, tsec);

					result = test.zlong();
				}
			}
		}
	}
	return result;
}



bool 
UserSession::auth(val_ptr role)
{
	if (!wasRead_)
	{
		read();
	}
	UserData* ud = ud_cpp();

	if (role.isString())
	{
		return ud->hasRole(role.zstr());
	}
	if (role.isArray())
	{
		return ud->hasAnyRole(role.zarray());
	}
	return false;
}


void UserSession::clearFlash()
{
	data_.property(UDi.flash_str, htab_ptr::empty_array());
}

void UserSession::delayWrite()
{
	doWrite_ = true;
}

void UserSession::flash(str_ptr msg, str_ptr status, htab_ptr exlines)
{
	str_buf buf;

	buf << msg;

	if (exlines.size())
	{
		htab_walk wk;
		auto line = wk.value();
		for(wk.start(exlines); wk.ok(); wk.next())
		{
			buf << "<br>\n" << line.zstr();
		}
	}
	zval* fptr = data_.property_ptr(UDi.flash_str);
	htab_rw list(fptr);
	htab_rc line;
	htab_rw flash(line);
	flash.push_back(buf.zstr());

	if (!status.ok())
	{
		status = UDi.status_info;
	}
	flash.push_back(status);
	list.push_back(line);
}


str_rc
UserSession::getEndTime()
{
	str_rc result;

	if (session_.ok())
	{
		obj_rc adapter = getAdapter();
		if (adapter.ok() && adapter.method_exists(UDi.getexpires_str))
		{
			//timestamp int
			val_rc expires = adapter.call(UDi.getexpires_str);
			if (expires.isLong())
			{
				result = datetime_obj::date(UDi.expires_fmt, expires.zlong());
			}
		}
	}
	if (!result.ok())
	{
		result = UDi.unknown_str;
	}
	return result;
}

htab_rc
UserSession::getFlash()
{
	return data_.array_property(UDi.flash_str);
}


val_rc 
UserSession::getKey(str_ptr key, val_ptr adefault)
{
	zval* array = data_.property_ptr(UDi.keys_p);
	htab_ptr keys(val_ptr(array).zarray());
	val_rc result;

	if (keys.size())
	{
		result = keys.get(key, adefault);
	}
	else {
		result = adefault;
	}
	return result;
}


obj_rc 
UserSession::getSession()
{
	obj_rc result;

	if (ended_)
	{
		return result;
	}
	if (!session_.ok())
	{
		session_ = Services::service(UDi.session_str);
	}
	result = session_;
	return result;
}

obj_rc 
UserSession::getUser()
{
	obj_rc result;
	if (!wasRead_)
	{
		read();
	}
	
	result = data_.obj_property(UDi.user_p);

	if (result.ok())
	{	
		//showobj("getUser 1", result);
		return result;
	}
	result = Config::omg.new_zobj();

	//showobj("getUser 2", result);
	data_.property(UDi.user_p, result);
	doWrite_ = true;
	//showobj("user", result);
	return result;
}


htab_rc UserSession::getUserRoles()
{
	if (!wasRead_)
	{
		read();
	}
	return data_.array_property(UDi.roles_p);
}



obj_rc 
UserSession::guestSession()
{
	setGuest();
	doWrite_ = true;
	write();
	return obj_rc(self_);
}



bool 
UserSession::hasKey(str_ptr key)
{
	zval* keys = data_.property_ptr(UDi.keys_p);
	htab_ptr values(keys);
	return values.has_key(key);
}


bool 
UserSession::hasValues()
{
	zval* keys = data_.property_ptr(UDi.keys_p);
	htab_ptr values(keys);
	return values.size() > 0;

}


bool 
UserSession::isEmpty()
{
	obj_rc user = getUser();
	bool result = true;
	if (user.ok())
	{
		val_ptr id = user.property_ptr(UDi.id_p);
		result = id.is_nullptr() || (id.zlong() == 0);
	}
	return result;
}


bool 
UserSession::isEnded()
{
	return ended_;
}


bool 
UserSession::isLoggedIn(val_ptr roles)
{
	if(!wasRead_)
	{
		read();
	}
	UserData* ud = ud_cpp();
	if (roles.isString())
	{
		return ud->hasRole(roles.zstr());
	}
	if (roles.isArray())
	{
		return ud->hasAnyRole(roles.zarray());
	}
	return false;
}


void 
UserSession::nullify()
{
	read();
	if (!isEmpty())
	{
		wipe();
	}
	if (session_.ok())
	{
		session_.call(UDi.destroy_fn);
	}
	if (session_status() == php_session_status::php_session_active)
	{
		session_write_close();
	}
	session_.init();
	ended_ = true;
}


obj_rc 
UserSession::read()
{
	val_rc test;
	bool exists = false;

	DebugLog* log = DebugLog::cpp_global();

	if (!wasRead_ && !ended_)
	{
		doWrite_ = false;
		obj_rc session = getSession();
		if (session.ok())
		{
			data_.init();
			test = session.call(UDi.start_fn);

			exists = test.isTrue();
		}
		if (exists)
		{
			if (log) {
				
				log->dump("Session object:", session_);
			}
			// a virtual property?
			val_rc ud = session_.property(UDi.userData_p);

			if (log) {

				log->dump("UserData object:", val_ptr(ud));
			}
			if (ud.isObject())
			{
				data_ = ud.zobject();
				
				int change = adjustExpiry();

				if (change > 0)
				{
					doWrite_ = true;
				}

				wasRead_ = true;
			}
			else {
				//zend_printf("Session new\n");
				data_ = UserData::newobj();
				doWrite_ = true;
				wasRead_ = true;
			}
		}
	}
	return obj_rc(self_);
}


str_rc 
UserSession::roles()
{
	str_buf buf;
	str_rc result;
	
	val_ptr pr = data_.property_ptr(UDi.roles_p);
	if (pr.isArray())
	{
		htab_walk wk;
		auto value = wk.value();
		int ct = 0;
		for(wk.start(pr.zarray()); wk.ok(); ct++, wk.next())
		{
			if (ct) {
				buf << ", "; 
			}
			buf << value.zstr();
		}
	}
	result = buf.zstr();
	return result;
}


void 
UserSession::save()
{
	write(true);
}


void
UserSession::saveUser(obj_ptr src, htab_ptr roles)
{
	error_return result;
	val_rc test;

	obj_rc user = getUser();
	if (!user.ok())
	{
		user = Config::omg.new_zobj();
	}

	test = src.property(UDi.id_p);
	user.property(UDi.id_p, test);

	test = src.property(UDi.status_p);
	user.property(UDi.status_p, test);

	test = src.property(UDi.email_p);
	user.property(UDi.email_p, test);


	test = src.property(UDi.name_p);
	user.property(UDi.userName_p, test);

	test = src.property(UDi.memberid_p);
	user.property(UDi.memberid_p, test);

	data_.property(UDi.user_p, user);

	data_.property(UDi.roles_p, roles);

	wasRead_ = true;
	doWrite_ = true;
}



str_rc 
UserSession::sessionName()
{
	obj_rc user = getUser();
	return user.str_property(UDi.userName_p); 
}


void  
UserSession::setAdmin()
{
	htab_rc list;
	htab_rw roles(list);

	roles.push_back(UDi.Admin_str);
	roles.push_back(UDi.User_str);
	roles.push_back(UDi.Editor_str);	

	setValidUser(UDi.admin_user, roles);
}


void  
UserSession::setGuest()
{
	htab_rc list;
	htab_rw roles(list);

	roles.push_back(UDi.Guest_str);	

	setValidUser(UDi.Guest_str, roles);
}


void 
UserSession::setKey(str_ptr key, val_ptr value)
{
	zval* keys = data_.property_ptr(UDi.keys_p);
	htab_rw kdata(keys);
	kdata.set(key, value);
	doWrite_ = true;
}


void 
UserSession::setValidUser(str_ptr uname, htab_ptr roles)
{
	data_.property(UDi.keys_p, htab_ptr::empty_array());

	data_.property(UDi.roles_p, roles);

	obj_rc user = getUser();

	user.property(UDi.userName_p, uname);
	user.property(UDi.status_p, UDi.OK_str);
	user.property(UDi.email_p, str_ptr::empty_str());

	user.property(UDi.id_p, -1);
	user.property(UDi.memberid_p, (int)0 );
}





void 
UserSession::shutdown()
{
	write(true);
}


void UserSession::unsetKey(str_ptr key)
{
	val_ptr keys = data_.property_ptr(UDi.keys_p);
	htab_rw kdata(keys);
	if (kdata.has_key(key))
	{
		kdata.unset(key);
		doWrite_ = true;
	}
}


void 
UserSession::updated()
{
	obj_rc session = getSession();
	if (session.ok())
	{
		session.property(UDi.userData_p, data_);
	}
}


void UserSession::wipe()
{
	htab_ptr empty = htab_ptr::empty_array();

	data_.property(UDi.keys_p, empty);
	data_.property(UDi.roles_p, empty);

	obj_rc user = getUser();

	str_ptr estr = str_ptr::empty_str();

	user.property(UDi.userName_p, estr);
	user.property(UDi.status_p, estr);
	user.property(UDi.email_p, estr);

	user.property(UDi.id_p, (int)0);
	user.property(UDi.memberid_p, (int)0 );

	setGuest();
	doWrite_ = true;

}


void 
UserSession::UserSession::write(bool force)
{
	if (force || doWrite_)
	{
		doWrite_ = false;
		obj_rc session = getSession();
		if (session.ok())
		{
			adjustExpiry();
			session.property(UDi.userData_p, data_);
		}
	}
}


}// wcc namespace

using namespace zpp;
using namespace wcc;

ZEND_METHOD(Wcc_UserSession, __construct)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}

	UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
	cobj->construct();
}

ZEND_METHOD(Wcc_UserSession, activate)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
	obj_rc result = cobj->activate();
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_UserSession, addFlash)
{
	zarg_rd args(execute_data);

	str_ptr text = args.str(args.need(0));
	str_ptr status = args.str(args.need(1));
	if (!args.throw_errors(__FUNCTION__))
	{
		UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
		cobj->addFlash(text,status);
	}
}

ZEND_METHOD(Wcc_UserSession, addUserRoles)
{
	zarg_rd args(execute_data);

	htab_ptr roles = args.htab(args.need(0));
	if (!args.throw_errors(__FUNCTION__))
	{
		UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
		cobj->addUserRoles(roles);
	}
}

ZEND_METHOD(Wcc_UserSession, adjustExpiry)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
	int result = cobj->adjustExpiry();
	RETURN_LONG(result);
}

ZEND_METHOD(Wcc_UserSession, auth)
{
	zarg_rd args(execute_data);

	val_ptr role = args.string_or_array(args.need(0));

	if (!args.throw_errors(__FUNCTION__))
	{
		UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
		bool result = cobj->auth(role);
		RETURN_BOOL(result);
	}
}

ZEND_METHOD(Wcc_UserSession, clearFlash)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
	cobj->clearFlash();
}

ZEND_METHOD(Wcc_UserSession, delayWrite)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
	cobj->delayWrite();

}

ZEND_METHOD(Wcc_UserSession, flash)
{
	zarg_rd args(execute_data);
	str_ptr status;
	htab_ptr extra;

	str_ptr text = args.str(args.need(0));
	args.zstring_null(status, args.option(1));
	args.zarray_null(extra, args.option(2));

	if (!args.throw_errors(__FUNCTION__))
	{
		UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
		cobj->flash(text, status, extra);
	}
}

ZEND_METHOD(Wcc_UserSession, getEndTime)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
	str_rc result = cobj->getEndTime();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_UserSession, getFlash)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
	htab_rc result = cobj->getFlash();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_UserSession, getKey)
{
	zarg_rd args(execute_data);

	str_ptr key = args.str(args.need(0));
	val_ptr def = args.option(1);

	if (!args.throw_errors(__FUNCTION__))
	{
		UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
		val_rc result = cobj->getKey(key, def);
		result.move_zv(return_value);
	}
}

ZEND_METHOD(Wcc_UserSession, getUser)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
	obj_rc result = cobj->getUser();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_UserSession, getUserRoles)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
	htab_rc result = cobj->getUserRoles();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_UserSession, guestSession)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
	obj_rc result = cobj->guestSession();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_UserSession, hasKey)
{
	zarg_rd args(execute_data);

	str_ptr key = args.str(args.need(0));

	if (!args.throw_errors(__FUNCTION__))
	{
		UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
		bool result = cobj->hasKey(key);
		RETURN_BOOL(result);
	}

}

ZEND_METHOD(Wcc_UserSession, hasValues)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
	bool result = cobj->hasValues();
	RETURN_BOOL(result);

}

ZEND_METHOD(Wcc_UserSession, isEmpty)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
	bool result = cobj->isEmpty();
	RETURN_BOOL(result);

}

ZEND_METHOD(Wcc_UserSession, isEnded)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
	bool result = cobj->isEnded();
	RETURN_BOOL(result);
}

ZEND_METHOD(Wcc_UserSession, isLoggedIn)
{
	zarg_rd args(execute_data);

	val_ptr role = args.string_or_array(args.need(0));

	if (!args.throw_errors(__FUNCTION__))
	{
		UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
		bool result = cobj->isLoggedIn(role);
		RETURN_BOOL(result);
	}

}

ZEND_METHOD(Wcc_UserSession, nullify)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
	cobj->nullify();

}

ZEND_METHOD(Wcc_UserSession, read)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
	obj_rc result = cobj->read();
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_UserSession, roles)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
	str_rc result = cobj->roles();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_UserSession, save)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
	cobj->write();
}

ZEND_METHOD(Wcc_UserSession, saveUser)
{
	zarg_rd args(execute_data);

	obj_ptr user = args.obj(args.need(0));
	htab_ptr roles = args.htab(args.need(1));

	if (!args.throw_errors(__FUNCTION__))
	{
		UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
		cobj->saveUser(user, roles);
	}
}

ZEND_METHOD(Wcc_UserSession, sessionName)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
	str_rc result = cobj->sessionName();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_UserSession, setAdmin)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
	cobj->setAdmin();
}

ZEND_METHOD(Wcc_UserSession, setGuest)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
	cobj->setGuest();

}

ZEND_METHOD(Wcc_UserSession, setKey)
{
	zarg_rd args(execute_data);

	str_ptr key = args.str(args.need(0));
	val_ptr value = args.need(1);

	if (!args.throw_errors(__FUNCTION__))
	{
		UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
		cobj->setKey(key, value);
	}
}

ZEND_METHOD(Wcc_UserSession, setValidUser)
{
	zarg_rd args(execute_data);

	str_ptr name = args.str(args.need(0));
	htab_ptr roles = args.htab(args.need(1));

	if (!args.throw_errors(__FUNCTION__))
	{
		UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
		cobj->setValidUser(name, roles);
	}
}

ZEND_METHOD(Wcc_UserSession, shutdown)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
	cobj->write(true);
}

ZEND_METHOD(Wcc_UserSession, unsetKey)
{
	zarg_rd args(execute_data);

	str_ptr key = args.str(args.need(0));

	if (!args.throw_errors(__FUNCTION__))
	{
		UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
		cobj->unsetKey(key);
	}
}

ZEND_METHOD(Wcc_UserSession, updated)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
	cobj->updated();	
}

ZEND_METHOD(Wcc_UserSession, wipe)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
	cobj->wipe();	
}

ZEND_METHOD(Wcc_UserSession, write)
{
	zarg_rd args(execute_data);

	bool force = false; //default
	args.zbool(force, args.option(0));

	if (!args.throw_errors(__FUNCTION__))
	{
		UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
		cobj->write(force);
	}
}


PHP_MINIT_FUNCTION(Wcc_UserSession_reg)
{
	auto ce = register_class_Wcc_UserSession();
	
	UserSession::omg.classEntry(ce);

	return SUCCESS;
}

#endif