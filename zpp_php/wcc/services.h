#ifndef WCC_SERVICES_H
#define WCC_SERVICES_H


#ifndef ZPP_BASE_H
#include "base.h"
#endif


namespace wcc {

	using namespace zpp;

class  Services : public base_d {
protected:
	htab_rc  active_;
	htab_rc  defer_;
	htab_rc  instances_;

	int       defer_ct_;
	bool      throw_fail_;

	val_return  activate(str_ptr key);
	val_rc      call_value(obj_ptr callme);

public:

	static base_obj_mgr<Services> omg;

	Services();
	virtual ~Services();
	// instance
	static obj_rc  instance();
	static Services* cpp_global();

	static obj_rc  	   setOne(str_ptr key, obj_ptr obj);
	static obj_return  getOne(str_ptr key, htab_ptr arglist=htab_ptr());
	static obj_return  makeOne(str_ptr cname, htab_ptr arglist=htab_ptr());

	static val_return  service(str_ptr key);
	static val_return  service(const std::string_view& key);	

	obj_return  newInstance(str_ptr key, htab_ptr arglist = htab_ptr());
	
	obj_rc  setObject(obj_ptr obj, str_ptr key = str_ptr());

	/* return cached object */
	obj_rc    getObject(str_ptr key);

	/* service functions */

	bool       isActive(str_ptr name);
	bool       has(str_ptr name);

	void       setDefer(str_ptr name, val_ptr value);
	void       set(str_ptr name, val_ptr value);
	void       set(str_ptr name, obj_ptr obj);
	void       set(str_ptr name, val_rc& val);
	
	
	val_return get(str_ptr name);

	void       unset(str_ptr name);

	void       setThrowFail(bool value);

	virtual void debug_info(htab_rw ht);

	void       clearActive();
	void       clearDefer();
	void  	   clearObjects();
	
	VIRTUAL_ZOBJPTR


}; 


};//namespace
#endif
//wc_services.h