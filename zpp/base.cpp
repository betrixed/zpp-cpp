#ifndef WC_BASE_CPP
#define WC_BASE_CPP



#include "base.h"

#ifdef ZPP_BUILD_ALL
#include "str.cpp"

#include "obj.cpp"

#include "val_rc.cpp"
#include "val_ptr.cpp"

#include "htab.cpp"

#include "for_key_value.cpp"
#include "state_init.cpp"
#include "class_data.cpp"

#include "datetime.cpp"

#include "fn_call.cpp"
#include "preg.cpp"

#include "zarg_rd.cpp"
#else

#include "htab_rw.h"

#endif

namespace zpp {

	class base_init : public state_init {
	public:
		base_init() : state_init() {}

		zstr_intern class_name;

		virtual void init()
		{
			class_name = "class";
		}
	};

base_init BI_str;

	 void //virtual
	 base_d::debug_info(htab_rw di)
	 {
	 	obj_ptr temp(this->vobj());

	 	htab_rc plist;


	 	di.set(BI_str.class_name, str_ptr(temp.className()));

	 	if (temp.property_list(plist))
	 	{
	 		//showdata("prop class:", plist);

	 		htab_rd src(plist);

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
