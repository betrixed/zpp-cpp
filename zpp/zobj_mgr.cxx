#ifndef ZOBJ_MGR_CPP
#define ZOBJ_MGR_CPP

#ifndef ZOBJ_MGR_H
#include "zobj_mgr.h"
#endif

#ifndef ZOBJ_USER_H
#include "zobj_user.h"
#endif

#ifndef CLASS_DATA_H
#include "class_data.h"
#endif



namespace zpp {


void
zobj_mgr::try_addref(zend_object* ob)
{
	ob->gc.refcount++;
}

bool // static
zobj_mgr::try_decref(zend_object* ob)
{
	auto rct = GC_REFCOUNT(ob);
	if (rct==1) 
	{
		//showobj("RELEASE obj", ob);
		zend_object_release(ob);
		return true;
	}
	else {
		GC_DELREF(ob);
		//showobj("ROAMING obj", ob);
	}
	return false;	
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
	try_decref(obj_);
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
	//showobj("Adopted", obj_);
}

zobj_mgr::zobj_mgr(base_d* cobj) : zobj_user(cobj)
{
	if (obj_)
	{
		try_addref(obj_);
	}
}

zobj_mgr::zobj_mgr(zend_object* rc) : zobj_user(rc)
{
    own();
}

zobj_mgr::zobj_mgr(const zobj_mgr& rc) : zobj_user(rc.obj_)
{
    own();
}


zobj_mgr::zobj_mgr(zobj_mgr&& rc) : zobj_user(rc.obj_)
{
    rc.obj_ = nullptr;
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
		//Z_TYPE_FLAGS_P(ret) = 0; // Not allowed to dereference
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
	{
		ZVAL_OBJ_COPY(ret, obj_);
		//Z_TYPE_FLAGS_P(ret) = 0;
	}
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
	own();
	return *this;
}

zobj_mgr& 
zobj_mgr::operator=(const zval_user& zv)
{
	lose();
	obj_ = zv.zobject();
	own();
	//showobj("ZVAL_USER& ", obj_);
	return *this;
}

}; //namespace zpp
//zobj_mgr.cpp
#endif