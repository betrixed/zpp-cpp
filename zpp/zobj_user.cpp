#ifndef ZOBJ_USER_H
#define ZOBJ_USER_H

namespace zpp {

const zobj_user& 
zobj_user::operator=(zend_object* rc)
{
    obj_ = rc;
    return *this;
}

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

    ptab = zend_proptable_to_symtable(ptab,
        (obj_->ce->default_properties_count ||
         obj_->handlers != &std_object_handlers ||
         GC_IS_RECURSIVE(ptab)));

    list = ptab;
 	return true;
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

zval_mgr
zobj_user::callable()
{
    // no args
    // for call_user_function
    zval_mgr result;

    //callable is method of no object
    zval_own callme (obj_); 

    if (callable_fn(result, callme))
    {
        return result;
    }
    else {
        return zval_own();
    }
}

zval_mgr
zobj_user::callable(zval_user arg1)
{
    zval_mgr callme (obj_);

    zval_mgr   result;
    zval_init  argv;

    ZVAL_COPY_VALUE(&argv, arg1);

    if (callable_fn(result, callme, 1, &argv))
    {
        return result;
    }
    else {
        return zval_own();
    }
}

zval_mgr
zobj_user::callable(zval_user arg1, zval_user arg2)
{
    zval_mgr callme (obj_);

    zval_mgr      result;
    zval_init     argv[2];

    ZVAL_COPY_VALUE(&argv[0], arg1);
    ZVAL_COPY_VALUE(&argv[1], arg2);

    if (callable_fn(result, callme, 2, &argv[0]))
    {
        return result;
    }
    else {
        return zval_own();
    }
}

}; // namespace
#endif
//zobj_user.cpp
