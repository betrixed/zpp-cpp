#ifndef HTAB_RW_CPP
#define HTAB_RW_CPP

#ifndef HTAB_RW_H
#include "htab_rw.h"
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
	HashTable* h = test.zarray();
	if (!h)
	{
		// try to clean
		val_rc::try_decref(mgr);
		*mgr = {0};

		h = zend_new_array(init);
		/* #ifdef HTAB_SHOW_MEMORY
		showarray("giveback", h);
		#endif
		*/
		//comes with refcount ==1
		
	}
	else 
	{
		htab_rc::cowop(h);
		/*
		#ifdef HTAB_SHOW_MEMORY
		zend_printf("cowop array %lx to zval %lx\n", h, mgr);	
		#endif
		*/
	}
	ZVAL_ARR(mgr, h);
	ht_ = h;
}

htab_rw::htab_rw(htab_rc& mgr)
{
	// ensure both mgr, and write have same array rc==1
	//printf("htab_rw htab_rc& mgr\n");
	htab_rc::cowop(mgr.ht_);
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

htab_rw::htab_rw(const zval* p)
{
	if (p) {
		ht_ = val_ptr((zval*)p).zarray();
	}
	else {
		ht_ = nullptr;
	}
}


void htab_rw::push_back(HashTable* t)
{
	zval tmp = {0};
	val_ptr::array_bind(&tmp, t);
	if (zend_hash_next_index_insert(ht_, &tmp))
	{
		if (Z_TYPE_FLAGS(tmp) != 0)
			val_rc::try_addref(&tmp);
	}
}

void htab_rw::push_back(zend_string* zs)
{
	zval tmp = {0};
	val_ptr::string_bind(&tmp,zs);

	if (zend_hash_next_index_insert(ht_, &tmp))
	{
		if (Z_TYPE_FLAGS(tmp) != 0) {
			val_rc::try_addref(&tmp);
		}
		
	}
}

void htab_rw::push_back(zend_object* zo)
{
	zval tmp = {0};
	val_ptr::object_bind(&tmp, zo);

	if (zend_hash_next_index_insert(ht_, &tmp))
	{
		val_rc::try_addref(&tmp);
	}
}

void htab_rw::clear()
{
	if (ht_) {
		//showarray("htab_rw clear", ht_);
		zend_hash_clean(ht_);
	}
}

void htab_rw::push_back(zval* zv)
{
	if (zend_hash_next_index_insert(ht_, zv))
	{
		if (Z_TYPE_FLAGS_P(zv) != 0)
			val_rc::try_addref(zv);
	}
}

void htab_rw::push_back(const char* s, std::size_t slen)
{
	zstr_temp temp(s, slen);
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
	 	val_rc::try_addref(value);
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


void htab_rw::set(zend_string* key, HashTable* t)
{
	//showstr("htab_rw::set  key", key);

	zval tmp = {0};
	val_ptr::array_bind(&tmp, t);
	if (zend_hash_update(ht_, key, &tmp))
	{	 
		 if (Z_REFCOUNTED(tmp)) 
			htab_rc::try_addref(t);
	}
	//showarray("htab_rw::set  HashTable* ", value);
}

void htab_rw::set(zend_string* key, zend_object* obj)
{
	zval temp = {0};
	val_ptr::object_bind(&temp, obj);

	if (zend_hash_update(ht_, key, &temp))
	{
		if (obj)
			obj_rc::try_addref(obj);
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

void htab_rw::set(zend_string* key, zval* val)
{
	//showmem("htab_rw::set zval*", val);
	if (zend_hash_update(ht_, key, val))
	{
		if (Z_TYPE_FLAGS_P(val) != 0) {
			val_rc::try_addref(val);
			//showmem("try_addref zval*", val);
		}
	}	
}

/*
void
htab_rw::set(str_ptr key, const str_rc& value)
{
	val_rc temp(value);
	update(key, temp);
}
*/

void 
htab_rw::set(zend_string* key, zend_string* value)
{
	//showstr("htab_rw::set key", key);
	//showstr("htab_rw::set value", value);
	zval temp = {0};
	val_ptr::string_bind(&temp, value);
	if (zend_hash_update(ht_, key, &temp))
	{
		if (Z_TYPE_FLAGS(temp) != 0)
		{
			str_rc::try_addref(value);
		}
	}
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

void htab_rw::set(zend_long idx, HashTable* value)
{
	zval temp = {0};
	val_ptr::array_bind(&temp,value);
	
	if (zend_hash_index_update(ht_, idx, &temp))
	{
		if (Z_TYPE_FLAGS(temp) != 0)
		{
			htab_rc::try_addref(value);
		}
	}
}

void htab_rw::set(zend_long idx, zend_object* value)
{
	zval temp = {0};
	val_ptr::object_bind(&temp,value);
	
	if (zend_hash_index_update(ht_, idx, &temp))
	{
		if (Z_TYPE_FLAGS(temp) != 0)
		{
			val_rc::try_addref(&temp);
		}
	}
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
			val_rc::try_addref(value);
		}
	}
}

void
htab_rw::merge(HashTable* src)
{
	//showarray("src ,merge", src);
	htab_walk w;
	auto key = w.key();
	auto value = w.value();

	for(w.start(src); w.ok(); w.next())
	{
		//showmem("Key: ", key);
		//showmem("Value: ", value);
		if (key.isLong())
		{
			 this->push_back(value);
		}
		else {
			 this->set(key, value);
		}
	}

	//showarray("post ,merge", ht_);
	return;
}





void
htab_rw::removal(htab_rd exkeys)
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

}; // namespace zpp

#endif