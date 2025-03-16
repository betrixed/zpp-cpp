#ifndef CLASS_DATA_CPP
#define CLASS_DATA_CPP

#ifndef CLASS_DATA_H
#include "class_data.h"
#endif

namespace zpp {

void 
class_data::typed_property(
    zstr_user name, zval_user data, 
    zend_type datatype, int accessflag)
{
    zend_declare_typed_property(class_entry_,
            name, 
            data,
            accessflag, 
            /*doc_comment zend_string*/ nullptr, 
             datatype);
}

void 
class_data::add_constant(const char* name, const char* value)
{
    zend_declare_class_constant_stringl(
        class_entry_, 
        name, strlen(name), 
        value, strlen(value));
}

void 
class_data::add_constant(const char* name,  zend_string* value)
{
    zval_mgr temp(value);
    zend_declare_class_constant(class_entry_, name, strlen(name), temp);
    
    //add_constant(name, value.data());
}

void 
class_data::add_constant(const char* name,  zend_long value)
{
    zend_declare_class_constant_long(
        class_entry_, 
        name, strlen(name), 
        value);
}

bool class_data::check(const char* msg)
{
    if (!class_entry_) {
        zend_throw_error(zend_ce_error, msg, 0);
        return false;
    } 
    return true;  
}
 class_data::class_data(zstr_user classname)
 {
    set(classname);
 }

 bool 
 class_data::set(zstr_user classname)
 {
    //showstr("class_data:set", classname);
    class_entry_ =  zend_fetch_class(classname, ZEND_FETCH_CLASS_SILENT);
    return check("class_data for class name failed");
 }

 bool  
 class_data::new_object(zobj_mgr& result)
 {
   
    if (!check("class_data without class_entry"))
    {
        return false;
    }

    zval_mgr   temp;
    auto code = object_init_ex(temp, class_entry_);

    if (code==SUCCESS)
    {
        //showmem("zstr_ptr new_object", &temp);
        // The new object was created already referenced.

        result = zval_user(temp).zobject();
        return true;
    }
    zend_string* s = class_entry_->name;
    zend_throw_error(zend_ce_exception, "new_object failed %s", ZSTR_VAL(s));
    return false;
 }

zobj_mgr//static
class_data::create_object(zstr_user classname)
{
    class_data temp(classname);

    zobj_mgr result;

    temp.new_object(result);

    return result;
}

zstr_user
class_data::className()
{
    if (class_entry_)
    {
        return class_entry_->name;
    }
    return zend_empty_string;
}

zobj_mgr//static
class_data::std_object()
{
    zval_mgr init;
    object_init(init);
    return zobj_mgr(std::move(init));
}


};


//class_data.cpp
#endif


