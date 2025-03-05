#ifndef REFLECT_CACHE_H
#define REFLECT_CACHE_H

#ifndef ZPP_BASE_H
#include "base.h"
#endif


namespace wcc {

using namespace zpp;

class  ReflectCache : public base_d {
protected:
	htab_mgr  cache_;
public:

	static const char* class_name;

	static base_obj_mgr<ReflectCache> omg;

	static zobj_mgr instance();

	static ReflectCache* cpp();

	static zobj_mgr staticInstance(zstr_user class_name);

	static zobj_mgr staticInstanceArgs(zstr_user class_name, zval_user args);
	
	ReflectCache();

	zobj_mgr getReflectClass(zstr_user class_name);

	zobj_mgr newInstance(zstr_user class_name);

	zobj_mgr newInstanceArgs(zstr_user class_name, zval_user args);

	void clear();
	
	void debug_info(htab_write hw) override;
};


}; // namespace


#endif
//reflect_cache.h