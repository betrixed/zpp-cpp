#ifndef WCC_SESSION_ISESSION_CPP
#define WCC_SESSION_ISESSION_CPP

#ifndef WCC_SESSION_ISESSION_H
#include "isession.h"
#endif

#ifndef WCC_DEBUG_LOG_H
#include "debuglog.h"
#endif

#ifndef  PHP_SESSION_H
extern "C" {
	#include <ext/session/php_session.h>
};
#endif

#ifndef WCC_ISESSION_ARGINFO_H
#define WCC_ISESSION_ARGINFO_H
extern "C" {
	#include "stub/isession_arginfo.h"
}
#endif

namespace wcc {

using namespace zpp;
/*
class  ISession : public base_d {
protected:
	obj_rc adapter_;

	htab_rc options_;

	str_rc name_;

	str_rc uniqueId_;
*/

SessInit SIN;
ISession_mgr ISession::omg;

void 
SessInit::init()
{
	global_sess = "_SESSION";
	getexpires_fn = "getexpires";
	expires_fmt = "D H:i e";
	unknown_str = "unknown";

	options_str = "options";
	adapter_str = "adapter";
	prefix_str = "prefix";
	name_str = "name";

	savePath_str = "savePath";
}


void //static
ISession_mgr::setup_handlers(zend_object_handlers& hand)
{
	hand.read_property = ISession_mgr::read_property;
	hand.write_property = ISession_mgr::write_property;
	hand.has_property = ISession_mgr::has_property;
	hand.unset_property = ISession_mgr::unset_property;
}

void ISession_mgr::init_class_fn()
{
	mydef::init_class_fn();

	//setup_handlers(mydef::handlers_);
}

zval* 	  
ISession_mgr::read_property(zend_object* object, zend_string* name, int type, void** cache_slot, zval* rv)
{
	*cache_slot = nullptr;
	ISession* iobj = zobj_toc<ISession>(object);
	val_rc  result = iobj->get(name);
	result.move_zv(rv); 
	return rv;
}

zval* 	  
ISession_mgr::write_property(zend_object* object, zend_string* name, zval *value, void** cache_slot)
{
	*cache_slot = nullptr;
	ISession* iobj = zobj_toc<ISession>(object);
	iobj->set(name, val_ptr(value));
	return value;
}

int 
ISession_mgr::has_property(zend_object* object, zend_string* name, int has_set_exists, void** cache_slot)
{
	*cache_slot = nullptr;
	ISession* iobj = zobj_toc<ISession>(object);
	return iobj->has(name);
}

void
ISession_mgr::unset_property(zend_object* object, zend_string* name, void **cache_slot)
{
	*cache_slot = nullptr;
	ISession* iobj = zobj_toc<ISession>(object);
	return iobj->remove(name);
}

void ISession::debug_info(htab_rw di)
{
	di.set(SIN.options_str, options_);
	di.set(SIN.adapter_str, adapter_);
	di.set(SIN.name_str, name_);
	di.set(SIN.prefix_str, prefix_);
}

void 
ISession::construct(htab_ptr options)
{
	options_ = options;
}

void 
ISession::destroy()
{
	if (ISession::exists())
	{
		php_session_destroy();
		htab_ptr::set_global(SIN.global_sess, val_rc::empty_array_ptr());
	}
	/*
	*/
}

int
ISession::status()
{
	return php_get_session_status();
}

bool
ISession::exists()
{
	return (php_get_session_status() == php_session_status::php_session_active);
}

val_rc 
ISession::get(str_ptr key, val_ptr defval, bool remove)
{
	val_rc result;

	if (!ISession::exists())
	{
		return result;
	}
	str_rc ukey = getUniqueKey(key);

	zval* gsession = htab_ptr::get_global(SIN.global_sess);

	if (gsession && (Z_TYPE_P(gsession) == IS_REFERENCE))
	{
		htab_rw sess(gsession);
		zval* test = sess.get(ukey);
		if (test)
		{
			result = test;
			if (remove) {
				sess.unset(ukey);
			}
		}
		else {
			result = defval;
		}
	}
	return result;
}

val_rc
ISession::__get(str_ptr key)
{
	return this->get(key,val_rc::null_value_ptr(), false);
}
val_rc 
ISession::__isset(str_ptr key)
{
	return this->has(key);
}

void   
ISession::__set(str_ptr key, val_ptr value)
{
	return this->set(key, value);
}



void   
ISession::__unset(str_ptr key)
{
	return this->remove(key);
}

obj_rc 
ISession::getAdapter()
{
	return adapter_;
}

str_rc 
ISession::getEndTime()
{
	str_rc result;

	if (adapter_.ok())
	{
		if (adapter_.method_exists(SIN.getexpires_fn))
		{
			val_rc extime = adapter_.call(SIN.getexpires_fn);
			if (extime.isLong())
			{
				result = datetime_obj::date(SIN.expires_fmt, extime.zlong());
			}
		}
	}
	if (!result.ok())
	{
		result = SIN.unknown_str;
	}
	return result;
}

bool 
ISession::has(str_ptr key)
{
	if (!ISession::exists())
	{
		return false;
	}

	str_rc ukey = getUniqueKey(key);

	zval* gsession = htab_ptr::get_global(SIN.global_sess);

	if (gsession && (Z_TYPE_P(gsession)==IS_REFERENCE))
	{
		htab_ptr sess(gsession);
		return sess.has_key(ukey);
	}
	return false;
}

str_rc 
ISession::getUniqueKey(str_ptr key)
{
	str_rc result;

	if (prefix_.size())
	{
		str_buf buf;

		buf << prefix_ << '#' << key;

		result = buf.zstr();
	}
	else {
		result = key;
	}
	return result;
}

str_rc //static
ISession::id(str_ptr id)
{
	return zpp::session_id(id);
}

str_rc //static
ISession::name(str_ptr name)
{
	return zpp::session_name(name);
}

bool 
ISession::regenerateId(bool deleteOld)
{
	if (!ISession::exists())
	{
		return false;
	}
	return session_regenerate_id(deleteOld);
}

void
ISession::remove(str_ptr key)
{
	if (!ISession::exists()) {
		return;
	}
	str_rc ukey = getUniqueKey(key);
	zval* gsession = htab_ptr::get_global(SIN.global_sess);
	if (gsession && (Z_TYPE_P(gsession)==IS_REFERENCE))
	{
		htab_rw sess(gsession);
		sess.unset(ukey);
	}
}

//C++ only
void 
ISession::set(str_ptr key, obj_ptr value)
{
	val_rc temp(value);
	set(key, temp);
}

void 
ISession::set(str_ptr key, val_ptr value)
{
	if (ISession::exists())
	{
		str_rc ukey = getUniqueKey(key);
		zval* gsession = htab_ptr::get_global(SIN.global_sess);

		if (Z_TYPE_P(gsession) == IS_REFERENCE)
		{
			htab_rw sess(gsession);
			sess.set(ukey, value);
		}
	}
}

void
ISession::setAdapter(obj_ptr itf)
{
	adapter_ = itf;
}

void ISession::setOptions(htab_ptr opt)
{
	prefix_ =  opt.get(SIN.prefix_str);
	if (!prefix_.ok())
	{
		prefix_ = str_ptr::empty_str();
	}
	options_ = opt;
}

bool
ISession::start()
{
	if (ISession::exists() || headers_sent())
	{
		return false; 
	}

	str_rc path = options_.get(SIN.savePath_str);

	session_save_path(path);


	bool result = true;

	if (adapter_.ok())
	{
		result = session_set_save_handler(adapter_, true);
	}
	if (result)
	{
		result = session_start();
	}
	return result;
}
}//wcc namespace


