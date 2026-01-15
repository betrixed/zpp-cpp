#ifndef SERVICE_ACCESS_CPP
#define SERVICE_ACCESS_CPP
//service_access.cpp

#ifndef SERVICE_ACCESS_H
#include "service_access.h"
#endif

#ifndef WCC_SERVICES_H
#include "services.h"
#endif

#ifndef WCC_SERVICEACCESS_ARGINFO
#define WCC_SERVICEACCESS_ARGINFO
extern "C" {
	#include "stub/serviceaccess_arginfo.h"
};
#endif

using namespace zpp;

namespace wcc {

base_obj_mgr<ServiceAccess> ServiceAccess::omg;

class SADATA : public state_init {
public:

	str_intern services;
	str_intern cache;
	str_intern init_access;
	str_intern gservices;

	void init() override
	{
		services = "services";
		cache = "cache";
		init_access = "init_access";
		gservices = "gservices";
	}
};

SADATA SAdata;



void ServiceAccess::init_access()
{
	/* php override for descendent classes */
}

//virtual
str_ptr
ServiceAccess::extender()
{
	return extender_.className();
}

void ServiceAccess::setExtender(obj_ptr obj)
{
	if (obj.ok())
	{
		extender_.set(obj->ce);
	}
}

//virtual
void ServiceAccess::debug_info(htab_rw hw)
{

	base_d::debug_info(hw);
	hw.set(SAdata.cache, cache_);
}

void 
ServiceAccess::construct(val_ptr services_obj)
{
	gservices_ =  Services::instance();
	if (services_obj.isObject())
	{
		services_ = services_obj.zobject();
	}
	else {
		services_ =  gservices_;
	}

	obj_ptr self(vobj());
	val_rc arg(gservices_);
	self.property(SAdata.gservices, arg);

	arg = services_;
	self.property(SAdata.services, arg);

	self.call(SAdata.init_access);
}

void ServiceAccess::destruct()
{
	cache_.init();
	services_.init();
}

val_rc
ServiceAccess::service(str_ptr name)
{
	val_rc result;
	val_ptr zu(result);

	htab_rw hw(cache_);
	result = hw.get(name);

	if (!zu.isNull())
	{
		return result;
	}

	Services* cobj = zobj_toc<Services>(services_);

	result = cobj->get(name);

	if (!zu.isNull()) {
		hw.set(name, result);
		return result;
	}

	if ((zend_object*) services_ != (zend_object*) gservices_)
	{
		 cobj = zobj_toc<Services>(gservices_);

		result = cobj->get(name);
		if (!zu.isNull())
		{
			hw.set(name, result);
		}
	}

	return result;
}

void 
ServiceAccess::set(str_ptr name, val_ptr value)
{
	htab_rw(cache_).set(name, value);
}

bool 
ServiceAccess::has(str_ptr name)
{
	val_rc test = service(name);
	return !test.isNull();
}


void 
ServiceAccess::unset(str_ptr name)
{
	htab_rw(cache_).unset(name);
}

val_rc 
ServiceAccess::nullService(str_ptr name)
{
	val_rc result;

	str_buf buf;

	str_rc temp(name);
	temp.lowercase();
	buf << "get" << temp;

	str_rc method = buf.zstr();

	obj_ptr self(this);
	if (self.method_exists(method))
	{
		result = self.call(method);
		return result;
	}
	zend_throw_error(zend_ce_error, "Service %s not found", name.data());
	return result;
}

}; //namespace

using namespace wcc;

ZEND_METHOD(Wcc_ServiceAccess, __construct)
{
	zend_class_entry* svc_ce = Services::omg.classEntry();
	zval*  svc = nullptr;

	ZEND_PARSE_PARAMETERS_START(0, 1)
	Z_PARAM_OPTIONAL
	Z_PARAM_OBJECT_OF_CLASS_OR_NULL(svc, svc_ce);
	ZEND_PARSE_PARAMETERS_END();

	//showobj("sa construct", svc);
	auto cobj = zval_toc<ServiceAccess>(ZEND_THIS);
	cobj->construct(svc);
}

ZEND_METHOD(Wcc_ServiceAccess, __destruct)
{
	ZEND_PARSE_PARAMETERS_NONE();
	auto cobj = zval_toc<ServiceAccess>(ZEND_THIS);
	cobj->destruct();
}

ZEND_METHOD(Wcc_ServiceAccess, __get)
{
	zend_string* name;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(name);
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<ServiceAccess>(ZEND_THIS);
	val_rc result = cobj->service(name);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_ServiceAccess, service)
{
	zend_string* name;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(name);
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<ServiceAccess>(ZEND_THIS);
	val_rc result = cobj->service(name);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_ServiceAccess, init_access)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<ServiceAccess>(ZEND_THIS);
	cobj->init_access();
}

ZEND_METHOD(Wcc_ServiceAccess, getServices)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<ServiceAccess>(ZEND_THIS);
	obj_ptr result = cobj->getServices();

	result.copy_zv(return_value);
}


ZEND_METHOD(Wcc_ServiceAccess, __set)
{
	zend_string*        name;
	zval*               value;

	ZEND_PARSE_PARAMETERS_START(2, 2)
	Z_PARAM_STR(name)
	Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	ServiceAccess* cobj = zval_toc<ServiceAccess>(ZEND_THIS);	

	cobj->set(name, value);
}

ZEND_METHOD(Wcc_ServiceAccess, hasService)
{
	zend_string*        name;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	ServiceAccess* cobj = zval_toc<ServiceAccess>(ZEND_THIS);	

	bool result = cobj->has(name);
	RETURN_BOOL(result);
}

ZEND_METHOD(Wcc_ServiceAccess, __isset)
{
	zend_string*        name;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	ServiceAccess* cobj = zval_toc<ServiceAccess>(ZEND_THIS);	

	bool result = cobj->has(name);
	RETURN_BOOL(result);
}

ZEND_METHOD(Wcc_ServiceAccess, __unset)
{
	zend_string*        name;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	ServiceAccess* cobj = zval_toc<ServiceAccess>(ZEND_THIS);	

	cobj->unset(name);

}

ZEND_METHOD(Wcc_ServiceAccess, unset)
{
	zend_string*        name;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	ServiceAccess* cobj = zval_toc<ServiceAccess>(ZEND_THIS);	

	cobj->unset(name);

}
/*
ZEND_METHOD(Wcc_ServiceAccess, setServices)
{
	zval*        sobj;
	zend_class_entry* sv_ce = Services::omg.classEntry();

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_OBJECT_OF_CLASS(sobj, sv_ce)
	ZEND_PARSE_PARAMETERS_END();

	ServiceAccess* cobj = zval_toc<ServiceAccess>(ZEND_THIS);	

	cobj->setServices(sobj);
}
*/

ZEND_METHOD(Wcc_ServiceAccess, setExtender)
{
	zval*        sobj;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_OBJECT(sobj)
	ZEND_PARSE_PARAMETERS_END();

	ServiceAccess* cobj = zval_toc<ServiceAccess>(ZEND_THIS);	

	cobj->setExtender(sobj);
}


PHP_MINIT_FUNCTION(ServiceAccess_reg)
{
	auto ce = register_class_Wcc_ServiceAccess();

	ServiceAccess::omg.classEntry(ce);

	return SUCCESS;
}
#endif