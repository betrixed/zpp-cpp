#ifndef WCC_SERVICES_H
#define WCC_SERVICES_H


#ifndef ZPP_BASE_H
#include "base.h"
#endif


namespace wcc {

class  Services : public base_d {
protected:
	htab_rc  active_;
	htab_rc  defer_;
	htab_rc  instances_;

	int       defer_ct_;
	bool      throw_fail_;

	val_rc  activate(str_ptr key);
	val_rc  call_value(obj_ptr callme);

public:

	static base_obj_mgr<Services> omg;

	Services();
	virtual ~Services();
	// instance
	static obj_ptr  instance();
	static Services* cpp_global();

	static obj_ptr  setOne(str_ptr key, obj_ptr obj);
	static obj_ptr  getOne(str_ptr key);

	static val_rc  service(str_ptr key);
	static val_rc  service(const std::string_view& key);	

	obj_ptr  newInstance(str_ptr key);
	
	obj_ptr  setObject(obj_ptr obj, str_ptr key);

	/* return cached object */
	obj_ptr  getObject(str_ptr key);

	/* service functions */

	bool       isActive(str_ptr name);
	bool       has(str_ptr name);

	void       setDefer(str_ptr name, val_ptr value);
	void       set(str_ptr name, val_ptr value);
	val_rc   get(str_ptr name);

	void       unset(str_ptr name);

	void       setThrowFail(bool value);

	virtual void debug_info(htab_wr ht);

	void       clearActive();
	void       clearDefer();
	void  	   clearObjects();
	
	VIRTUAL_ZOBJPTR


}; 


};//namespace
#endif
//wc_services.h