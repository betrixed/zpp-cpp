#ifndef REF_RC_CPP
#define REF_RC_CPP

/**
 * @file zpp/ref_rc.cpp
 * @author Michael Rynn <michael.rynn.500@gmail.com>
 * @brief Implementation of ref_rc class.
 * @copyright copyright (c) 2025 Michael Rynn
 */
#ifndef REF_RC_H
#include "ref_rc.h"
#endif

namespace zpp {

void 
ref_rc::lose()
{
	if (!rh_)
	{
		return;
	}

	if (GC_REFCOUNT(rh_) == 1)
	{
		zval_ptr_dtor(&rh_->val);
		efree_size(rh_, sizeof(zend_reference));
	}
	else {
		GC_DELREF(rh_);
	}
	rh_ = nullptr;
}

void 
ref_rc::own()
{
	if (rh_) 
	{
		GC_ADDREF(rh_);
	}
}

void 
ref_rc::create()
{
	// From zend ZVAL_NEW_EMPTY_REF
	rh_ = (zend_reference *) emalloc(sizeof(zend_reference));
	GC_SET_REFCOUNT(rh_, 1);								 
	GC_TYPE_INFO(rh_) = GC_REFERENCE;	
	// ?? Why sources.ptr ??						 
	rh_->sources.ptr = nullptr;	
	zval* zp = &rh_->val;
	*zp = {0};
	ZVAL_NULL(zp);					 
}

ref_rc::~ref_rc()
{
	lose();
}


ref_rc::ref_rc()
{
	create();
}

ref_rc::ref_rc(const ref_rc& c)
{
	rh_ = c.rh_;
	own();
}


ref_rc::ref_rc(ref_rc&& m)
{
	// transfer
	rh_ = m.rh_;
	m.rh_ = nullptr;
}


ref_rc& 
ref_rc::operator=(ref_rc&& m)
{
	zend_reference* ref = m.rh_;
	if (ref == rh_)
	{
		// Still have to strip donor
		m.lose();
	}
	else {
		lose();
		rh_ = ref;
		m.rh_ = nullptr;
	}
	return *this;
}


const ref_rc& 
ref_rc::operator=(const ref_rc& c)
{
	zend_reference* ref = c.rh_;
	if (ref != rh_)
	{
		if (rh_) lose();
		rh_ = ref;
		if (rh_) own();
	}
	return *this;
}

/** 
 * Move assignments give full ownership to zref_,
 *  and nullify their argument.
 * Also the  assign operators, replace existing zval,
 * without changing zref_ rc.
 * 
 * The first assignment creates the zref_.
 * Assigning a null htab_own, will create
 * an empty array.
 * Not so with null zobj_own, because no idea
 * what object class might be appropriate. 
 * Not even stdClass,
 */

ref_rc& 
ref_rc::operator=(htab_rc &&m)
{
	HashTable* ht = m.ptr();
	
	/**
	zval_ptr zptr(&zref_->val);
	if (zptr.isArray() && (ht==zptr.zarray()) {
		// really confused

	}
	*/
	zval* vp = &rh_->val;
	Z_TRY_DELREF_P(vp);
	if (!ht)
	{	// if htab_own didn't have array.
		ht = zend_new_array(0);
	}
	else {
		m.ht_ = nullptr;
	}
	ZVAL_ARR(vp, ht);
	return *this;
}

ref_rc::ref_rc(HashTable* ht)
{
	create();
	ZVAL_ARR(&rh_->val, ht);
}


/*
 * Is this needed?
 */
ref_rc& 
ref_rc::operator=(val_rc &&m)
{
	zval* vp = &rh_->val;

	val_ptr::try_decref(vp);

	//TODO: Not sure about why this
	ZVAL_COPY_VALUE(vp, m);
	ZVAL_NULL(m);
	return *this;

}

ref_rc& 
ref_rc::operator=(obj_rc &&m)
{
	zval* vp = &rh_->val;

	zend_object* obj = m.obj_;
	// ownership by theft
	m.obj_ = nullptr;

	val_ptr::try_decref(vp);

	if (obj) {
		val_ptr::object_bind(vp, obj);
	}
	else {
		ZVAL_NULL(vp);
	}
	return *this;
}


/** Check what is inside
   Value means not array or object.
   It seems redundant to have a reference to an object,
   since zend_object* is own reference.
 */
bool ref_rc::isArray() const
{
	val_ptr test(&rh_->val);
	return test.isArray();
}

int ref_rc::ref_type() const
{
	val_ptr test(&rh_->val);
	return test.ref_type();
}

bool ref_rc::isObject() const
{
	val_ptr test(&rh_->val);
	return test.isObject();
}


/** Pull out the underlying ref counted zval
 */
val_ptr 
ref_rc::value() const
{
	return val_ptr(&rh_->val);
}

htab_ptr 
ref_rc::array() const
{
	val_ptr temp(&rh_->val);
	return temp.zarray();
}

obj_ptr 
ref_rc::object() const
{
	val_ptr temp(&rh_->val);
	return temp.zobject();
}

/** looks weird */
void 
ref_rc::takeback(val_rc& takes)
{
	takes = std::move(val_rc(&rh_->val));
}

/** looks weird */
htab_rc 
ref_rc::take_array()
{
	val_rc ref;

	takeback(ref);

	return htab_rc(std::move(ref));
}

};


#endif
//zref_own.cpp