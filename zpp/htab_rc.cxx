#ifndef HTAB_RC_CPP
#define HTAB_RC_CPP

// clean this zval of its reference counted value and reinitialize
#ifndef HTAB_RC_H
#include "htab_rc.h"
#endif

#ifndef VAL_RC_H
#include "val_rc.h"
#endif

#ifndef STR_RC_H
#include "str_rc.h"
#endif

#ifndef HTAB_WALK_H
#include "htab_walk.h"
#endif

#ifndef HTAB_RW_H
#include "htab_rw.h"
#endif

//#define HTAB_SHOW_MEMORY

namespace zpp {
// Protected static function
//! static, set value in _GLOBALS table
void 
htab_rc::set_global(str_ptr key, val_ptr value)
{
	// pre-emptive try reference count boost
	val_rc::try_addref(value); 
    str_rc::try_addref(key);    
    
    // make it exist in $GLOBALS
    zend_symtable_update_ind(&EG(symbol_table), key, value);
}

//! static, get (or not) from _GLOBALS table
val_ptr  
htab_rc::get_global(str_ptr key)
{

	val_ptr result = val_ptr(zend_hash_find_ind(&EG(symbol_table), key));
	//showmem("get_global", result);
	return result;
}

void //static
htab_rc::try_addref(HashTable *h)
{
	if (!h)
		return;
	if (h->gc.u.type_info & GC_IMMUTABLE)
    {
        return;
    }
    h->gc.refcount++;
}


bool  //static
htab_rc::try_decref(HashTable* h)
{
	if (!h || (h->gc.u.type_info & GC_IMMUTABLE))
	{
		return false;
	}
	int rct =  h->gc.refcount-1;
	if (!rct) {
		#ifdef HTAB_SHOW_MEMORY
		showarray("destroy", h);
		#endif
		/* zend_hash_destroy did 
		   not do a complete job 
		*/
		zend_array_destroy(h);
		return true;
	}
	h->gc.refcount--;
	return false;
}

void htab_rc::own()
{
	if (!ht_) return;
	htab_rc::try_addref(ht_);
}

HashTable* 
htab_rc::steal()
{
	HashTable* result = ht_;
	ht_ = nullptr;
	return result;
}

void 
htab_rc::adopt(HashTable *h)
{
	if (ht_)
		lose();
	ht_ = h;
}


void htab_rc::lose()
{
	if (ht_) 
	{
		try_decref(ht_);

		ht_ = (HashTable*) nullptr;
	}
}

htab_empty::~htab_empty()
{
	//zend_printf("~htab_empty %lx\n", ht_);
}
htab_rc::~htab_rc()
{
	//zend_printf("~htab_rc  %lx\n", ht_);
	if (ht_) {
		lose();
	}
}

htab_rc::htab_rc(HashTable *h)
{
	ht_ = h;
	if (ht_) own();
}

htab_rc::htab_rc(val_rc&& zw)
{
	HashTable *p = val_ptr(zw).zarray();

	if (p != ht_)
	{
		ht_ = p;
		zw.init();
	}
}

htab_rc::htab_rc(const val_rc& zw)
{
	ht_ = val_ptr(zw).zarray();
	if (ht_) own();
}

htab_rc::htab_rc(const val_ptr& zptr)
{
	ht_ = zptr.zarray();
	if (ht_) own();
}


htab_rc::htab_rc(zval* p)
{
	if (p) {
		ht_ = val_ptr(p).zarray();
		if (ht_) own();
	}
	else {
		ht_ = nullptr;
	}
}

htab_rc::htab_rc(const htab_rc& c)
{
	ht_ = c.ht_;
	if (ht_)
	{
		own();
	}
}


const htab_rc& 
htab_rc::operator=(const val_rc& zw)
{
	HashTable* p = val_ptr(zw).zarray();

	if (p) 
	{
		if (p == ht_)
			return *this;
		if (ht_)
			try_decref(ht_);
		ht_ = p;
		own();
	}
	else {
		if (ht_)
			try_decref(ht_);
		ht_ = (HashTable*) nullptr;
	}
	return *this;
}

const htab_rc& 
htab_rc::operator=(const val_ptr& zptr)
{
	lose();
	ht_ = zptr.zarray();
	if (ht_) own();
	return *this;
}

htab_rc& 
htab_rc::operator=(val_rc&& zw)
{
	//showmem("htab_rc=zw&&", zw);

	HashTable* p = val_ptr(zw).zarray();

	if (p != ht_)
	{
		lose();
		zw.init();
		ht_ = p;
	}
	return *this;
}


const htab_rc& 
htab_rc::operator=(HashTable* htab)
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
htab_rc::new_array()
{
	return zend_new_array(HT_MIN_SIZE);
}

HashTable* //static
htab_rc::empty_array()
{
	return (HashTable*) &zend_empty_array;
}

void htab_rc::init()
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
htab_rc::isEmpty() const 
{ 
	return  (!ht_) 
			|| (ht_ == &zend_empty_array) 
			|| (zend_array_count(ht_) == 0); 
}

void htab_rc::reset()
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
void htab_rc::cow(const htab_rc& c)
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


htab_rc::htab_rc(htab_rc&& m)
{
	ht_ = m.ht_;
	m.ht_ = nullptr;
}

bool //static
htab_rc::cowop(HashTable*& inout)
{
	//printf("cowop& %lx\n", &inout);
	HashTable* used = inout;
	if ( (used == nullptr)
	   ||(used == const_cast<HashTable*>(&zend_empty_array)))
	{
		//zend_printf("New Array\n");
		HashTable* newht = htab_rc::new_array();
		#ifdef HTAB_SHOW_MEMORY
		showarray("new array", newht);
		#endif
		inout = newht;
		return true;
	}
	if (GC_REFCOUNT(used) > 1) 
	{
		// updates not allowed, 
		// make a copy with rc == 1
		inout = zend_array_dup(used);
		#ifdef HTAB_SHOW_MEMORY
		zend_printf("ARRAY DUPLICATE %lx of ", inout);
		showarray("used", used);
		#endif	
		htab_rc::try_decref(used);
	    return true;
	}
	return false;
}

const htab_rc& 
htab_rc::operator=(const htab_rc& c)
{
	if (ht_)
	{
		lose();
	}
	ht_ = c.ht_;
	own();
	return *this;
}

htab_rc& 
htab_rc::operator=(htab_rc&& m)
{
	//zend_printf("operator=(htab_rc&& m)  ");
	if (ht_)
	{
		lose();
	}
	ht_ = m.ht_;
	m.ht_ = nullptr;
	//TODO: maybe should not need this
	return *this;
}


void htab_rc::move_zv(zval* return_value)
{
	//showarray("move_zv", ht_);
	val_ptr::array_bind(return_value, ht_);
	ht_ = nullptr;
}

const htab_rc& 
htab_rc::operator=(zval* zv)
{
	lose();
	ht_ = val_ptr(zv).zarray();
	own();
	return *this;
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


htab_empty::htab_empty()
{
	ht_ = htab_rc::empty_array();
}


htab_rc
htab_rc::getValues(htab_rd hr)
{
	 htab_rc result;
	 if (!hr.size())
		return result;
	 htab_rw merge(result);

	 htab_walk wk;
	 auto val = wk.value();
	 for(wk.start(hr); wk.ok(); wk.next())
	 {
	 		merge.push_back(val);
	 }
	 return result;
}

//* return indexed array of keys
htab_rc 
htab_rc::getKeys(htab_rd hr)
{
	 htab_rc result;
	 if (!hr.size())
		return result;
	 htab_rw merge(result);

	 htab_walk wk;
	 auto val = wk.key();
	 for(wk.start(hr); wk.ok(); wk.next())
	 {
	 		merge.push_back(val);
	 }
	 return result;
}


/**
 * This version "pulls out" the key and value
 * from hfrom array, and returns a new array with the
 * extracted key => value found in key list exkeys.
 */
htab_rc //static
htab_rc::extract(htab_rd exkeys, htab_rw hfrom)
{

	htab_rc result;

	if (!hfrom.size())
	{
		return result;
	}

	htab_rw merger(result);
	//showarray("exkeys", exkeys);

	htab_walk wk;

	auto exkey = wk.value();

	for(wk.start(exkeys); wk.ok(); wk.next()) 
	{
		//showarray("extract from", ht_);
		//showmem("value for key", exkey);

		val_ptr v2 = hfrom.get(exkey);
		
		if (v2.ok())
		{
			//showmem("extract value", v2);
			merger.set(exkey, v2);
			hfrom.unset(exkey);
		}
	}
	//showarray("extract result", result);
	return result;
}

/**
 * This version "pulls out" the key and value
 * from hfrom array, and returns a new array with the
 * extracted key => value found in key list exkeys.
 */
htab_rc //static
htab_rc::subset(htab_rd exkeys, htab_rd hfrom, bool nullmiss)
{

	htab_rc result;

	if (!hfrom.size())
	{
		return result;
	}

	htab_rw merger(result);
	//showarray("exkeys", exkeys);

	htab_walk wk;

	auto exkey = wk.value();

	for(wk.start(exkeys); wk.ok(); wk.next()) 
	{
		//showarray("extract from", ht_);
		//showmem("value for key", exkey);

		val_ptr v2 = hfrom.get(exkey);
		
		if (v2.ok())
		{
			//showmem("extract value", v2);
			merger.set(exkey, v2);
		}
		else if (nullmiss)
		{
			merger.set_null(exkey);
		}
	}
	//showarray("extract result", result);
	return result;
}

/**
 * get list of values found by exkeys in hfrom. 
 * Null in list indicates not found.
 */
htab_rc //static
htab_rc::sublist(htab_rd exkeys, htab_rd hfrom)
{

	htab_rc result;

	if (!hfrom.size())
	{
		return result;
	}

	htab_rw vlist(result);
	//showarray("exkeys", exkeys);

	htab_walk wk;

	auto exkey = wk.value();
	for(wk.start(exkeys); wk.ok(); wk.next()) 
	{

		val_ptr v2 = hfrom.get(exkey);
		vlist.push_back(v2);
	}
	return result;
}

}; // namespace
//htab_rc.cpp
#endif