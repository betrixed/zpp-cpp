#ifndef SESSION_USERDATA_CPP
#define SESSION_USERDATA_CPP

#ifndef SESSION_FLASH_H
#include "flash.h"
#endif

#ifndef SESSION_USERDATA_H
#include "userdata.h"
#endif

namespace wcc {


void 
UserData::construct()
{
	obj_ptr self(self_);

	userName_v = self.property_ptr(UDi.userName_p);
	roles_v = self.property_ptr(UDi.roles_p);
	email_v = self.property_ptr(UDi.email_p);
	id_v = self.property_ptr(UDi.id_p);
	memberid_v = self.property_ptr(UDi.memberid_p);
	status_v = self.property_ptr(UDi.status_p);
	keys_v = self.property_ptr(UDi.keys_p);
}

obj_rc 
UserData::addFlash(str_ptr msg, str_ptr status)
{

}

void   
UserData::auth(str_ptr role)
{

}

val_rc 
UserData::getKey(str_ptr key, val_ptr defval)
{

}

bool   
UserData::hasAnyRole(htab_ptr rolelist)
{

}

bool   
UserData::hasRole(str_ptr role)
{

}

bool   
UserData::hasUser()
{

}

void   
UserData::init()
{

}

bool   
UserData::isGuest()
{

}

void   
UserData::setGuest()
{

}

void   
UserData::setKey(str_ptr key, mixed value)
{

}

void   
UserData::setValidUser(str_ptr name, htab_ptr roles)
{

}

void   
UserData::unsetKey(str_ptr key)
{

}

void   
UserData::wipekeys()
{

}

};

#endif
