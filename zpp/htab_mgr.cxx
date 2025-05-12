#ifndef HTAB_MGR_CPP
#define HTAB_MGR_CPP

// clean this zval of its reference counted value and reinitialize
#ifndef HTAB_MGR_H
#include "htab_mgr.h"
#endif

#ifndef ZVAL_MGR_H
#include "zval_mgr.h"
#endif

#ifndef ZSTR_MGR_H
#include "zstr_mgr.h"
#endif

#ifndef HTAB_WALK_H
#include "htab_walk.h"
#endif

#ifndef HTAB_WRITE_H
#include "htab_write.h"
#endif

//#define HTAB_SHOW_MEMORY

namespace zpp {
// Protected static function
//! static, set value in _GLOBALS table
void 
htab_mgr::set_global(zstr_user key, zval_user value)
{
	// pre-emptive try reference count boost
	zval_mgr::try_addref(value); 
    zstr_mgr::try_addref(key);    
    
    // make it exist in $GLOBALS
    zend_symtable_update_ind(&EG(symbol_table), key, value);
}

//! static, get (or not) from _GLOBALS table
zval_user  
htab_mgr::get_global(zstr_user key)
{

	zval_user result = zval_user(zend_hash_find_ind(&EG(symbol_table), key));
	//showmem("get_global", result);
	return result;
}

void //static
htab_mgr::try_addref(HashTable *h)
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
htab_mgr::try_decref(HashTable* h)
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

void htab_mgr::own()
{
	if (!ht_) return;
	htab_mgr::try_addref(ht_);
}

HashTable* 
htab_mgr::steal()
{
	HashTable* result = ht_;
	ht_ = nullptr;
	return result;
}

void 
htab_mgr::adopt(HashTable *h)
{
	if (ht_)
		lose();
	ht_ = h;
}


void htab_mgr::lose()
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
htab_mgr::~htab_mgr()
{
	//zend_printf("~htab_mgr  %lx\n", ht_);
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

htab_mgr::htab_mgr(const htab_mgr& c)
{
	ht_ = c.ht_;
	if (ht_)
	{
		own();
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
	//printf("cowop& %lx\n", &inout);
	HashTable* used = inout;
	if ( (used == nullptr)
	   ||(used == const_cast<HashTable*>(&zend_empty_array)))
	{
		//zend_printf("New Array\n");
		HashTable* newht = htab_mgr::new_array();
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
		htab_mgr::try_decref(used);
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
	zval_user::array_bind(return_value, ht_);
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


htab_mgr
htab_mgr::getValues(htab_read hr)
{
	 htab_mgr result;
	 if (!hr.size())
		return result;
	 htab_write merge(result);

	 htab_walk wk;
	 auto val = wk.value();
	 for(wk.start(hr); wk.ok(); wk.next())
	 {
	 		merge.push_back(val);
	 }
	 return result;
}

//* return indexed array of keys
htab_mgr 
htab_mgr::getKeys(htab_read hr)
{
	 htab_mgr result;
	 if (!hr.size())
		return result;
	 htab_write merge(result);

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
htab_mgr //static
htab_mgr::extract(htab_read exkeys, htab_write hfrom)
{

	htab_mgr result;

	if (!hfrom.size())
	{
		return result;
	}

	htab_write merger(result);
	//showarray("exkeys", exkeys);

	htab_walk wk;

	auto exkey = wk.value();

	for(wk.start(exkeys); wk.ok(); wk.next()) 
	{
		//showarray("extract from", ht_);
		//showmem("value for key", exkey);

		zval_user v2 = hfrom.get(exkey);
		
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
htab_mgr //static
htab_mgr::subset(htab_read exkeys, htab_read hfrom, bool nullmiss)
{

	htab_mgr result;

	if (!hfrom.size())
	{
		return result;
	}

	htab_write merger(result);
	//showarray("exkeys", exkeys);

	htab_walk wk;

	auto exkey = wk.value();

	for(wk.start(exkeys); wk.ok(); wk.next()) 
	{
		//showarray("extract from", ht_);
		//showmem("value for key", exkey);

		zval_user v2 = hfrom.get(exkey);
		
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

}; // namespace
//htab_mgr.cpp
#endif