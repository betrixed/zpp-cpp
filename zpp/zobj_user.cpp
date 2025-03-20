#ifndef ZOBJ_USER_CPP
#define ZOBJ_USER_CPP

#ifndef FN_CALL_H
#include "fn_call.h"
#endif

extern "C" {
#include <ext/date/php_date.h>
#include <Zend/zend_closures.h>
};


namespace zpp {


const zobj_user& 
zobj_user::operator=(zval* rc)
{
    if (!rc) {
        obj_ = nullptr;
        return *this;
    }
    obj_ = zval_user(rc).zobject();
    return *this;

}
/*
const zobj_user& 
zobj_user::operator=(const zval_mgr& rc)
{
    zval_user test(rc);

    if (!test.isObject()) {
        obj_ = nullptr;
        return *this;
    }
    obj_ = test.zobject();
    return *this;

}
*/

zobj_user::zobj_user(base_d* cobj)
{
    obj_ = cobj->vobj();
}

zobj_user::zobj_user(const zval_mgr& rc) 
{
    obj_ = zval_user(rc).zobject();
}

const zobj_user& 
zobj_user::operator=(zend_object* rc)
{
    obj_ = rc;
    return *this;
}

// return true if something found
bool 
zobj_user::property_list(htab_mgr& list)
{
	if (!obj_)
	{
		return false;
	}

	HashTable *ptab = obj_->handlers->get_properties(obj_);

    if (!ptab) {
        return false;
    }

    //! second argument bool can force duplication
    ptab = zend_proptable_to_symtable(ptab,
        (obj_->ce->default_properties_count ||
         obj_->handlers != &std_object_handlers ||
         GC_IS_RECURSIVE(ptab)));

    htab_mgr temp;

    temp.adopt(ptab); // allow for destroy array

    if (zend_array_count(ptab)) {
        list = std::move(temp);
        return true;
    }
    return false;
}

bool zobj_user::isDateTime() const
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
zobj_user::return_zv(zval* ret)
{
    if (obj_)
        ZVAL_OBJ_COPY(ret, obj_);
    else
        ZVAL_NULL(ret);
}

zend_string* 
zobj_user::className()
{
    if (obj_) {
        return obj_->ce->name;
    }
    return nullptr;
}

void callable_failed()
{
   zend_throw_error(zend_ce_error, "Object callable failed ", 0); 
}

zval_mgr
zobj_user::callable()
{
    // no args
    // for call_user_function
    zval_mgr result;

    //callable is method of no object
    zval_mgr callme (obj_); 

    if (!callable_fn(result, callme))
    {
        callable_failed();
    }
    return result;
}

zval_mgr
zobj_user::callable(zval* arg1)
{
    zval_mgr callme (obj_);

    zval_mgr   result;
    zval_init  argv;

    ZVAL_COPY_VALUE(argv, arg1);

    //showmem("callable arg1", argv);
    
    if (!callable_fn(result, callme, 1, argv))
        callable_failed();
    return result;
}


zval_mgr
zobj_user::callable(zval* arg1, zval* arg2)
{
    zval_mgr callme (obj_);

    zval_mgr      result;
    zval_init     argv[2];

    ZVAL_COPY_VALUE(argv[0], arg1);
    ZVAL_COPY_VALUE(argv[1], arg2);

    if (!callable_fn(result, callme, 2, argv[0]))
        callable_failed();
    return result;
}
void
zobj_user::property(zstr_user key, zval_user value)
{
    // zend_class_entry* scope = obj_->ce;
    zend_class_entry* scope = EG(fake_scope);

    if (!scope) {
        scope = zend_get_executed_scope();
        //zend_printf("ex scope %lx\n", scope);
    }

    zend_update_property_ex(scope, obj_, key, value);    
}

/**
 * Get a dynamic property by name 
 *  Relies on return value optimisation.
 *  Also result is expected to dec reference
 * */
zval_mgr
zobj_user::property(zstr_user key)
{
    zval_mgr result;

    //amazing stuff from PHP-CPP Value::get(const har*, size_t)
 
    zend_class_entry* scope = EG(fake_scope) ? EG(fake_scope) : zend_get_executed_scope();
    
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
    zend_read_property_ex(scope, obj_, key, 0, result);
    return result;
}

/**
 * explicit, use return_value
 */

zval*
zobj_user::property_get(zstr_user key, zval* ret)
{
    zval_mgr result;

    //amazing stuff from PHP-CPP Value::get(const har*, size_t)
 
    zend_class_entry* scope = EG(fake_scope) ? EG(fake_scope) : zend_get_executed_scope();
    
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
    return zend_read_property_ex(scope, obj_, key, 0, ret);
}

zval_mgr
zobj_user::call(zstr_user method, zval* arg1)
{

    fn_call_args<1> caller;

    caller.set_fci(obj_, method);
    ZVAL_COPY_VALUE(caller.argsptr(), arg1);
    return caller.call_fn();
}

zval_mgr
zobj_user::call(zstr_user method, 
            zval* arg1, zval* arg2)
{
    fn_call_args<2> caller;
    caller.set_fci(obj_, method);
    zval* pz = caller.argsptr();

    ZVAL_COPY_VALUE(pz, arg1);
    ZVAL_COPY_VALUE(pz+1, arg2);

    return caller.call_fn();
}

zval_mgr
zobj_user::call(zstr_user method)
{
    fn_call fn;

    fn.set_fci(obj_, method);

    return fn.call_fn();
}


zval_mgr
zobj_user::call(zstr_user method, HashTable* args)
{
    fn_call fn;

    fn.set_fci(obj_, method, args);

    return fn.call_fn();
}

zval_mgr
zobj_user::call(zstr_user method, 
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


zval_mgr
zobj_user::call(zstr_user method, 
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

zobj_user::zobj_user(const zval_user& rc)
{
    obj_ = rc.zobject();
}

bool 
zobj_user::instanceof(zend_class_entry *ce) const
{
    if (!obj_)
        return false;
    if (ce == obj_->ce)
        return true;
    return instanceof_function_slow(obj_->ce, ce);
}


bool 
zobj_user::method_exists(zstr_user method) const
{
    if (!obj_)
        return false;

    zend_class_entry* ce = obj_->ce;
    zstr_user method_name(method);
    zstr_mgr lcname_str = method_name.to_lower();

   
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

zobj_user::zobj_user(zval* zp)
{
    if (!zp)
    {
        obj_ = nullptr;
        return;
    }
    obj_ = zval_user(zp).zobject();
}

}; // namespace
#endif
//zobj_user.cpp
