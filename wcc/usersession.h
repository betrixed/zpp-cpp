#ifndef WCC_USERSESSION_H
#define WCC_USERSESSION_H

#ifndef SESSION_USERDATA_H
#include "userdata.h"
#endif


#ifndef WCC_SESSION_ISESSION_H
#include "isession.h"
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
	
	
	obj_rc getAdapter();

	UserData* ud_cpp();
	ISession* is_cpp();

public:

	static base_obj_mgr<UserSession> omg;
	
	void debug_info(htab_rw hw) override;

	void construct();
	obj_rc  activate();
	void addFlash(str_ptr text, str_ptr status);
	void addUserRoles(htab_ptr roles);
	int  adjustExpiry();

	bool auth(val_ptr role);
	void clearFlash();
	void delayWrite();

	void flash(str_ptr msg, str_ptr status, htab_ptr exlines);
	
	obj_rc getSession();
	obj_rc getUserData();
	obj_rc guestSession();

	val_rc getKey(str_ptr key, val_ptr adefault);
	bool hasKey(str_ptr);
	void setKey(str_ptr key, val_ptr value);	
	void unsetKey(str_ptr key);
	
	bool hasValues();
	bool isEmpty();
	bool isEnded();
	bool isLoggedIn(val_ptr roles);
	
	void nullify();
	obj_rc read();
	str_rc roles();
	void   save();

	void  setAdmin();
	void  setGuest();
	
	void setValidUser(str_ptr uname, htab_ptr roles);

	void shutdown();
	
	void updated();
	void wipe();
	void write(bool force=false);

};

}//wcc namespace

#endif