#ifndef WEAK_REF_CPP
#define WEAK_REF_CPP

#ifndef WEAK_REF_H
#include "weak_ref.h"
#endif

namespace zpp {

weak_ref::weak_ref() : obj_rc()
{
}

weak_ref::weak_ref(const obj_ptr& obj) : obj_rc(obj)
{
}

weak_ref::weak_ref(const weak_ref& obj) : obj_rc(obj)
{
}

weak_ref //static
weak_ref::refObject(obj_ptr p)
{
	weak_ref result = weakref_create(p);
	return result;
}

const weak_ref& 
weak_ref::operator=(const obj_ptr& c)
{
	lose();
	obj_ = c.obj_;
	own();
	return *this;
}


const weak_ref& 
weak_ref::operator=(const weak_ref& c)
{
	lose();
	obj_ = c.obj_;
	own();
	return *this;
}

// get referred object (or null?)
obj_rc 
weak_ref::get()
{
	return weakref_get(obj_);
}

} // namespace

#endif
