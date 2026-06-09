#ifndef HTAB_RW_CPP
#define HTAB_RW_CPP

/**
 * @file zpp/htab_rw.cxx
 * @author Michael Rynn <michael.rynn.500@gmail.com>
 * @brief htab_rw - read/write HashTable manager
 * @copyright Copyright (c) 2025
 * @license BSD 3-Clause License
 * 
 * @details This class provides read and write access to a HashTable*.
 * It ensures that the HashTable is unique (copy-on-write) before any write operation.
 * It can be constructed from various sources, including htab_rc, val_rc, val_ptr, zval*, or HashTable*.
 * It provides methods to modify the HashTable, such as push_back, set, unset, clear, merge, etc.
 * It does not manage the reference count of the HashTable, which is done by the donor class.
 *  */

#ifndef HTAB_RW_H
#include "zpp/htab_rw.h"
#endif


#ifndef HTAB_RC_H
#include "htab_rc.h"
#endif

#ifndef VAL_RC_H
#include "val_rc.h"
#endif

#ifndef OBJ_RC_H
#include "obj_rc.h"
#endif

#ifndef HTAB_WALK_H
#include "htab_walk.h"
#endif

//#define HTAB_SHOW_MEMORY

namespace zpp {



void 
htab_rw::giveback(zval* mgr, size_t init)
{
	//printf("htab_rw giveback\n");
	val_ptr test(mgr);
	//showmem("giveback zval", test);

	int ztype = test.ztype();
	HashTable* h;

	switch(ztype)
	{
	case IS_ARRAY:
		{
			h = test.zarray();
			if (htab_rc::cowop(h, init))
			{	// replace original with cow.
				val_ptr::array_bind(mgr, h);
			}
			ht_ = h;
		}
		break;
	case IS_REFERENCE:
		{	
			// yank copy referenced HashTable*
			// assume by intention rc == 1 for writing
			// No need to give back
			ht_ = test.zarray();
		}
		break;
	case IS_NULL:
		{
			ht_ = zend_new_array(init);//rc==1
			val_ptr::array_bind(mgr, ht_); // keep rc==1
		}
		break;
	default:
		//* some value??
		//* treat as if (array) $cast
		{	

			val_rc temp(std::move(*mgr)); //move value.
			*mgr = {0};
			ht_ = zend_new_array(init);//rc==1
			htab_rw wt(ht_); //loan 
			wt.push_back(temp);
			val_ptr::array_bind(mgr, ht_); // keep rc==1
		}
		break;
	}
}

htab_rw::htab_rw(htab_rc& mgr, size_t init)
{
	// ensure both mgr, and write have same array rc==1
	//printf("htab_rw htab_rc& mgr\n");
	htab_rc::cowop(mgr.ht_, init);
	ht_ = mgr.ht_;
}


htab_rw::htab_rw(val_rc& mgr, size_t init)
{
	giveback(mgr, init);
}

htab_rw::htab_rw(val_ptr mgr, size_t init)
{
	giveback(mgr, init);
}

htab_rw::htab_rw(HashTable* h)
{
	//printf("htab_rw HashTable*\n");
	ht_ = h;
}

htab_rw::htab_rw(zval* p, size_t init)
{
	giveback(p, init);
}


void htab_rw::push_back(HashTable* val)
{
	zval tmp = {0};
	bool refct = val_ptr::array_bind(&tmp, val);
	//showdata("push_back", val);
	if (zend_hash_next_index_insert(ht_, &tmp))
	{
		if (refct) GC_ADDREF(val);
	}
}

void htab_rw::push_back(zend_string* zs)
{
	zval tmp = {0};
	bool refct = val_ptr::string_bind(&tmp,zs);

	if (zend_hash_next_index_insert(ht_, &tmp))
	{
		if (refct) GC_ADDREF(zs);	
	}
}

void htab_rw::push_back(zend_object* zo)
{
	zval tmp = {0};
	bool refct = val_ptr::object_bind(&tmp, zo);

	if (zend_hash_next_index_insert(ht_, &tmp))
	{
		if (refct) GC_ADDREF(zo);
	}
}

void htab_rw::clear()
{
	if (size()) {
		//showarray("htab_rw clear", ht_);
		zend_hash_clean(ht_);
	}
}

void htab_rw::push_back(zval* zv)
{
	if (zend_hash_next_index_insert(ht_, zv))
	{
		if (Z_TYPE_FLAGS_P(zv) != 0)
			val_ptr::try_addref(zv);
	}
}

void htab_rw::push_back(const char* s, std::size_t slen)
{
	str_temp temp(s, slen);
	push_back((zend_string*)temp);
}


void 
htab_rw::set(zval* key, zval* value)
{
	 val_ptr test(key);

	 zval* result = nullptr;
	 if (test.isLong())
	 {
		result = zend_hash_index_update(ht_, test.zlong(), value);
	 }
	 else if (test.isString())
	 {
	 	result = zend_hash_update(ht_, test.zstr(), value);
	 }
	 else {
	 	//TODO:??
	 }
	 if (result)
	 {
	 	val_ptr::try_addref(value);
	 }
}
void htab_rw::set(zval* key, zend_string* value)
{
	zval temp = {0};
	val_ptr::string_bind(&temp, value);
	set(key, &temp);
}

/*
void 
htab_rw::set(zend_string* key, zend_string* value)
{
	zval_own temp(value);
	set(key, temp);
}
*/


void htab_rw::set(zend_string* key, HashTable* ht)
{
	//showstr("htab_rw::set  key", key);

	zval tmp = {0};
	bool refct = val_ptr::array_bind(&tmp, ht);
	if (zend_hash_update(ht_, key, &tmp))
	{	 
		if (refct) GC_ADDREF(ht);
	}
	//showarray("htab_rw::set  HashTable* ", value);
}

void htab_rw::set(zend_string* key, zend_object* obj)
{
	zval temp = {0};
	bool refct = val_ptr::object_bind(&temp, obj);

	if (zend_hash_update(ht_, key, &temp))
	{
		if (obj && refct)
			GC_ADDREF(obj);
	}
}

void htab_rw::set(zend_string* key, double value)
{
	zval temp = {0};
	ZVAL_DOUBLE(&temp, value);
	zend_hash_update(ht_, key, &temp);
}

void htab_rw::set(zend_string* key, int val)
{
	zval temp = {0};
	ZVAL_LONG(&temp, val);
	zend_hash_update(ht_, key, &temp);
}

void htab_rw::setnull(zend_string* key)
{
	zval temp = {0};
	ZVAL_NULL(&temp);
	zend_hash_update(ht_, key, &temp);
}

void 
htab_rw::setbool(zend_string* key, bool value)
{
	zval temp = {0};
	ZVAL_BOOL(&temp, value);
	zend_hash_update(ht_, key, &temp);
}
void htab_rw::set(zend_string* key, zval* val)
{
	//showmem("htab_rw::set zval*", val);
	if (zend_hash_update(ht_, key, val))
	{
		if (Z_TYPE_FLAGS_P(val) != 0) {
			val_ptr::try_addref(val);
			//showmem("try_addref zval*", val);
		}
	}	
}

void 
htab_rw::set(zend_string* key, zend_string* value)
{
	zval temp = {0};
	bool refct = val_ptr::string_bind(&temp, value);
	if (zend_hash_update(ht_, key, &temp))
	{
		if (refct) GC_ADDREF(value);
	}
}

void htab_rw::set(zend_string* key, const char* value, size_t vlen)
{
	str_rc temp(value, vlen);
	set(key, temp);
}

void 
htab_rw::set_null(zend_string* key)
{
	zval temp = {0};
	ZVAL_NULL(&temp);
	zend_hash_update(ht_, key, &temp);
}

void 
htab_rw::set_null(zend_long idx)
{
	zval temp = {0};
	ZVAL_NULL(&temp);
	zend_hash_index_update(ht_, idx, &temp);
}

void htab_rw::set_null(val_ptr key)
{
	if (key.isLong()) {
		 return set_null(key.zlong());
	}
	else if (key.isString())
	{
		 return set_null(key.zstr());
	}
}

bool htab_rw::unset(val_ptr key)
{
	if (key.isLong()) {
		 return unset(key.zlong());
	}
	else if (key.isString())
	{
		 return unset(key.zstr());
	}
	return false;
}

/*
void htab_rw::set(zend_long idx, val_ptr value)
{
	update(idx, value);
}
*/

void htab_rw::set(zend_long idx, HashTable* ht)
{
	zval temp = {0};
	bool refct = val_ptr::array_bind(&temp,ht);
	
	if (zend_hash_index_update(ht_, idx, &temp))
	{
		if (refct) GC_ADDREF(ht);
	}
}

void htab_rw::set(zend_long idx, zend_object* value)
{
	zval temp = {0};
	bool refct = val_ptr::object_bind(&temp,value);
	
	if (zend_hash_index_update(ht_, idx, &temp))
	{
		if (refct) GC_ADDREF(value);
	}
}

void htab_rw::set(zend_long idx, zend_string* value)
{
	zval temp = {0};
	val_ptr::string_bind(&temp, value);
	zend_hash_index_update(ht_, idx, &temp);
}


bool htab_rw::unset(zend_string* skey)
{
	return (zend_hash_del(ht_, skey) == SUCCESS);
}

bool htab_rw::unset(zend_long idx)
{
	return (zend_hash_index_del(ht_, idx) == SUCCESS);
}

void htab_rw::set(zend_long idx, zval* value)
{	
	if (zend_hash_index_update(ht_, idx, value))
	{
		if (Z_TYPE_FLAGS_P(value) != 0)
		{
			val_ptr::try_addref(value);
		}
	}
}

int
htab_rw::merge(HashTable* src)
{
	//static int test = 1;

	/* if (test)
	{
		zend_printf("<pre>\n");
		test = 0;
	}
	*/
	
	int oldsize = size();
	//showdata("\nbefore merge", ht_);
	php_array_merge(ht_, src);
	//showdata("\nafter merge", ht_);
	int newsize = size();
	return (newsize - oldsize);
}

val_rc 
htab_rw::pop()
{
	zval*     val = nullptr;
	val_rc    result;
	uint32_t  idx;
	zend_long nextFree;

	if (ht_) 
	{
		if (HT_IS_PACKED(ht_))
		{
			idx = ht_->nNumUsed;
			while(idx > 0)
			{
				if (idx == 0)
				{
					goto BAIL_OUT;
				}
				idx--;
				val = ht_->arPacked + idx;
				if (Z_TYPE_P(val) != IS_UNDEF) {
					break;
				}
			}
			ZVAL_COPY_VALUE(result, val);
			ZVAL_UNDEF(val);
			nextFree = ht_->nNextFreeElement - 1;
			if (idx == nextFree)
			{
				ht_->nNextFreeElement = nextFree;
			}
			zend_hash_packed_del_val(ht_, val);
		}
		else {
			Bucket *p;
			idx = ht_->nNumUsed;
			while (1) 
			{
				if (idx == 0) {
					goto BAIL_OUT;
				}
				idx--;
				p = ht_->arData + idx;
				val = &p->val;
				if (Z_TYPE_P(val) != IS_UNDEF) {
					break;
				}
			}
			ZVAL_COPY_VALUE(result, val);
			ZVAL_UNDEF(val);
			nextFree = ht_->nNextFreeElement - 1;
			if (!p->key && ((zend_long)p->h == nextFree))
			{
				ht_->nNextFreeElement = nextFree;
			}
			zend_hash_del_bucket(ht_, p);
		}
		// in case it points to deleted value?
		zend_hash_internal_pointer_reset(ht_);
	}
	// zend code unwraps any reference result here. Is this necessary?
BAIL_OUT:
	return result;

}




void
htab_rw::removal(htab_ptr exkeys)
{
	//showarray("exkeys", exkeys);

	htab_walk wk;

	auto exkey = wk.value();

	for(wk.start(exkeys); wk.ok(); wk.next()) 
	{
		//showarray("extract from", ht_);
		//showmem("value for key", exkey);

		val_ptr v2 = this->get(exkey);
		
		if (v2.ok()) 
		{
			//showmem("extract value", v2);
			this->unset(exkey);
		}
	}
}

void htab_rw::push_back(const val_rc& zv) 
{
    push_back((zval*) zv);
}

void htab_rw::push_back(int value) 
{
	val_rc temp(value);
    push_back((zval*) temp);
}

void htab_rw::push_back(val_ptr zv) 
{
    push_back((zval*) zv);
}

void htab_rw::push_back(str_ptr su)
{
    push_back((zend_string*) su);
}

//- *@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*

void 
htab_persist::val_destroy(zval* val)
{
	/*showmem("val_destroy", val);
	if (!val_ptr(val).isString())
	{
		val_rc::try_decref(val);
	}*/
}

htab_persist::~htab_persist()
{
	if (htab_)
	{
		/* 
		zend_hash_graceful_destroy(htab_);
		
		htab_persist::freehtmemory(htab_);
		free(htab_);
		*/
		htab_ = nullptr;
	}
}

void 
htab_persist::init(size_t slots)
{

	_zend_hash_init(htab_, slots, htab_persist::val_destroy, 1);
}

void
htab_persist::wipe()
{
	htab_ = nullptr;
}

htab_persist::htab_persist()
{
	htab_ = &data_;
}

htab_persist::htab_persist(size_t slots)
{
	htab_ = &data_;
	init(slots);
}


/*
void  
htab_persist::set(val_ptr key, val_ptr value)
{
	if (key.isLong())
	{
		set(key.zlong(), value);
	}
	else {
		set(key.zstr(), value);
	}
}

 void 
 htab_persist::set(zend_long ix, val_ptr value)
 {
 	htab_rw rw(htab_);

	int ztype = value.ref_type();
	switch(ztype)
	{
	case IS_TRUE:
	case IS_FALSE:
	case IS_LONG:
	case IS_DOUBLE:
		rw.set(ix, value);
		break;
	case IS_STRING:
		{
			str_ptr s = value.zstr();
			str_perm sp (s.data(), s.size());
			rw.set(ix, sp);
		}
	case IS_ARRAY:
		{
			htab_ptr h (value.zarray());

			htab_persist  hp(h.size());

			htab_walk wk;

			auto subkey = wk.key();
			auto subval = wk.value();
			for(wk.start(h); wk.ok(); wk.next())
			{
				hp.set(subkey, subval);
			}
			rw.set(ix, (HashTable*) hp);
			hp.wipe();
		}
	}
 }
 
 void 
 htab_persist::set(str_ptr key, val_ptr value)
 {
	htab_rw rw(htab_);

	if (!key.interned())
	{
		str_intern skey(key.data(), key.size());
		key = skey;
	}
	

	int ztype = value.ref_type();
	switch(ztype)
	{
	case IS_TRUE:
	case IS_FALSE:
	case IS_LONG:
	case IS_DOUBLE:
		rw.set(key, value);
		break;
	case IS_STRING:
		{
			str_ptr s = value.zstr();
			if (!s.interned())
			{
				str_intern sp (s.data(), s.size());
				s = sp;
			}
			rw.set(key, s);
		}
	case IS_ARRAY:
		{
			htab_ptr h (value.zarray());

			htab_persist  hp(h.size());

			htab_walk wk;

			auto subkey = wk.key();
			auto subval = wk.value();
			for(wk.start(h); wk.ok(); wk.next())
			{
				if (subkey.isString())
					hp.set(subkey.zstr(), subval);
				else {
					hp.set(subkey.zlong(), subval);
				}
			}
			rw.set(key, (HashTable*) hp);
			hp.wipe();
		}
	}

 }


 void 
 htab_persist::freehtmemory(HashTable* ht)
{
	bool persistent = GC_FLAGS(ht) & IS_ARRAY_PERSISTENT;
	void* ptr = HT_GET_DATA_ADDR(ht);
	//zend_printf("Free HashTable Data %lx, persistent=%d\n", ptr, persistent);

	pefree(ptr, persistent);
}
*/
}; // namespace zpp
#endif