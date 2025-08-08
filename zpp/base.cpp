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

#include "htab_wr.h"

#endif

namespace zpp {

	 void //virtual
	 base_d::debug_info(htab_wr di)
	 {
	 	obj_ptr temp(this->vobj());

	 	htab_rc plist;

	 	if (temp.property_list(plist))
	 	{
	 		htab_rd src(plist);

		 	if (src.size())
		 	{
		 		di.merge(src);
		 	}
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
