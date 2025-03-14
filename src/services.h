#ifndef WCC_SERVICES_H
#define WCC_SERVICES_H


#ifndef ZPP_BASE_H
#include "base.h"
#endif


namespace wcc {

class  Services : public base_d {
protected:
	htab_mgr  active_;
	htab_mgr  defer_;
	htab_mgr  instances_;

	int       defer_ct_;
	bool      throw_fail_;

	zval_mgr  activate(zstr_user key);
	zval_mgr  call_value(zobj_user callme);

public:

	static base_obj_mgr<Services> omg;

	Services();
	virtual ~Services();
	// instance
	static zobj_user  instance();
	static Services* cpp_global();

	static zobj_user  setOne(zstr_user key, zobj_user obj);
	static zobj_user  getOne(zstr_user key);

	static zval_mgr  service(zstr_user key);
	static zval_mgr  service(const std::string_view& key);	

	zobj_user  newInstance(zstr_user key);
	
	zobj_user  setObject(zobj_user obj, zstr_user key);

	/* return cached object */
	zobj_user  getObject(zstr_user key);

	/* service functions */

	bool       isActive(zstr_user name);
	bool       has(zstr_user name);

	void       setDefer(zstr_user name, zval_user value);
	void       set(zstr_user name, zval_user value);
	zval_mgr   get(zstr_user name);

	void       unset(zstr_user name);

	void       setThrowFail(bool value);

	void debug_info(htab_write ht) override;

	void       clearActive();
	void       clearDefer();
	void  	   clearObjects();
	

}; 


};//namespace
#endif
//wc_services.h