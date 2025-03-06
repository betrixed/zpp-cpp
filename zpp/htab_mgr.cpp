#ifndef HTAB_MGR_CPP
#define HTAB_MGR_CPP

// clean this zval of its reference counted value and reinitialize
#ifndef HTAB_MGR_H
#include "htab_mgr.h"
#endif

namespace zpp {
// Protected static function


void htab_mgr::own()
{
	if (!ht_) return;
	GC_TRY_ADDREF(ht_);
}

HashTable* 
htab_mgr::steal()
{
	HashTable* result = ht_;
	ht_ = nullptr;
	return result;
}

void  
htab_mgr::decref()
{
	if (!ht_ || (ht_->gc.u.type_info & GC_IMMUTABLE))
		return;
	int rc = GC_REFCOUNT(ht_) - 1;
	if (!rc) {
		zend_array_destroy(ht_);
		ht_ = (HashTable*) nullptr;
	}
	else {
		GC_TRY_DELREF(ht_);
	}
}

void htab_mgr::lose()
{
	if (ht_) 
	{
		decref();
		ht_ = (HashTable*) nullptr;
	}
}

htab_mgr::~htab_mgr()
{
	if (ht_) {
		lose();
	}
}

htab_mgr::htab_mgr(HashTable *h)
{
	ht_ = h;
	if (ht_) own();
}

htab_mgr::htab_mgr(zval_mgr&& zw)
{
	HashTable *p = zval_user(zw).zarray();

	if (p != ht_)
	{
		ht_ = p;
		zw.init();
	}
}

htab_mgr::htab_mgr(const zval_mgr& zw)
{
	ht_ = zval_user(zw).zarray();
	if (ht_) own();
}

htab_mgr::htab_mgr(const zval_user& zptr)
{
	ht_ = zptr.zarray();
	if (ht_) own();
}

htab_mgr::htab_mgr(zval* p)
{
	if (p) {
		ht_ = zval_user(p).zarray();
		if (ht_) own();
	}
	else {
		ht_ = nullptr;
	}
}

const htab_mgr& 
htab_mgr::operator=(const zval_mgr& zw)
{
	HashTable* p = zval_user(zw).zarray();

	if (p) 
	{
		if (p == ht_)
			return *this;
		decref();
		ht_ = p;
		own();
	}
	else {
		decref();
		ht_ = (HashTable*) nullptr;
	}
	return *this;
}

const htab_mgr& 
htab_mgr::operator=(const zval_user& zptr)
{
	lose();
	ht_ = zptr.zarray();
	if (ht_) own();
	return *this;
}

htab_mgr& 
htab_mgr::operator=(zval_mgr&& zw)
{
	//showmem("htab_mgr=zw&&", zw);

	HashTable* p = zval_user(zw).zarray();

	if (p != ht_)
	{
		lose();
		zw.init();
		ht_ = p;
	}
	return *this;
}


const htab_mgr& 
htab_mgr::operator=(HashTable* htab)
{
	if (htab == ht_)
	{
		return *this;
	}

	lose();
	ht_ = htab;
	own();

	return *this;
}

HashTable* //static
htab_mgr::new_array()
{
	return zend_new_array(HT_MIN_SIZE);
}

HashTable* //static
htab_mgr::empty_array()
{
	return (HashTable*) &zend_empty_array;
}

void htab_mgr::init()
{
	HashTable* empty = (HashTable*) &zend_empty_array;
	if (ht_ && (ht_ != empty))
	{
		lose();
	}
	ht_ = empty;
	//ht_ = (HashTable*) &zend_empty_array;
}

bool 
htab_mgr::isEmpty() const 
{ 
	return  (!ht_) 
			|| (ht_ == &zend_empty_array) 
			|| (zend_array_count(ht_) == 0); 
}

void htab_mgr::reset()
{
	//zend_printf("reset\n");
	//showarray("reset array", ht_);
	if (ht_) {
		if (GC_REFCOUNT(ht_) > 1) {
			lose();
		}
		else {
			if (zend_array_count(ht_) == 0)
			{
				return;
			}
			lose();
		}
	} 
	//zend_printf("reset init\n");
	init();

}

/** 
 *  A COW helper.
 *  Assumes that return value
 *  will replace ht_ value if cow duplication occurs 
 */



/*
void htab_mgr::cow(const htab_mgr& c)
{
	HashTable* h = c.ht_;
	if (h == ht_)
		return;
	ht_ = h;
	own();
	ht_ = make_own(ht_);
	dbg_dump(__FUNCTION__)
}
*/


htab_mgr::htab_mgr(htab_mgr&& m)
{
	ht_ = m.ht_;
	m.ht_ = nullptr;
}

bool //static
htab_mgr::cowop(HashTable*& inout)
{
	HashTable* used = inout;
	if (used == nullptr) 
	{
		inout = zend_new_array(HT_MIN_SIZE);
		return true;
	}
	if (GC_REFCOUNT(used) > 1) 
	{
	    inout = zend_array_dup(used);
	    GC_TRY_DELREF(used); // safe to decref
	    return true;
	}
	return false;
}

const htab_mgr& 
htab_mgr::operator=(const htab_mgr& c)
{
	if (ht_)
	{
		lose();
	}
	ht_ = c.ht_;
	own();
	return *this;
}

htab_mgr& 
htab_mgr::operator=(htab_mgr&& m)
{
	//zend_printf("operator=(htab_mgr&& m)  ");
	if (ht_)
	{
		lose();
	}
	ht_ = m.ht_;
	m.ht_ = nullptr;
	//TODO: maybe should not need this
	return *this;
}


void htab_mgr::move_zv(zval* return_value)
{
	//showarray("move_zv", ht_);
	ZVAL_ARR(return_value, ht_);
	ht_ = nullptr;
}

const htab_mgr& 
htab_mgr::operator=(zval* zv)
{
	lose();
	ht_ = zval_user(zv).zarray();
	own();
	return *this;
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


htab_empty::htab_empty()
{
	ht_ = htab_mgr::empty_array();
}


}; // namespace
//htab_mgr.cpp
#endif