#ifndef WCC_USERSESSION_CPP
#define WCC_USERSESSION_CPP

#ifndef WCC_USERSESSION_H
#include "usersession.h"
#endif

namespace wcc {

using namespace zpp;

base_obj_mgr<UserSession> UserSession::omg;

void UserSession::construct()
{
	
}

obj_rc  UserSession::activate()
{

}


void UserSession::addFlash(str_ptr text, str_ptr status)
{

}


void UserSession::addUserRoles(htab_ptr roles)
{

}


int  UserSession::adjustExpiry()
{

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


obj_rc UserSession::getFlash()
{

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