using namespace wcc;
using namespace zpp;

ZEND_METHOD(Wcc_Session_ISession, __construct)
{
	zarg_rd args(execute_data);
	htab_ptr options = args.htab(args.option(0));

	if (!args.throw_errors())
	{
		if (!options.ok())
		{
			options = htab_ptr::empty_array();
		}
		ISession* cobj = zval_toc<ISession>(ZEND_THIS);
		cobj->construct(options);
	}
}

ZEND_METHOD(Wcc_Session_ISession, destroy)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	ISession* cobj = zval_toc<ISession>(ZEND_THIS);
	cobj->destroy();

}

ZEND_METHOD(Wcc_Session_ISession, exists)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	bool result = ISession::exists();
	RETURN_BOOL(result);
}

ZEND_METHOD(Wcc_Session_ISession, getAdapter)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	ISession* cobj = zval_toc<ISession>(ZEND_THIS);
	obj_rc result = cobj->getAdapter();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Session_ISession, getEndTime)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	ISession* cobj = zval_toc<ISession>(ZEND_THIS);
	str_rc result = cobj->getEndTime();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Session_ISession, id)
{
	zarg_rd args(execute_data);
	str_ptr idval;

	args.zstring_null(idval, args.option(0));
	if (!args.throw_errors())
	{
		str_rc result = ISession::id(idval);
		if (!result.ok())
		{
			RETURN_BOOL(false);
		}
		else {
			result.move_zv(return_value);
		}
	}
}

