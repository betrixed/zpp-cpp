#ifndef OBJ_PTR_CPP
#define OBJ_PTR_CPP

/**
 * @file zpp/obj_ptr.cxx
 * @author Michael Rynn <michael.rynn.500@gmail.com>
 * @brief obj_ptr - A simple zend_object* wrapper
 * @copyright Copyright (c) 2025
 * @license BSD 3-Clause License
 */
#ifndef FN_CALL_H
#include "fn_call.h"
#endif

#ifndef OBJ_PTR_H
#include "obj_ptr.h"
#endif

#ifndef OBJ_RC_H
#include "obj_rc.h"
#endif

extern "C" {
#include <ext/date/php_date.h>
#include <Zend/zend_closures.h>
};


#ifndef HTAB_RC_H
#include "htab_rc.h"
#endif


namespace zpp {


const obj_ptr& 
obj_ptr::operator=(zval* rc)
{
    if (!rc) {
        obj_ = nullptr;
        return *this;
    }
    obj_ = val_ptr(rc).zobject();
    return *this;

}
/*
const obj_ptr& 
obj_ptr::operator=(const val_rc& rc)
{
    val_ptr test(rc);

    if (!test.isObject()) {
        obj_ = nullptr;
        return *this;
    }
    obj_ = test.zobject();
    return *this;

}
*/

obj_ptr::obj_ptr(const obj_rc& rc) : obj_(rc.obj_)
{
}

obj_ptr::obj_ptr(const val_rc& rc) 
{
    obj_ = val_ptr(rc).zobject();
}

const obj_ptr& 
obj_ptr::operator=(zend_object* rc)
{
    obj_ = rc;
    return *this;
}

htab_rc 
obj_ptr::properties()
{
    htab_rc result;
    property_list(result);
    return result;
}

bool 
obj_ptr::get_proplist(htab_rc& mgr)
{
    auto zobj = obj_;
    if (!zobj)
    {
        return false;
    }
    zend_property_info *prop_info;
    zend_class_entry *ce = obj_->ce;
    val_rc  retval;

    size_t ct = ce->default_properties_count;

    if (!ct)
    {
        return false;
    }

    htab_rw pstore(retval, ct);
    for (size_t i = 0; i < ct; i++) 
    {
        prop_info = ce->properties_info_table[i];

        if (!prop_info) {
            continue;
        }

        zval* propzval = OBJ_PROP(zobj, prop_info->offset);

        //showmem("prop ptr", propzval);

        pstore.set(prop_info->name, propzval);

        /* _zend_hash_append_ind(zobj->properties, prop_info->name,
            OBJ_PROP(zobj, prop_info->offset));
        */
    }
    mgr = retval;

    return true;
}
// return true if something found
bool 
obj_ptr::property_list(htab_rc& list)
{
	if (!obj_)
	{
		return false;
	}  

    HashTable* ptab = obj_->properties;

    if (!ptab)
    {
        //zend_printf("No properties yet\n");
        ptab = zend_std_get_properties(obj_);
    }
    else {
        //zend_printf("Has properties now\n");
    }

    if (!ptab) {
        return false;
    }
    

   
    //! second argument bool can force duplication
    /** ptab = zend_proptable_to_symtable(ptab,
        (obj_->ce->default_properties_count ||
         obj_->handlers != &std_object_handlers ||
         GC_IS_RECURSIVE(ptab))); */

    htab_rc temp(ptab);// borrow array

    //temp.adopt(ptab); 

    if (!temp.size())
    {
        return false;
    }

    list = std::move(temp);

    return true;
}

bool obj_ptr::isDateTime() const
{
    if (!obj_)
    {
        return false;
    }
    zend_class_entry* ce = obj_->ce;

    zend_class_entry* date_ce = php_date_get_interface_ce();

    return instanceof_function(ce, date_ce);
}

void 
obj_ptr::copy_zv(zval* ret) const 
{
    if (obj_)
        ZVAL_OBJ_COPY(ret, obj_);
    else
        ZVAL_NULL(ret);
}

zend_class_entry* 
obj_ptr::class_entry() const
{
    if (obj_) {
        return obj_->ce;
    }
    return nullptr;
}

zend_string* 
obj_ptr::className() const
{
    if (obj_) {
        return obj_->ce->name;
    }
    return nullptr;
}

void callable_failed()
{
   zend_throw_error(zend_ce_error, "Object callable failed "); 
}

val_rc
obj_ptr::callable()
{
    // no args
    // for call_user_function
    val_rc result;

    //callable is method of no object
    val_rc callme (obj_); 

    if (!callable_fn(result, callme))
    {
        callable_failed();
    }
    return result;
}

val_rc
obj_ptr::callable(zval* arg1)
{
    val_rc callme (obj_);

    val_rc   result;
    zval   argv = {0};

    ZVAL_COPY_VALUE(&argv, arg1);

    //showmem("callable arg1", argv);
    
    if (!callable_fn(result, callme, 1, &argv))
        callable_failed();
    return result;
}


val_rc
obj_ptr::callable(zval* arg1, zval* arg2)
{
    val_rc callme (obj_);

    val_rc      result;
    zval        argv[2] = {{0},{0}};

    ZVAL_COPY_VALUE(&argv[0], arg1);
    ZVAL_COPY_VALUE(&argv[1], arg2);

    if (!callable_fn(result, callme, 2, &argv[0]))
        callable_failed();
    return result;
}

zval* 
obj_ptr::property_ptr(str_ptr name)
{
    return zend_std_get_property_ptr_ptr(obj_, name, BP_VAR_IS, nullptr);
}

void 
obj_ptr::property(str_ptr key, obj_ptr value)
{
    zval temp = {0};
    val_ptr::object_bind(&temp, value);
    property(key, val_ptr(&temp));
}

void 
obj_ptr::property(str_ptr key, val_rc& value)
{
    const zend_class_entry* scope = EG(fake_scope);

    if (!scope) {
        scope = zend_get_executed_scope();
    }
    zend_update_property_ex((zend_class_entry*) scope, obj_, key, value);   
}

void obj_ptr::property(str_ptr key, str_ptr value)
{
    zval temp = {0};
    // No rc++, because zval is thrown away on exit.
    val_ptr::string_bind(&temp, value);
    property(key, val_ptr(&temp));
}

void
obj_ptr::property(str_ptr key, val_ptr value)
{
    // zend_class_entry* scope = obj_->ce;
    const zend_class_entry* scope = EG(fake_scope);

    if (!scope) {
        scope = zend_get_executed_scope();
    }
    zend_update_property_ex((zend_class_entry*)scope, obj_, key, value);    
}


bool 
obj_ptr::has_property(str_ptr name)
{
    zend_class_entry *ce;
    zend_property_info *property_info;

    if (!obj_)
    {
        return false;
    }
    ce = obj_->ce;
    property_info = (zend_property_info *) zend_hash_find_ptr(&ce->properties_info, name);
    if (property_info != nullptr && (
            !(property_info->flags & ZEND_ACC_PRIVATE) || property_info->ce == ce)
        ) 
    {
        return true;
    }
    //zend_printf("call handler has property\n");
    if ((obj_->handlers->has_property)(obj_, name, ZEND_PROPERTY_EXISTS, NULL))
    {
        return true;
    }
    return false;
}
/**
 * explicit, use return_value
 */

zval*
obj_ptr::property_get(str_ptr key, zval* ret)
{
    // I do not understand why or what scope is required, (?? private, public protected access?
    // or what would be most permissive.
 
    const zend_class_entry* scope = EG(fake_scope) ? EG(fake_scope) : zend_get_executed_scope();
    
    /**
     *  phpinternals book php7, probably outdated.
     *  says function can return pointer to zval  owned by object, and
     *  this hasn't been modified by read_property.
     * 
     *  But the the indirect value is for temporary zvals, like returned by call to __get
     *  and will have its reference count , which needs decrementing.
     * 
     *  Both work at same time, and then contain data with same reference count!!
     *  
     *  Not clear.
     *  This call only wants to return one value!
     *  Execution of direct & indirect indicates one may be same as the other!
     *  
     */ 
    return zend_read_property_ex( (zend_class_entry*) scope, obj_, key, 1, ret);
}


/**
 * Get a dynamic property by name 
 *  Relies on return value optimisation.
 *  Also result is expected to dec reference
 * */
val_rc
obj_ptr::property(str_ptr key)
{
    val_rc result;

    //amazing stuff from PHP-CPP Value::get(const har*, size_t)
    
    const zend_class_entry* scope = EG(fake_scope) ? EG(fake_scope) : zend_get_executed_scope();
    
    /**
     *  phpinternals book php7, probably outdated.
     *  says function can return pointer to zval  owned by object, and
     *  this hasn't been modified by read_property.
     * 
     *  But the the indirect value is for temporary zvals, like returned by call to __get
     *  and will have its reference count , which needs decrementing.
     * 
     *  Both work at same time, and then contain data with same reference count!!
     *  
     *  Not clear.
     *  This call only wants to return one value!
     *  Execution of direct & indirect indicates one may be same as the other!
     *  
     */ 
    //showstr("obj_ptr property get", key);

    zval* direct = zend_read_property_ex((zend_class_entry*) scope, obj_, key, 1, result);

    if (direct)
    {
        result = direct;
    }

    return result;
}

void obj_ptr::unset_property(str_ptr name)
{
    zend_std_unset_property(obj_, name, nullptr);
}

val_rc
obj_ptr::call(str_ptr method, zval* arg1)
{

    fn_call_args<1> caller;

    caller.set_fci(obj_, method);
    ZVAL_COPY_VALUE(caller.argsptr(), arg1);
    return caller.call_fn();
}

val_rc
obj_ptr::call(str_ptr method, 
            zval* arg1, zval* arg2)
{
    fn_call_args<2> caller;
    caller.set_fci(obj_, method);
    zval* pz = caller.argsptr();

    ZVAL_COPY_VALUE(pz, arg1);
    ZVAL_COPY_VALUE(pz+1, arg2);

    return caller.call_fn();
}

val_rc
obj_ptr::call(str_ptr method)
{
    fn_call fn;

    fn.set_fci(obj_, method);
    val_rc result = fn.call_fn();

    return result;
}


val_rc
obj_ptr::call(str_ptr method, HashTable* args)
{
    fn_call fn;

    fn.set_fci(obj_, method, args);

    return fn.call_fn();
}

val_rc
obj_ptr::call(str_ptr method, 
            zval*  arg1, zval*  arg2, zval*  arg3)
{
    fn_call_args<3> caller;

    caller.set_fci(obj_, method);
    zval* pz = caller.argsptr();

    ZVAL_COPY_VALUE(pz, arg1);
    ZVAL_COPY_VALUE(pz+1, arg2);
    ZVAL_COPY_VALUE(pz+2, arg3);

    return caller.call_fn();
}


val_rc
obj_ptr::call(str_ptr method, 
            zval*  arg1, zval*  arg2, 
            zval*  arg3, zval*  arg4)
{
    fn_call_args<4> caller;

    caller.set_fci(obj_, method);
    zval* pz = caller.argsptr();

    ZVAL_COPY_VALUE(pz, arg1);
    ZVAL_COPY_VALUE(pz+1, arg2);
    ZVAL_COPY_VALUE(pz+2, arg3);
    ZVAL_COPY_VALUE(pz+3, arg4);

    return caller.call_fn();
}

obj_ptr::obj_ptr(const val_ptr& rc)
{
    obj_ = rc.zobject();
}
#ifndef OMIT_BASE_D
obj_ptr::obj_ptr(base_d* cobj) 
{
    obj_ = cobj ? cobj->vobj() : nullptr;
}
#endif

bool 
obj_ptr::instanceof(zend_class_entry *ce) const
{
    if (!obj_)
        return false;
    if (ce == obj_->ce)
        return true;
    return instanceof_function_slow(obj_->ce, ce);
}


bool 
obj_ptr::method_exists(str_ptr method) const
{
    if (!obj_)
        return false;

    zend_class_entry* ce = obj_->ce;
    str_rc lcname_str(method);
    lcname_str.lowercase();

    zend_function*  func = (zend_function*) zend_hash_find_ptr(&ce->function_table, lcname_str);

    if (func) {
        return true;
    }
    //C-macro below needs zend_string*
    zend_string* lcname = lcname_str;


    zend_object* temp = obj_;

    func = obj_->handlers->get_method(&temp, lcname, nullptr); // why not use lowercased string?

    if (func != nullptr) 
    {
        bool result = true;
        if (func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) 
        {
            // Returns true for the fake Closure's __invoke 
            result = (func->common.scope == zend_ce_closure
                && zend_string_equals_literal_ci(lcname, ZEND_INVOKE_FUNC_NAME));

            zend_string_release_ex(func->common.function_name, 0);
            zend_free_trampoline(func);
        }
        return result;
    }
    // didn't make it
    return false;
}

obj_ptr::obj_ptr(zval* zp)
{
    if (!zp)
    {
        obj_ = nullptr;
        return;
    }
    obj_ = val_ptr(zp).zobject();
}

obj_rc
obj_ptr::clone() const 
{
    zend_object* copy = zend_objects_clone_obj(obj_);

    obj_rc result;
    result.adopt(copy);
    return  result;
}


}; // namespace
#endif
//obj_ptr.cpp
