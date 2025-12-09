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

class TargetData : public state_init {
public:

	str_intern class_name;
	str_intern method;
	str_intern module;
	str_intern index;

	TargetData() : state_init() {}

	void init() override
	{
		// "protected property names"
		class_name = "class_name";
		method = "func_name";
		module = "module_name";
		index = "index";
	}
};

TargetData target_data;

base_obj_mgr<Target> Target::omg;

void Target::debug_info(htab_rw di)
{
	di.set(target_data.class_name, class_);
	di.set(target_data.method, func_);
	di.set(target_data.module, module_);
}
	
obj_rc 
Target::go(str_ptr cname, str_ptr fname)
{
	obj_rc result;

	result = Target::omg.new_zobj();

	Target* cobj = zobj_toc<Target>(result);

	cobj->construct(cname, fname);
	//val_rc temp(result);
	//dump_info::msg_dump("target go ", temp);
	return result;
}


void 
Target::construct(str_ptr cname, str_ptr fname)
{
	class_ = cname;
	if (!fname.size())
	{
		func_ = target_data.index;
	}
	else { 
		func_ = fname;
	}
	//showobj("target construct ", vobj());

}

obj_rc
Target::copy()
{
	obj_rc result = Target::omg.new_zobj();

	Target* cobj = zobj_toc<Target>( result );

	cobj->construct(this->class_, this->func_);
	cobj->setModule(this->module_);

	return result;
}

str_ptr 
Target::getClass()
{
	return class_;
}

str_ptr 
Target::getFunc()
{
	return func_;
}

str_ptr 
Target::getModule()
{
	return module_;
}

void Target::setFunc(str_ptr name)
{
	func_ = name;
}

void Target::setModule(str_ptr name)
{
	module_ = name;
}

htab_rc
Target::serialize()
{
	htab_rc result;

	htab_rw hw(result);

	this->debug_info(hw);

	return result;
}

void 
Target::unserialize(htab_ptr htab)
{
	//showdata("Target", htab);
	class_ = htab.get(target_data.class_name);
	func_ = htab.get(target_data.method);
	module_ = htab.get(target_data.module);
}

}; //namespace wcc

ZEND_METHOD(Wcc_Target, __construct)
{
	zend_string* cname;
	zend_string* func;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_STR(cname)
	Z_PARAM_STR(func)
	ZEND_PARSE_PARAMETERS_END();

	Target* cobj = zval_toc<Target>(ZEND_THIS);
	cobj->construct(cname, func);

}

ZEND_METHOD(Wcc_Target, go)
{
	zend_string* cname;
	zend_string* func = nullptr;

	ZEND_PARSE_PARAMETERS_START(1,2)
	Z_PARAM_STR(cname)
	Z_PARAM_OPTIONAL
	Z_PARAM_STR(func)
	ZEND_PARSE_PARAMETERS_END();

	obj_rc obj = Target::go(cname, func);
	//TODO: why needs a ref boost?
	obj.move_zv(return_value);
	//showobj("obj after move,", obj);
	//showmem("return_value after move,", return_value);
	//val_rc::try_addref(return_value);

}

ZEND_METHOD(Wcc_Target, getClass)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Target* cobj = zval_toc<Target>(ZEND_THIS);
	str_ptr cname = cobj->getClass();

	cname.copy_zv(return_value);
}


ZEND_METHOD(Wcc_Target, getFunc)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Target* cobj = zval_toc<Target>(ZEND_THIS);
	str_ptr name = cobj->getFunc();

	name.copy_zv(return_value);	
}


ZEND_METHOD(Wcc_Target, getModule)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Target* cobj = zval_toc<Target>(ZEND_THIS);
	str_ptr name = cobj->getModule();

	name.copy_zv(return_value);	
}


ZEND_METHOD(Wcc_Target, setFunc)
{
	zend_string* func;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(func)
	ZEND_PARSE_PARAMETERS_END();
	
	Target* cobj = zval_toc<Target>(ZEND_THIS);
	cobj->setFunc(func);

}


ZEND_METHOD(Wcc_Target, setModule)
{
	zend_string* module_name;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(module_name)
	ZEND_PARSE_PARAMETERS_END();
	
	Target* cobj = zval_toc<Target>(ZEND_THIS);
	cobj->setModule(module_name);	

	// Return this Target Object (in ZEND_THIS);
	ZVAL_COPY(return_value, ZEND_THIS);
}


ZEND_METHOD(Wcc_Target, __serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Target* cobj = zval_toc<Target>(ZEND_THIS);

	htab_rc ret = cobj->serialize();
	ret.move_zv(return_value);
}


ZEND_METHOD(Wcc_Target, __unserialize)
{
	zval* data;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ARRAY(data)
	ZEND_PARSE_PARAMETERS_END();

	htab_ptr htab(Z_ARR_P(data));

	Target* cobj = zval_toc<Target>(ZEND_THIS);

	cobj->unserialize(htab);	
}




PHP_MINIT_FUNCTION(wcc_target)
{
	//WcR_ce = wcc_class_reg("Wc\\Route", class_Wcc_Route_methods);
	zend_class_entry *ce = register_class_Wcc_Target();
	Target::omg.classEntry(ce);
	
	return SUCCESS;

}
//target.cpp
#endif