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
/*
#ifndef WCC_SESSION_ISESSION_H
#include "isession.h"
#endif
*/

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

ISession* 
UserSession::is_cpp()
{
	return zobj_toc<ISession>(session_);
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
	obj_rc data = getUserData();

	val_ptr rlist = data.property_ptr(UDi.roles_p);

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

	if (data_.ok())
	{
		UserData* ud = ud_cpp();

		val_return r_obj = Services::service(UDi.config_str);
		if (r_obj.has_errors() || !r_obj.value_.isObject())
		{
			return 0; // fail silently
		}
		obj_rc gConfig = r_obj.value_.zobject();

		htab_rc ttl_roles = gConfig.array_property(UDi.TTLroles);

		int ttl_time = gConfig.int_property(UDi.TTLtime);

		
		if (ud->hasAnyRole(ttl_roles))
		{

			obj_rc adapter = getAdapter();
			if (adapter.ok())
			{
				val_rc tsec(ttl_time);

				val_rc test = adapter.call(UDi.gc_str, tsec);

				result = test.zlong();
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

	return ud->isLoggedIn(role);
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


/*
htab_rc
UserSession::getFlash()
{
	return data_.array_property(UDi.flash_str);
}
*/

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

bool 
UserSession::hasKey(str_ptr key)
{
	zval* keys = data_.property_ptr(UDi.keys_p);
	htab_ptr values(keys);
	return values.has_key(key);
}

void 
UserSession::setKey(str_ptr key, val_ptr value)
{
	zval* keys = data_.property_ptr(UDi.keys_p);
	htab_rw kdata(keys);
	kdata.set(key, value);
	doWrite_ = true;
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

obj_return
UserSession::getSession()
{
	obj_return result;

	if (ended_)
	{
		return result;
	}
	if (!session_.ok())
	{
		val_return test = Services::service(UDi.session_str);
		if (!test.has_errors() && test.value_.isObject())
		{
			session_ = test.value_.zobject();
		}
		else {
			result = test.move_error();
		}
	}
	result.value_ = session_;
	return result;
}

obj_rc 
UserSession::getUserData()
{
	obj_rc result;
	if (!wasRead_)
	{
		read();
	}
	// ensure data has a user object
	obj_rc user = data_.obj_property(UDi.user_p);
	if (!user.ok())
	{
		user = ud_cpp()->getUser();
		doWrite_ = true; 
	}
	return data_;
}

int
UserSession::getUserId()
{
	int result = 0;

	if (wasRead_)
	{
		obj_rc user = data_.obj_property(UDi.user_p);
		if (user.ok())
		{
			result = user.int_property(UDi.id_p);
		}
	}

	return result;
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
UserSession::isEnded()
{
	return ended_;
}

bool
UserSession::isEmpty()
{
	if (!data_.ok())
	{
		return false;
	}
	obj_rc user = data_.obj_property(UDi.user_p);

	htab_rc roles = data_.array_property(UDi.roles_p);

	return (!user.ok() || (roles.size()==0));
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
	obj_rc result;

	bool exists = false;

	if (!wasRead_ && !ended_)
	{
		doWrite_ = false;
		obj_return sret = getSession();
		if (sret.has_errors())
		{
			// post/hrow exception?
			return result;
		}
		ISession* sesp = is_cpp();

		data_.init();
		exists = sesp->start();

		//test = session.call(UDi.start_fn);
		
		if (exists)
		{
			/*if (log) {
				
				log->line("Session read");
			}*/

			val_rc ud = sesp->get(UDi.userData_p);
			val_ptr test(ud);

			//val_rc arg1(UDi.userData_p);
			//ud = session_.call(UDi.get_str, arg1);

			/*if (log) {

				log->dump("UserData object:", test);
			}*/

			if (test.isObject())
			{
				data_ = test.zobject();
				
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
				
				sesp->set(UDi.userData_p, data_);
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
UserSession::setValidUser(str_ptr uname, htab_ptr roles)
{
	obj_rc user = ud_cpp()->getUser();
	
	data_.property(UDi.keys_p, htab_ptr::empty_array());
	data_.property(UDi.roles_p, roles);

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





void 
UserSession::updated()
{
	obj_return sret = getSession();
	if (sret.has_errors())
	{
		return;//TODO: when to worry?
	}
	// session_ should have been set
	if (session_.ok())
	{
		ISession* sesp = is_cpp();
		sesp->set(UDi.userData_p, data_);

		//session.property(UDi.userData_p, data_);
	}
}


void UserSession::wipe()
{
	htab_ptr empty = htab_ptr::empty_array();

	data_.property(UDi.keys_p, empty);
	data_.property(UDi.roles_p, empty);
	obj_rc user = data_.obj_property(UDi.user_p);

	data_.property(UDi.user_p, val_rc::null_value_ptr());

	setGuest();
	doWrite_ = true;

}


void 
UserSession::UserSession::write(bool force)
{
	if (force || doWrite_)
	{
		doWrite_ = false;
		obj_return session = getSession();
		if (session.has_errors())
		{
			return; //TODO: push error up.
		}
		if (session_.ok())
		{
			adjustExpiry();
			ISession* sesp = is_cpp();
			sesp->set(UDi.userData_p, data_);
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


ZEND_METHOD(Wcc_UserSession, getSession)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
	obj_return result = cobj->getSession();

	if (!result.throw_errors())
	{
		result.value_.move_zv(return_value);
	}
	
}



ZEND_METHOD(Wcc_UserSession, getUserData)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
	obj_rc result = cobj->getUserData();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_UserSession, getUserId)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	UserSession* cobj = zval_toc<UserSession>(ZEND_THIS);
	RETURN_LONG(cobj->getUserId());
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