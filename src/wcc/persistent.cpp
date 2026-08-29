#ifndef PERSISTENT_WCC_CPP
#define PERSISTENT_WCC_CPP

#ifndef PERSISTENT_WCC_H
#include "persistent.h"
#endif

#ifndef PERSISTENT_ARGINFO_H
#define PERSISTENT_ARGINFO_H
extern "C" {
	#include "stub/persistent_arginfo.h"
}
#endif

namespace wcc {
	using namespace zpp;

	base_obj_mgr<Persistent> Persistent::omg;

	PersistInit PIRoot;

void PersistInit::init()
{
	root_.init(16);
}


obj_rc //static 
Persistent::get_root()
{
	obj_rc result =  Persistent::omg.new_zobj();

	Persistent* p = zobj_toc<Persistent>(result);

	p->init(&PIRoot.root_);

	return result;
}

Persistent::Persistent() : base_d(), persist_(nullptr) 
{

}

void 
Persistent::init(htab_persist* p) 
{ 
	persist_ = p;
}

void 
Persistent::set(str_ptr key, val_ptr value)
{
	persist_->set(key, value);
}

val_rc 
Persistent::get(str_ptr key)
{
	val_rc result = persist_->get(key);

	return result;
}

}; // namespace wcc

using namespace wcc;
using namespace zpp;


ZEND_METHOD(Wcc_Persistent, root)
{

}

ZEND_METHOD(Wcc_Persistent, get)
{

}

ZEND_METHOD(Wcc_Persistent, set)
{

}



PHP_MINIT_FUNCTION(wcc_persistent_md)
{
	auto ce = register_class_Wcc_Persistent();

	Persistent::omg.classEntry(ce);

	STATE_INIT_ADD(PIRoot);
	
	return SUCCESS;
}


#endif