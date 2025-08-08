#ifndef CLASS_DATA_CPP
#define CLASS_DATA_CPP

#ifndef CLASS_DATA_H
#include "class_data.h"
#endif

#ifndef VAL_PTR_H
#include "val_ptr.h"
#endif

#ifndef SHOW_ZPP_H
#include "show_zpp.h"
#endif

extern "C" {
    #include "Zend/zend_API.h"
    #include "Zend/zend_exceptions.h"
}


namespace zpp {

void 
class_data::typed_property(
    str_ptr name, val_ptr data, 
    zend_type datatype, int accessflag)
{
    zend_declare_typed_property(class_entry_,
            name, 
            data,
            accessflag, 
            nullptr,  //doc_comment zend_string
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
    val_rc temp(value);
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
 class_data::class_data(str_ptr classname)
 {
    set(classname);
 }

zend_class_entry* //static
class_data::get_class(str_ptr classname)
{
    return zend_fetch_class(classname, ZEND_FETCH_CLASS_SILENT);
}

 bool 
 class_data::set(str_ptr classname)
 {
    //showstr("class_data:set", classname);
    class_entry_ =  zend_fetch_class(classname, ZEND_FETCH_CLASS_SILENT);
    return check("class_data for class name failed");
 }

 bool  
 class_data::new_object(obj_rc& result)
 {
   
    if (!check("class_data without class_entry"))
    {
        return false;
    }

    val_rc   temp;
    auto code = object_init_ex(temp, class_entry_);

    if (code==SUCCESS)
    {
        //showmem("zstr_ptr new_object", &temp);
        // The new object is already referenced.

        result = val_ptr(temp).zobject();
        return true;
    }
    zend_string* s = class_entry_->name;
    zend_throw_error(zend_ce_exception, "new_object failed %s", ZSTR_VAL(s));
    return false;
 }

obj_rc//static
class_data::create_object(str_ptr classname)
{
    class_data temp(classname);
    //showstr("create_object", classname);
    obj_rc result;

    temp.new_object(result);

    return result;
}

str_ptr
class_data::className()
{
    if (class_entry_)
    {
        return class_entry_->name;
    }
    return zend_empty_string;
}

obj_rc//static
class_data::std_object()
{
    val_rc init;
    object_init(init);
    return obj_rc(std::move(init));
}

void
class_data::static_property(zend_string* pname, zval* value)
{
    zend_update_static_property_ex(class_entry_, pname, value);
}

val_rc 
class_data::static_property(zend_string* s)
{
    zval* p = zend_read_static_property_ex(class_entry_, s, true);

    return val_rc(p);
}

val_rc 
class_data::constant_value(zend_string* s)
{   
    zend_class_constant *c = NULL;

    c = (zend_class_constant*)zend_hash_find_ptr(CE_CONSTANTS_TABLE(class_entry_), s);

    val_rc result;

    if (c)
    {
        result = &c->value;
    }
    return result;
}

};


//class_data.cpp
#endif


