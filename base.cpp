#ifndef WC_BASE_CPP
#define WC_BASE_CPP

/**
 * @file zpp/base.cpp
 * @brief base_d is base class for PHP class objects. based_obj_mgr is template for class specific data.
 * @author Michael Rynn <michael.rynn.500@gmail.com>
 * @date 2025
 * 
 */ 


#include "base.h"

#ifdef ZPP_BUILD_ALL


#include "phpalloc.cpp"

#include "str.cpp"

#include "obj.cpp"

#include "val_rc.cpp"
#include "val_ptr.cpp"
#include "ref_rc.cpp"

#include "htab.cpp"

#include "for_key_value.cpp"
#include "state_init.cpp"
#include "class_data.cpp"

#include "datetime.cpp"

#include "fn_call.cpp"
#include "preg.cpp"

#include "bireturn.cpp"

#else

#include "htab_rw.h"

#endif

namespace zpp {

class base_init : public state_init {
public:

	str_intern class_name;

	void init() override
	{
		class_name = "class";
	}
};

base_init BI_str;

	 void //virtual
	 base_d::debug_info(htab_rw di)
	 {
	 	obj_ptr temp(self_);

	 	htab_rc plist;

	 	//di.set(BI_str.class_name, str_ptr(temp.className()));

	 	if (temp.property_list(plist))
	 	{
	 		//showdata("prop class:", plist);

	 		htab_ptr src(plist);


		 	di.merge(src);

		 	//showdata("properties:", di);
		 }
	 }
	 
	 str_ptr //virtual
	 base_d::extender()
	 {
	 	return zend_empty_string;
	 }

	 mgr_link* mgr_link::l_start_ = nullptr;
	 mgr_link* mgr_link::l_end_ = nullptr;
	



};//namespace

#endif
