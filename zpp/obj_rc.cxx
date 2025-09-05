#ifndef OBJ_RC_CPP
#define OBJ_RC_CPP

#ifndef OBJ_RC_H
#include "obj_rc.h"
#endif

#ifndef OBJ_PTR_H
#include "obj_ptr.h"
#endif

#ifndef CLASS_DATA_H
#include "class_data.h"
#endif



namespace zpp {

void //static
obj_rc::try_decref(zend_object* ob)
{
	if (!ob || (GC_FLAGS(ob) & GC_IMMUTABLE))
    {
        return;
    }
	auto& rct = ob->gc.refcount;
	if (rct==1) 
	{
		zend_object_release(ob);
		return;
	}
	rct--;	
}

void //static
obj_rc::try_addref(zend_object* ob)
{
	if (!ob || (GC_FLAGS(ob) & GC_IMMUTABLE))
    {
        return;
    }
    ob->gc.refcount++;
}

void obj_rc::own()
{
	if (!obj_)
	{
		return;
	}
	try_addref(obj_);
}

void //protected
obj_rc::lose()
{
	if (!obj_) {
		return;
	}
	try_decref(obj_);
	obj_ = nullptr;
}

void
obj_rc::init()
{
	if (obj_)
	{
		lose();
	}	
}

void 
obj_rc::adopt(zend_object *zo)
{
	lose();
	obj_ = zo;
	//showobj("Adopted", obj_);
}

obj_rc::obj_rc(base_d* cobj) : obj_ptr(cobj)
{
	if (obj_)
	{
		try_addref(obj_);
	}
}

obj_rc::obj_rc(zend_object* rc) : obj_ptr(rc)
{
    own();
}

obj_rc::obj_rc(const obj_ptr& rc) : obj_ptr(rc.obj_)
{
    own();
}

obj_rc::obj_rc(const obj_rc& rc) : obj_ptr(rc.obj_)
{
    own();
}

obj_rc::obj_rc(const zval* zp)
{
	obj_ = val_ptr(zp).zobject();
	own();
}

obj_rc::obj_rc(obj_rc&& rc) : obj_ptr(rc.obj_)
{
    rc.obj_ = nullptr;
}

obj_rc& 
obj_rc::operator=(const obj_ptr &rc)
{
	if (rc.obj_ != obj_)
	{
		lose();
		obj_ = rc.obj_;
		own();
	}
	return *this;
}

obj_rc& 
obj_rc::operator=(const obj_rc &rc)
{
	if (rc.obj_ != obj_)
	{
		lose();
		obj_ = rc.obj_;
		own();
	}
	return *this;
}


obj_rc& 
obj_rc::operator=(obj_rc&& rc)
{
    lose();
    obj_ = rc.obj_;
    rc.obj_ = nullptr;
    return *this;
}

const obj_rc& 
obj_rc::operator=(zend_object* rc)
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
obj_rc::new_object(str_ptr classname, obj_rc& host)
{
	class_data maker(classname);

	return maker.new_object(host);
}

// return must do a move
void 
obj_rc::return_zv(zval* ret)
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
obj_rc::move_zv(zval* ret)
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


obj_rc::obj_rc(val_rc&& m) 
{
	obj_ = val_ptr(m).zobject();
	m.init();
}

obj_rc& 
obj_rc::operator=(val_rc&& rc)
{
	lose();
	obj_ = val_ptr(rc).zobject();
	rc.init();
	return *this;
}

const obj_rc& 
obj_rc::operator=(const zval* rc)
{
	lose();
	//showmem("operator= const zval*", (zval*)rc);
	obj_ = val_ptr(rc).zobject();
	//showobj("obj_", obj_);
	own();
	//showobj("obj_", obj_);
	return *this;
}

obj_rc& 
obj_rc::operator=(const val_ptr& zv)
{
	lose();
	obj_ = zv.zobject();
	own();
	
	return *this;
}

}; //namespace zpp
//obj_rc.cpp
#endif