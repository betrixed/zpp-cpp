 /*  
  *  PHP extension C++ classes - zpp 
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  */

#ifndef STATE_INIT_CPP
#define STATE_INIT_CPP

#ifndef STATE_INIT_H
#include "state_init.h"
#endif

namespace zpp {

// static and externals

state_init* state_init::first_ = nullptr;
state_init* state_init::last_ = nullptr;

state_init::state_init() : next_((state_init*)nullptr)
{
    //zend_printf("state_init\n");
    if (first_ == nullptr)
    {
        first_ = this;
        last_ = this;
    }
    else {
        last_->next_ = this;
        last_ = this;
    }
}


void //static
state_init::init_all()
{
    state_init* link = state_init::first_;
    while(link)
    {
        link->init();
        link = link->next_;
    }
}

void //static
state_init::end_all()
{
    state_init* link = state_init::first_;
    while(link)
    {
        state_init* temp = link;
        link = link->next_;
        temp->end();
    }
}

void //virtual
state_init::init()
{
}

void //virtual
state_init::end()
{
}

/* not expecting any work here */
//virtual
state_init::~state_init()
{
}



void 
class_data::add_constant(const char* name, const char* value)
{
        //zend_printf(" %s = %s\n", name, value);

        zend_declare_class_constant_stringl(
        class_entry_, 
        name, strlen(name), 
        value, strlen(value));
}
/**
 * 
zend_declare_class_constant_ex(
    end_class_entry *ce, 
    zend_string *name, 
    zval *value, 
    int flags, 
    zend_string *doc_comment)
*/



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

zobj_mgr//static
class_data::std_object()
{
    zval_mgr init;
    object_init(init);
    return zobj_mgr(std::move(init));
}

}; // namespace
#endif
