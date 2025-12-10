#ifndef WEAK_REF_H
#define WEAK_REF_H

#ifndef OBJ_RC_H
#include "obj_rc.h"
#endif

namespace zpp {

class weak_ref : public obj_rc {
public:

	weak_ref();
	weak_ref(const obj_ptr&);
	weak_ref(const weak_ref& obj);

	// use this to make a new WeakReference
	static weak_ref refObject(obj_ptr p);

	const weak_ref& operator=(const weak_ref& c);

	const weak_ref& operator=(const obj_ptr& c);

	// get referred object (or null?)
	obj_rc get();
};

} // namespace

#endif