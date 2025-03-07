#ifndef ZOBJ_MGR_CPP
#define ZOBJ_MGR_CPP

#ifndef ZOBJ_MGR_H
#include "zobj_mgr.h"
#endif

#ifndef ZOBJ_USER_H
#include "zobj_user.h"
#endif

namespace zpp {


void
zobj_mgr::try_addref(zend_object* ob)
{
	ob->gc.refcount++;
}

bool
zobj_mgr::try_delref(zend_object* ob)
{
	int rct = ob->gc.refcount - 1;
	zend_object_release(ob);
	return !(rct);	
}

void zobj_mgr::own()
{
	if (!obj_)
	{
		return;
	}
	try_addref(obj_);
}

void //protected
zobj_mgr::lose()
{
	if (!obj_) {
		return;
	}
	try_delref(obj_);
	obj_ = nullptr;
}

void
zobj_mgr::init()
{
	if (obj_)
	{
		lose();
	}	
}

void 
zobj_mgr::adopt(zend_object *zo)
{
	lose();
	obj_ = zo;
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

zobj_mgr::zobj_mgr(zval_mgr&& m)
{
	obj_ = zval_user(m).zobject();
	m.init();
}

zobj_mgr& 
zobj_mgr::operator=(zval_mgr&& rc)
{
	lose();
	obj_ = zval_user(rc).zobject();
	rc.init();
	return *this;
}

const zobj_mgr& 
zobj_mgr::operator=(zval* rc)
{
	lose();
	obj_ = zval_user(rc).zobject();
	return *this;
}

}; //namespace zpp
//zobj_mgr.cpp
#endif