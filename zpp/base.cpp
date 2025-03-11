#ifndef WC_BASE_CPP
#define WC_BASE_CPP

#include "base.h"

#include "zstr_mgr.cpp"
#include "zstr_user.cpp"

#include "zobj_mgr.cpp"
#include "zobj_user.cpp"

#include "zval_mgr.cpp"
#include "zval_user.cpp"

#include "htab_mgr.cpp"
#include "htab_read.cpp"
#include "htab_write.cpp"
#include "htab_walk.cpp"
#include "for_key_value.cpp"
#include "state_init.cpp"

#include "datetime.cpp"
#include "globals.cpp"
#include "global.cpp"
#include "zstr_buffer.cpp"
#include "fn_call.cpp"
#include "preg.cpp"

namespace zpp {

	 void //virtual
	 base_d::debug_info(htab_write di)
	 {
	 	zobj_user temp(this->zobj());

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
	 
	 zend_string* //virtual
	 base_d::extender()
	 {
	 	return zend_empty_string;
	 }

	 mgr_link* mgr_link::l_start_ = nullptr;
	 mgr_link* mgr_link::l_end_ = nullptr;


	zval_mgr::zval_mgr(base_d* cobj)
	{
		ZVAL_OBJ(&zv_, cobj->zobj());
	}
};//namespace

#endif
