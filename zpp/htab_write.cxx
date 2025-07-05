#ifndef HTAB_WRITE_CPP
#define HTAB_WRITE_CPP

#ifndef HTAB_WRITE_H
#include "htab_write.h"
#endif


#ifndef HTAB_MGR_H
#include "htab_mgr.h"
#endif

#ifndef ZVAL_MGR_H
#include "zval_mgr.h"
#endif

#ifndef ZOBJ_MGR_H
#include "zobj_mgr.h"
#endif

#ifndef HTAB_WALK_H
#include "htab_walk.h"
#endif

//#define HTAB_SHOW_MEMORY

namespace zpp {

void 
htab_write::giveback(zval* mgr)
{
	//printf("htab_write giveback\n");
	zval_user test(mgr);
	//showmem("giveback zval", test);
	HashTable* h = test.zarray();
	if (!h)
	{
		// try to clean
		zval_mgr::try_decref(mgr);
		*mgr = {0};

		h = zend_new_array(HT_MIN_SIZE);
		#ifdef HTAB_SHOW_MEMORY
		showarray("giveback", h);
		#endif
		//comes with refcount ==1
		
	}
	else 
	{
		htab_mgr::cowop(h);
		#ifdef HTAB_SHOW_MEMORY
		zend_printf("cowop array %lx to zval %lx\n", h, mgr);	
		#endif
	}
	ZVAL_ARR(mgr, h);
	ht_ = h;
}

htab_write::htab_write(htab_mgr& mgr)
{
	// ensure both mgr, and write have same array rc==1
	//printf("htab_write htab_mgr& mgr\n");
	htab_mgr::cowop(mgr.ht_);
	ht_ = mgr.ht_;
}


htab_write::htab_write(zval_mgr& mgr)
{
	giveback(mgr);
}

htab_write::htab_write(zval_user mgr)
{
	giveback(mgr);
}

htab_write::htab_write(HashTable* h)
{
	//printf("htab_write HashTable*\n");
	ht_ = h;
}

htab_write::htab_write(const zval* p)
{
	if (p) {
		ht_ = zval_user((zval*)p).zarray();
	}
	else {
		ht_ = nullptr;
	}
}


void htab_write::push_back(HashTable* t)
{
	zval tmp = {0};
	zval_user::array_bind(&tmp, t);
	if (zend_hash_next_index_insert(ht_, &tmp))
	{
		if (Z_TYPE_FLAGS(tmp) != 0)
			zval_mgr::try_addref(&tmp);
	}
}

void htab_write::push_back(zend_string* zs)
{
	zval tmp = {0};
	zval_user::string_bind(&tmp,zs);

	if (zend_hash_next_index_insert(ht_, &tmp))
	{
		if (Z_TYPE_FLAGS(tmp) != 0) {
			zval_mgr::try_addref(&tmp);
		}
		
	}
}

void htab_write::push_back(zend_object* zo)
{
	zval tmp = {0};
	zval_user::object_bind(&tmp, zo);

	if (zend_hash_next_index_insert(ht_, &tmp))
	{
		zval_mgr::try_addref(&tmp);
	}
}

void htab_write::clear()
{
	if (ht_) {
		//showarray("htab_write clear", ht_);
		zend_hash_clean(ht_);
	}
}

void htab_write::push_back(zval* zv)
{
	if (zend_hash_next_index_insert(ht_, zv))
	{
		if (Z_TYPE_FLAGS_P(zv) != 0)
			zval_mgr::try_addref(zv);
	}
}

void htab_write::push_back(const char* s, std::size_t slen)
{
	zstr_temp temp(s, slen);
	push_back((zend_string*)temp);
}


void 
htab_write::set(zval* key, zval* value)
{
	 zval_user test(key);

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
	 	zval_mgr::try_addref(value);
	 }
}
void htab_write::set(zval* key, zend_string* value)
{
	zval temp = {0};
	zval_user::string_bind(&temp, value);
	set(key, &temp);
}

/*
void 
htab_write::set(zend_string* key, zend_string* value)
{
	zval_own temp(value);
	set(key, temp);
}
*/


void htab_write::set(zend_string* key, HashTable* t)
{
	//showstr("htab_write::set  key", key);

	zval tmp = {0};
	zval_user::array_bind(&tmp, t);
	if (zend_hash_update(ht_, key, &tmp))
	{	 
		 if (Z_REFCOUNTED(tmp)) 
			htab_mgr::try_addref(t);
	}
	//showarray("htab_write::set  HashTable* ", value);
}

