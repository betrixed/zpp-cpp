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
				
			}
		}
	}
}



bool UserSession::auth(val_ptr role)
{

}


void UserSession::clearFlash()
{

}


void UserSession::flash(str_ptr msg, htab_ptr exlines, str_ptr status)
{

}


str_rc UserSession::getEndTime()
{

}

htab_rc
UserSession::getFlash()
{
	return data_.array_property(UDi.flash_str);
}


val_rc UserSession::getKey(str_ptr key, val_ptr adefault)
{

}


obj_rc UserSession::getUser()
{

}



htab_rc UserSession::getUserRoles()
{

}



obj_rc UserSession::guestSession()
{

}



bool UserSession::hasKey(str_ptr)
{

}


bool UserSession::hasValues()
{

}


bool UserSession::isEmpty()
{

}


bool UserSession::isEnded()
{

}


bool UserSession::isLoggedIn(val_ptr roles)
{

}


void UserSession::nullify()
{

}


obj_rc UserSession::read()
{

}


str_rc UserSession::roles()
{

}


void UserSession::save()
{

}


void UserSession::saveUser(val_ptr user, htab_ptr roles)
{

}



str_rc UserSession::sessionName()
{

}


void  UserSession::setAdmin()
{

}


void  UserSession::setGuest()
{

}


void UserSession::setKey(str_ptr key, val_ptr value)
{

}


void UserSession::setValidUser(str_ptr uname, htab_ptr roles)
{

}





void UserSession::shutdown()
{

}


void UserSession::unsetKey(str_ptr key)
{

}


void UserSession::updated()
{

}


void UserSession::wipe()
{

}


void UserSession::UserSession::write(bool force=false)
{

}

{

}



} // wcc namespace
#endif