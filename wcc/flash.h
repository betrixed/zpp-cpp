#ifndef SESSION_FLASH_H
#define SESSION_FLASH_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

namespace wcc {


class UserDataInit : public state_init {
public:
	void init() override;
	str_intern lines_str;
	str_intern text_str;
	str_intern status_str;

	str_intern userName_p;
	str_intern roles_p;
	str_intern email_p;
	str_intern id_p;
	str_intern memberid_p;
	str_intern status_p;
	str_intern keys_p;

	str_intern guest_str;
	str_intern flash_str;
	str_intern OK_str;
};

extern UserDataInit UDi;

	class Flash : public base_d {
	public:

		static base_obj_mgr<Flash> omg;
		void construct(htab_ptr data);

		bool hasData();

		void add(str_ptr text, str_ptr status);

		htab_rc getData();

		void clear();

	};
};


#endif 