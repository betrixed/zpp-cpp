#ifndef SESSION_USERDATA_H
#define SESSION_USERDATA_H


#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif


namespace wcc {

using namespace zpp;



class UserDataInit : public state_init {
public:
	void init() override;
	str_intern lines_str;
	str_intern text_str;
	str_intern status_str;

	str_intern user_p;
	str_intern name_p;
	str_intern userName_p;
	str_intern roles_p;
	str_intern email_p;

	str_intern id_p;
	str_intern memberid_p;
	str_intern status_p;
	str_intern keys_p;

	str_intern flash_str;
	str_intern OK_str;
	str_intern destroy_fn;
	str_intern start_fn;
	str_intern userData_p;

	str_intern status_info;


	str_intern admin_user;
	str_intern Admin_str;
	str_intern Editor_str;
	str_intern Guest_str;
	str_intern User_str;

	str_intern getadapter_str;
	str_intern gc_str;
	str_intern session_str;

	str_intern getexpires_str;
	str_intern expires_fmt;
	str_intern unknown_str;

	str_intern doWrite;
	str_intern wasRead;



};

extern UserDataInit UDi;


class  UserData : public base_d {
protected:
	void init();
public:


	static base_obj_mgr<UserData> omg;
	
	static obj_rc newobj();

	void construct();

	bool   hasAnyRole(htab_ptr rolelist);
	bool   hasRole(str_ptr role);
	bool   hasUser();

	//void __unserialize(htab_ptr htab);
};

}//namespace wcc
#endif