void htab_write::set(zend_string* key, zend_object* obj)
{
	zval temp = {0};
	zval_user::object_bind(&temp, obj);

	if (zend_hash_update(ht_, key, &temp))
	{
		if (obj)
			zobj_mgr::try_addref(obj);
	}
}

void htab_write::set(zend_string* key, double value)
{
	zval temp = {0};
	ZVAL_DOUBLE(&temp, value);
	zend_hash_update(ht_, key, &temp);
}

void htab_write::set(zend_string* key, int val)
{
	zval temp = {0};
	ZVAL_LONG(&temp, val);
	zend_hash_update(ht_, key, &temp);
}

void htab_write::setnull(zend_string* key)
{
	zval temp = {0};
	ZVAL_NULL(&temp);
	zend_hash_update(ht_, key, &temp);
}

void htab_write::set(zend_string* key, zval* val)
{
	//showmem("htab_write::set zval*", val);
	if (zend_hash_update(ht_, key, val))
	{
		if (Z_TYPE_FLAGS_P(val) != 0) {
			zval_mgr::try_addref(val);
			//showmem("try_addref zval*", val);
		}
	}	
}

/*
void
htab_write::set(zstr_user key, const zstr_mgr& value)
{
	zval_mgr temp(value);
	update(key, temp);
}
*/

void 
htab_write::set(zend_string* key, zend_string* value)
{
	//showstr("htab_write::set key", key);
	//showstr("htab_write::set value", value);
	zval temp = {0};
	zval_user::string_bind(&temp, value);
	if (zend_hash_update(ht_, key, &temp))
	{
		if (Z_TYPE_FLAGS(temp) != 0)
		{
			zstr_mgr::try_addref(value);
		}
	}
}

void 
htab_write::set_null(zend_string* key)
{
	zval temp = {0};
	ZVAL_NULL(&temp);
	zend_hash_update(ht_, key, &temp);
}

void 
htab_write::set_null(zend_long idx)
{
	zval temp = {0};
	ZVAL_NULL(&temp);
	zend_hash_index_update(ht_, idx, &temp);
}

void htab_write::set_null(zval_user key)
{
	if (key.isLong()) {
		 return set_null(key.zlong());
	}
	else if (key.isString())
	{
		 return set_null(key.zstr());
	}
}

bool htab_write::unset(zval_user key)
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
void htab_write::set(zend_long idx, zval_user value)
{
	update(idx, value);
}
*/

void htab_write::set(zend_long idx, HashTable* value)
{
	zval temp = {0};
	zval_user::array_bind(&temp,value);
	
	if (zend_hash_index_update(ht_, idx, &temp))
	{
		if (Z_TYPE_FLAGS(temp) != 0)
		{
			htab_mgr::try_addref(value);
		}
	}
}

void htab_write::set(zend_long idx, zend_object* value)
{
	zval temp = {0};
	zval_user::object_bind(&temp,value);
	
	if (zend_hash_index_update(ht_, idx, &temp))
	{
		if (Z_TYPE_FLAGS(temp) != 0)
		{
			zval_mgr::try_addref(&temp);
		}
	}
}

bool htab_write::unset(zend_string* skey)
{
	return (zend_hash_del(ht_, skey) == SUCCESS);
}

bool htab_write::unset(zend_long idx)
{
	return (zend_hash_index_del(ht_, idx) == SUCCESS);
}

void htab_write::set(zend_long idx, zval* value)
{	
	if (zend_hash_index_update(ht_, idx, value))
	{
		if (Z_TYPE_FLAGS_P(value) != 0)
		{
			zval_mgr::try_addref(value);
		}
	}
}

void
htab_write::merge(HashTable* src)
{
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
htab_write::removal(htab_read exkeys)
{
	//showarray("exkeys", exkeys);

	htab_walk wk;

	auto exkey = wk.value();

	for(wk.start(exkeys); wk.ok(); wk.next()) 
	{
		//showarray("extract from", ht_);
		//showmem("value for key", exkey);

		zval_user v2 = this->get(exkey);
		
		if (v2.ok()) 
		{
			//showmem("extract value", v2);
			this->unset(exkey);
		}
	}
}

void htab_write::push_back(const zval_mgr& zv) 
{
    push_back((zval*) zv);
}

void htab_write::push_back(int value) 
{
	zval_mgr temp(value);
    push_back((zval*) temp);
}

void htab_write::push_back(zval_user zv) 
{
    push_back((zval*) zv);
}

void htab_write::push_back(zstr_user su)
{
    push_back((zend_string*) su);
}

}; // namespace zpp

#endif