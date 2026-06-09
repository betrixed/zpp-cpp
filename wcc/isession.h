#ifndef WCC_SESSION_ISESSION_H
#define WCC_SESSION_ISESSION_H


#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

namespace wcc {

using namespace zpp;

class  SessInit : public state_init {
public:
	str_intern  global_sess;
	str_intern  getexpires_fn;
	str_intern  expires_fmt;
	str_intern  unknown_str;

	str_intern  options_str;
	str_intern  adapter_str;
	str_intern  prefix_str;
	str_intern  name_str;

	str_intern  savePath_str;
	

	void init() override;
};

extern SessInit SIN;

class ISession;

class ISession_mgr : public base_obj_mgr<ISession>
{
public:

	static zval* read_property(zend_object* object, zend_string* name, int type, void** cache_slot, zval* rv);
	static zval* write_property(zend_object* object, zend_string* name, zval *value, void** cache_slot);
	static int   has_property(zend_object* object, zend_string* name, int has_set_exists, void** cache_slot);
	static void  unset_property(zend_object* object, zend_string* name, void **cache_slot);

	static void  setup_handlers(zend_object_handlers& hand);

	void init_class_fn() override;

};

class  ISession : public base_d {
protected:
	obj_rc adapter_;

	htab_rc options_;

	str_rc name_;

	str_rc prefix_;

	

public:
	static ISession_mgr omg;

		// Call global PHP functions
	static str_rc id(str_ptr id = str_ptr());

	static str_rc name(str_ptr name = str_ptr());

	static int status();

	static bool exists();

	void debug_info(htab_rw) override;

	val_rc __get(str_ptr key);
	val_rc __isset(str_ptr key);
	void   __set(str_ptr key, val_ptr value);
	void   __unset(str_ptr key);

	void construct(htab_ptr options);

	void destroy();

	val_rc get(str_ptr key, val_ptr defval = val_rc::null_value_ptr(), bool remove=false);

	obj_rc getAdapter();

	str_rc getEndTime();

	str_rc getUniqueKey(str_ptr key);

	htab_rc getOptions();

	bool has(str_ptr key);

	bool regenerateId(bool deleteOld = false);

	void set(str_ptr key, val_ptr value);

	void setAdapter(obj_ptr itf);

	void setOptions(htab_ptr opt);
	
	bool start();

	void remove(str_ptr key);
};

}//wcc namespace


#endif