ZEND_METHOD(Wcc_Session_ISession, name)
{
	zarg_rd args(execute_data);
	str_ptr nameval;

	args.zstring_null(nameval, args.option(0));
	if (!args.throw_errors())
	{
		str_rc result = ISession::name(nameval);
		if (!result.ok())
		{
			RETURN_BOOL(false);
		}
		else {
			result.move_zv(return_value);
		}
	}

}

ZEND_METHOD(Wcc_Session_ISession, status)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	int result = ISession::status();
	RETURN_LONG(result);
}

ZEND_METHOD(Wcc_Session_ISession, getOptions)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	ISession* cobj = zval_toc<ISession>(ZEND_THIS);
	htab_rc result = cobj->getOptions();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Session_ISession, has)
{
	zarg_rd args(execute_data);
	str_ptr key = args.str(args.need(0));

	if (!args.throw_errors())
	{
		ISession* cobj = zval_toc<ISession>(ZEND_THIS);
		bool result = cobj->has(key);
		RETURN_BOOL(result);
	}
}

ZEND_METHOD(Wcc_Session_ISession, regenerateId)
{
	zarg_rd args(execute_data);
	bool deleteOld = false; //default
	args.zbool(deleteOld, args.option(0));
	if (!args.throw_errors())
	{
		ISession* cobj = zval_toc<ISession>(ZEND_THIS);
		bool result = cobj->regenerateId(deleteOld);
		RETURN_BOOL(result);
	}
}

ZEND_METHOD(Wcc_Session_ISession, remove)
{
	zarg_rd args(execute_data);
	str_ptr key = args.str(args.need(0));
	if (!args.throw_errors())
	{
		ISession* cobj = zval_toc<ISession>(ZEND_THIS);
		cobj->remove(key);
	}
}

ZEND_METHOD(Wcc_Session_ISession, set)
{
	zarg_rd args(execute_data);
	str_ptr key = args.str(args.need(0));
	val_ptr value = args.need(1);

	if (!args.throw_errors())
	{
		ISession* cobj = zval_toc<ISession>(ZEND_THIS);
		cobj->set(key, value);
	}
}
ZEND_METHOD(Wcc_Session_ISession, setAdapter)
{
	zarg_rd args(execute_data);
	obj_ptr itfobj = args.obj_class(args.need(0), php_session_iface_entry);
	if (!args.throw_errors())
	{
		ISession* cobj = zval_toc<ISession>(ZEND_THIS);
		cobj->setAdapter(itfobj);
	}
}

ZEND_METHOD(Wcc_Session_ISession, setOptions)
{
	zarg_rd args(execute_data);
	htab_ptr values = args.htab(args.need(0));
	if (!args.throw_errors())
	{
		ISession* cobj = zval_toc<ISession>(ZEND_THIS);
		cobj->setOptions(values);
	}
}

ZEND_METHOD(Wcc_Session_ISession, start)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	ISession* cobj = zval_toc<ISession>(ZEND_THIS);
	bool result = cobj->start();
	RETURN_BOOL(result);
}

ZEND_METHOD(Wcc_Session_ISession, __get)
{	
	zarg_rd args(execute_data);
	str_ptr key = args.str(args.need(0));
	if (!args.throw_errors())
	{
		ISession* cobj = zval_toc<ISession>(ZEND_THIS);
		val_rc result = cobj->get(key, val_rc::null_value_ptr(), false);
		result.move_zv(return_value);
	}
}

ZEND_METHOD(Wcc_Session_ISession, __isset)
{	
	zarg_rd args(execute_data);
	str_ptr key = args.str(args.need(0));
	if (!args.throw_errors())
	{
		ISession* cobj = zval_toc<ISession>(ZEND_THIS);
		bool result = cobj->has(key);
		RETURN_BOOL(result);
	}	
}

ZEND_METHOD(Wcc_Session_ISession, __set)
{	
	zarg_rd args(execute_data);
	str_ptr key = args.str(args.need(0));
	val_ptr value = args.need(1);

	if (!args.throw_errors())
	{
		ISession* cobj = zval_toc<ISession>(ZEND_THIS);
		cobj->set(key, value);
	}	
}

ZEND_METHOD(Wcc_Session_ISession, __unset)
{	
	zarg_rd args(execute_data);
	str_ptr key = args.str(args.need(0));
	if (!args.throw_errors())
	{
		ISession* cobj = zval_toc<ISession>(ZEND_THIS);
		cobj->remove(key);
	}	
}


PHP_MINIT_FUNCTION(wcc_isession_reg)
{
	zend_class_entry* ce = register_class_Wcc_Session_ISession();
	ISession::omg.classEntry(ce);

	STATE_INIT_ADD(SIN);

	return SUCCESS;
}

#endif