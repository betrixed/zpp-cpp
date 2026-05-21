#ifndef SESSION_USERDATA_H
#define SESSION_USERDATA_H


#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif


namespace wcc {

using namespace zpp;

class  UserData : public base_d {
	val_ptr  userName_v;
	val_ptr  roles_v;
	val_ptr  email_v;
	val_ptr  id_v;
	val_ptr  memberid_v;
	val_ptr  status_v;
	val_ptr  keys_v;

public:
	void construct();

	obj_rc addFlash(str_ptr msg, str_ptr status);
	void   auth(str_ptr role);
	val_rc getKey(str_ptr key, val_ptr defval);
	bool   hasAnyRole(htab_ptr rolelist);
	bool   hasRole(str_ptr role);
	bool   hasUser();
	void   init();
	bool   isGuest();
	void   setGuest();
	void   setKey(str_ptr key, mixed value);
	void   setValidUser(str_ptr name, htab_ptr roles);
	void   unsetKey(str_ptr key);
	void   wipekeys();
};

}//namespace wcc
#endif