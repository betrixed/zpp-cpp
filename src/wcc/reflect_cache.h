#ifndef REFLECT_CACHE_H
#define REFLECT_CACHE_H

#ifndef ZPP_BASE_H
#include "base.h"
#endif


namespace wcc {

using namespace zpp;

class  ReflectCache : public base_d {
protected:
	htab_rc  cache_;
public:

	static const char* class_name;

	static base_obj_mgr<ReflectCache> omg;

	static obj_ptr instance();

	static ReflectCache* cpp();

	static obj_rc staticInstance(str_ptr class_name);

	static obj_rc staticInstanceArgs(str_ptr class_name, htab_ptr args);
	
	ReflectCache();

	obj_rc getReflectClass(str_ptr class_name);

	obj_rc newInstance(str_ptr class_name);

	obj_rc newInstanceArgs(str_ptr class_name, htab_ptr args);

	void clear();
	
	virtual void debug_info(htab_rw hw);
	
	VIRTUAL_ZOBJPTR
	
};


}; // namespace


#endif
//reflect_cache.h