#ifndef SERVICE_ACCESS_CPP
#define SERVICE_ACCESS_CPP
//service_access.cpp

#ifndef SERVICE_ACCESS_H
#include "service_access.h"
#endif

#ifndef WCC_SERVICES_H
#include "services.h"
#endif

using namespace zpp;

namespace wcc {

base_obj_mgr<ServiceAccess> ServiceAccess::omg;

class SADATA : public state_init {
public:
	SADATA() : state_init() {}

	zstr_intern services;
	zstr_intern cache;
	zstr_intern init_access;

	virtual void init()
	{
		services = "services";
		cache = "cache";
		init_access = "init_access";
	}
};

SADATA SAdata;

zobj_user
ServiceAccess::getServices() {
	return services_;
}

void 
ServiceAccess::setServices(zval_user svc)
{
	services_ = svc.zobject();
}

void ServiceAccess::init_access()
{
	/* for descendent classes */
}

//virtual
zstr_user
ServiceAccess::extender()
{
	return extender_.className();
}

void ServiceAccess::setExtender(zobj_user obj)
{
	if (obj.ok())
	{
		extender_.set(obj->ce);
	}
}

//virtual
void ServiceAccess::debug_info(htab_write hw)
{

	base_d::debug_info(hw);

	hw.set(SAdata.services, services_);
	hw.set(SAdata.cache, cache_);
}

void 
ServiceAccess::construct(zval_user services_obj)
{
	if (services_obj.isObject())
	{
		services_ = services_obj.zobject();
	}
	else {
		services_ =  Services::instance();
	}

	zobj_user caller (this);

	caller.call(SAdata.init_access);
}

void ServiceAccess::destruct()
{
	cache_.init();
	services_.init();
}

zval_mgr
ServiceAccess::service(zstr_user name)
{
	zval_mgr result;
	zval_user zu(result);

	htab_write hw(cache_);

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

	Services* gobj = Services::cpp_global();

	if (gobj != cobj) {
		result = gobj->get(name);
		if (!zu.isNull())
		{
			hw.set(name, result);
		}
	}
	return result;
}

void 
ServiceAccess::set(zstr_user name, zval_user value)
{
	htab_write(cache_).set(name, value);
}

bool 
ServiceAccess::has(zstr_user name)
{
	return htab_read(cache_).has_key(name);
}


void 
ServiceAccess::unset(zstr_user name)
{
	htab_write(cache_).unset(name);
}

zval_mgr 
ServiceAccess::nullService(zstr_user name)
{
	zval_mgr result;

	zstr_buffer buf;

	buf << "get" << name.to_lower();

	zstr_mgr method = buf.zstr();

	zobj_user self(this);
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
	zval_mgr result = cobj->service(name);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_ServiceAccess, service)
{
	zend_string* name;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(name);
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<ServiceAccess>(ZEND_THIS);
	zval_mgr result = cobj->service(name);
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
	zobj_user result = cobj->getServices();

	result.return_zv(return_value);
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