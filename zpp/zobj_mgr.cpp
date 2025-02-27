#ifndef ZOBJ_MGR_CPP
#define ZOBJ_MGR_CPP

#ifndef ZOBJ_MGR_H
#include "zobj_mgr.h"
#endif

#ifndef ZOBJ_USER_H
#include "zobj_user.h"
#endif

namespace zpp {


void //protected
zobj_mgr::own()
{
	if (!obj_)
	{
		return;
	}
	obj_->gc.refcount++;
}

void //protected
zobj_mgr::lose()
{
	if (!obj_) {
		return;
	}
	zend_object_release(obj_);
	obj_ = nullptr;
}


void 
zobj_mgr::adopt(base_d* cobj)
{
	if (obj_)
	{
		lose();
	}
	obj_ = cobj->zobj();
}

int 
zobj_mgr::decref()
{
    int rc = GC_REFCOUNT(obj_)-1;
    zend_object_release(obj_);
    if (!rc)
    {
        obj_ = nullptr;
    }
    return rc;
}


zobj_mgr& 
zobj_mgr::operator=(const zobj_user &rc)
{
	if (rc.obj_ != obj_)
	{
		lose();
		obj_ = rc.obj_;
		own();
	}
	return *this;
}

zobj_mgr& 
zobj_mgr::operator=(zobj_mgr&& rc)
{
    lose();
    obj_ = rc.obj_;
    rc.obj_ = nullptr;
    return *this;
}

const zobj_mgr& 
zobj_mgr::operator=(zend_object* rc)
{
    if (obj_ == rc)
    {
        return *this;
    }
    lose();
    obj_ = rc;
    own();
    return *this;
}

bool //static
zobj_mgr::new_object(zstr_user classname, zobj_mgr& host)
{
	class_data maker(classname);

	return maker.new_object(host);
}

void 
zobj_mgr::move_zv(zval* ret)
{
	if (obj_)
	{
		ZVAL_OBJ(ret, obj_); 
		obj_ = nullptr;// give up ownership privilege
	}
	else {
		ZVAL_NULL(ret);
	}	
}

void 
zobj_mgr::return_zv(zval* ret)
{
	if (obj_)
		ZVAL_OBJ_COPY(ret, obj_);
	else
		ZVAL_NULL(ret);
}

}; //namespace zpp
//zobj_mgr.cpp
#endif