#ifndef WCC_TARGET_CPP
#define WCC_TARGET_CPP

#ifndef WCC_TARGET_H
#include "target.h"
#endif

#ifndef WCC_TARGET_ARGINFO
#define WCC_TARGET_ARGINFO
extern "C" {
	#include "stub/target_arginfo.h"	
};
#endif

namespace wcc {
using namespace zpp;

TargetNames  TNinit;

void
TargetNames::init()
{
	p_objclass = "objclass";
	p_objmethod = "objmethod";
	s_index = "index";
	p_params = "params";
}


base_obj_mgr<Target> Target::omg;

/*
void Target::debug_info(htab_cow di)
{
	di.set(target_data.class_name, class_);
	di.set(target_data.method, func_);
	di.set(target_data.module, module_);
	di.set(target_data.params, params_);
}
	*/

str_rc 
Target::getModule()
{
	htab_cow cow = params();
	str_rc result;

	if (cow.size())
	{
		result = cow.get(route_data.MOD_S);
		if (!result.ok())
		{
			result = DSPi.default_str;
		}
	}
	return result;
}

str_rc 
Target::getClass()
{
	return obj_ptr(self_).str_property(TNinit.p_objclass);
}

void   
Target::setMethod(str_ptr s)
{
	obj_ptr(self_).property(TNinit.p_objmethod,s);
}

void   
Target::setModule(str_ptr s)
{
	htab_cow cow = params();
	cow.set(route_data.MOD_S, s);
}

str_rc 
Target::getMethod()
{
	return obj_ptr(self_).str_property(TNinit.p_objmethod);
}

obj_rc 
Target::go(str_ptr cname, str_ptr method, htab_ptr params)
{
	obj_rc result;

	result = Target::omg.new_zobj();

	Target* cobj = zobj_toc<Target>(result);

	cobj->construct(cname, method, params);
	//val_rc temp(result);
	//dump_info::msg_dump("target go ", temp);
	return result;
}


void 
Target::construct(str_ptr cname, str_ptr method, htab_ptr params)
{
	obj_ptr self(self_);

	self.property(TNinit.p_objclass, cname);
	if (!method.size())
	{
		method = TNinit.s_index;
	}
	self.property(TNinit.p_objmethod, method);

	if (!params.ok())
	{
		params = htab_ptr::empty_array();
	}
	self.property(TNinit.p_params, params);
	
	//showobj("target construct ", vobj());

}

val_rc 
Target::getRoles()
{
	val_rc result;

	htab_cow params = this->params();

	result = params.get(route_data.ROLE_S);

	return result;
}

val_rc 
Target::refParams()
{
	val_ptr ap(obj_ptr(self_).property_ptr(TNinit.p_params));
	return val_rc(ap.make_ref());              
}

htab_cow
Target::params() 
{
	val_ptr ap(obj_ptr(self_).property_ptr(TNinit.p_params));

	zval* pval = ap.make_ref();

	return htab_cow(pval);
}

}; //namespace wcc

ZEND_METHOD(Wcc_Target, __construct)
{
	zarg_rd args(execute_data);

	str_ptr cname = args.str(args.need(0));
	str_ptr method = args.str_or_default(args.option(1), TNinit.s_index);
	htab_ptr params = args.htab_or_null(args.option(2));

	if (!args.throw_errors())
	{
		Target* cobj = zval_toc<Target>(ZEND_THIS);
		cobj->construct(cname, method, params);
	}

}

ZEND_METHOD(Wcc_Target, go)
{
	zarg_rd args(execute_data);

	str_ptr cname = args.str(args.need(0));
	str_ptr method = args.str_or_default(args.option(1), TNinit.s_index);
	htab_ptr params = args.htab_or_null(args.option(2));

	if (!args.throw_errors())
	{
		obj_rc obj = Target::go(cname, method, params);
		obj.move_zv(return_value);
	}
}

ZEND_METHOD(Wcc_Target, refParams)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	Target* cobj = zval_toc<Target>(ZEND_THIS);
	val_rc result = cobj->refParams();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Target, getModule)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	Target* cobj = zval_toc<Target>(ZEND_THIS);
	str_rc result = cobj->getModule();
	result.move_zv(return_value);
}

PHP_MINIT_FUNCTION(wcc_target)
{
	//WcR_ce = wcc_class_reg("Wc\\Route", class_Wcc_Route_methods);
	zend_class_entry *ce = register_class_Wcc_Target();
	Target::omg.classEntry(ce);
	
	STATE_INIT_ADD(TNinit)
	
	return SUCCESS;

}
//target.cpp
#endif