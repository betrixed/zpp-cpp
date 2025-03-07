#ifndef WCC_TARGET_CPP
#define WCC_TARGET_CPP

#ifndef WCC_TARGET_H
#include "target.h"
#endif

namespace wcc {

class TargetData : public state_init {
public:

	zstr_intern class_name;
	zstr_intern method;
	zstr_intern module;
	zstr_intern index;

	TargetData() : state_init() {}

	virtual void init() {
		class_name = zstr_intern("class_name");
		method = zstr_intern("func_name");
		module = zstr_intern("module_name");
		index = zstr_intern("index");
	}
};

TargetData target_data;
base_obj_mgr<Target> Target::omg;

void Target::debug_info(htab_write di)
{
	di.set(target_data.class_name, class_);
	di.set(target_data.method, func_);
	di.set(target_data.module, module_);
}
	
zobj_mgr 
Target::go(zstr_user cname, zstr_user fname)
{

	zobj_mgr result;

	result = Target::omg.new_zobj();

	Target* cobj = zobj_toc<Target>(result);

	cobj->construct(cname, fname);

	return result;
}


void 
Target::construct(zstr_user cname, zstr_user fname)
{
	class_ = cname;
	if (fname.isNull() || fname.size()==0)
	{
		func_ = target_data.index;
	}
	else { 
		func_ = fname;
	}
}

zstr_user 
Target::getClass()
{
	return class_;
}

zstr_user 
Target::getFunc()
{
	return func_;
}

zstr_user 
Target::getModule()
{
	return module_;
}

void Target::setFunc(zstr_user name)
{
	func_ = name;
}

void Target::module(zstr_user name)
{
	module_ = name;
}

htab_mgr
Target::serialize()
{
	htab_mgr result;

	htab_write hw(result);

	this->debug_info(hw);

	return result;
}

void 
Target::unserialize(htab_read htab)
{
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

	zobj_mgr obj = Target::go(cname, func);

	obj.move_zv(return_value);
}

ZEND_METHOD(Wcc_Target, getClass)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Target* cobj = zval_toc<Target>(ZEND_THIS);
	zstr_user cname = cobj->getClass();

	cname.return_zv(return_value);
}


ZEND_METHOD(Wcc_Target, getFunc)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Target* cobj = zval_toc<Target>(ZEND_THIS);
	zstr_user name = cobj->getFunc();

	name.return_zv(return_value);	
}


ZEND_METHOD(Wcc_Target, getModule)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Target* cobj = zval_toc<Target>(ZEND_THIS);
	zstr_user name = cobj->getModule();

	name.return_zv(return_value);	
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


ZEND_METHOD(Wcc_Target, module)
{
	zend_string* module_name;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(module_name)
	ZEND_PARSE_PARAMETERS_END();
	
	Target* cobj = zval_toc<Target>(ZEND_THIS);
	cobj->module(module_name);	
}


ZEND_METHOD(Wcc_Target, __serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Target* cobj = zval_toc<Target>(ZEND_THIS);

	htab_mgr ret = cobj->serialize();
	ret.move_zv(return_value);
}


ZEND_METHOD(Wcc_Target, __unserialize)
{
	zval* data;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ARRAY(data)
	ZEND_PARSE_PARAMETERS_END();

	htab_read htab(Z_ARR_P(data));

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