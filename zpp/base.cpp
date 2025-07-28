#ifndef WC_BASE_CPP
#define WC_BASE_CPP



#include "base.h"

#ifdef ZPP_BUILD_ALL
#include "zstr.cpp"

#include "zobj.cpp"

#include "zval_mgr.cpp"
#include "zval_user.cpp"

#include "htab.cpp"

#include "for_key_value.cpp"
#include "state_init.cpp"
#include "class_data.cpp"

#include "datetime.cpp"

#include "fn_call.cpp"
#include "preg.cpp"

#include "zarg_exec.cpp"
#else

#include "htab_write.h"

#endif

namespace zpp {

	 void //virtual
	 base_d::debug_info(htab_write di)
	 {
	 	zobj_user temp(this->vobj());

	 	htab_mgr plist;

	 	if (temp.property_list(plist))
	 	{
	 		htab_read src(plist);

		 	if (src.size())
		 	{
		 		di.merge(src);
		 	}
		 }
	 }
	 
	 zstr_user //virtual
	 base_d::extender()
	 {
	 	return zend_empty_string;
	 }

	 mgr_link* mgr_link::l_start_ = nullptr;
	 mgr_link* mgr_link::l_end_ = nullptr;

	

};//namespace

#endif
