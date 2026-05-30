#ifndef WCC_USERSESSION_CPP
#define WCC_USERSESSION_CPP

#ifndef WCC_USERSESSION_H
#include "usersession.h"
#endif

namespace wcc {

using namespace zpp;

base_obj_mgr<UserSession> UserSession::omg;


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
	wasread_ = false;
	ended_ = false;
	//session_ = null;
	data_ = UserData::omg.new_zobj();

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

	zval* roles = data_.property_ptr(UDi.roles_p);
	htab_rw myroles(roles);

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
		return ud->hasRole(role.zstring());
	}
	if (role.isArray())
	{
		return ud->hasAnyRole(role.zarray());
	}
	return false;
}


void UserSession::clearFlash()
{
	zval* fptr = data_.property_ptr(UDi.flash_str);
	htab_hw flash(fptr);
	flash.clear();
}

void UserSession::delayWrite()
{
	doWrite_ = true;
}

void UserSession::flash(str_ptr msg, htab_ptr exlines, str_ptr status)
{
	str_buf buf;

	buf << msg;

	if (exlines.size())
	{
		htab_walk wk;
		auto line = wk.value();
		for(wk.start(exlines); wk.ok(); wk.next())
		{
			buf << "<br>\n" << line.zstring();
		}
	}
	zval* fptr = data_.property_ptr(UDi.flash_str);
	htab_rw list(fptr);
	htab_rc line;
	htab_rw flash(line);
	flash.push_back(buf.zstr());
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
				result = datetime_obj::date(UDi.expire_fmt, expires.zlong());
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
		result = keys.get(key);
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
	if (!wasRead_)
	{
		read();
	}
	return data_.obj_property(UDi.user_p);
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
	return values.has_key(key)
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
	return (!user.ok() || 
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
	if (session_status() === PHP_SESSION_ACTIVE)
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

	if (!wasRead_ && !ended_)
	{
		doWrite_ = false;
		objrc session = getSession();
		if (session.ok())
		{
			data_.init();
			test = session.call(UDi.start_fn);
			exists = test.isTrue();
		}
		if (exists)
		{
			val_ptr ud = session_.property_ptr(UDi.userData_p);
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
				data_ = UserData::omg.new_zobj();
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
	
	zval* pr = data_.property_ptr(UDi.roles_p);
	if (pr.isArray())
	{
		htab_walk wk;
		auto value = wk.value();
		int ct = 0;
		for(wk.start(); wk.ok(); ct++, wk.next())
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
	write();
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
	write();
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
UserSession::UserSession::write(bool force=false)
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




} // wcc namespace
#endif