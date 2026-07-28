#ifndef DS_BASE_CPP
#define DS_BASE_CPP
//dsbase.cpp

#ifndef DS_BASE_H
#include "dsbase.h"
#endif

namespace dso {

	 void //virtual
	 ds_base::debug_info(htab_user di)
	 {
	 	zobj_user temp(this->zobj());

	 	htab_rc plist;

	 	if (temp.property_list(plist))
	 	{
	 		htab_user src(plist);

		 	if (src.size())
		 	{
		 		di.merge(src);
		 	}
		 }
	 }
	 
	 zend_string* //virtual
	 ds_base::extender()
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