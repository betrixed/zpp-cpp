#ifndef WCC_USERSESSION_H
#define WCC_USERSESSION_H

#ifndef SESSION_FLASH_H
#include "flash.h"
#endif

#ifndef SESSION_USERDATA_H
#include "userdata.h"
#endif

namespace wcc {

using namespace zpp;

class  UserSession : public base_d {
protected:
	bool doWrite_;
	bool wasRead_;
	bool ended_;
	obj_rc session_;
	obj_rc data_;
	
	obj_rc getSession();
	UserData* ud_cpp();

public:

	static base_obj_mgr<UserSession> omg;
	
	void construct();
	obj_rc  activate();
	void addFlash(str_ptr text, str_ptr status);
	void addUserRoles(htab_ptr roles);
	int  adjustExpiry();

	bool auth(val_ptr role);
	void clearFlash();
	void flash(str_ptr msg, htab_ptr exlines, str_ptr status);
	str_rc getEndTime();
	obj_rc getFlash();
	val_rc getKey(str_ptr key, val_ptr adefault);
	obj_rc getUser();

	htab_rc getUserRoles();

	obj_rc guestSession();

	bool hasKey(str_ptr);
	bool hasValues();
	bool isEmpty();
	bool isEnded();
	bool isLoggedIn(val_ptr roles);
	void nullify();
	obj_rc read();
	str_rc roles();
	void   save();
	void saveUser(val_ptr user, htab_ptr roles);

	str_rc sessionName();
	void  setAdmin();
	void  setGuest();
	void setKey(str_ptr key, val_ptr value);
	void setValidUser(str_ptr uname, htab_ptr roles);
	
	

	void shutdown();
	void unsetKey(str_ptr key);
	void updated();
	void wipe();
	void write(bool force=false);







};

}//wcc namespace
#endif