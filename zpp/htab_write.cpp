#ifndef HTAB_WRITE_CPP
#define HTAB_WRITE_CPP


#ifndef HTAB_MGR_H
#include "htab_mgr.h"
#endif

namespace zpp {

void 
htab_write::giveback(zval* mgr)
{
	//printf("htab_write giveback\n");
	zval_user test(mgr);
	HashTable* h = test.zarray();
	if (!h)
	{
		// try to clean
		zval_mgr::try_decref(mgr);
		*mgr = {0};

		h = zend_new_array(HT_MIN_SIZE);
		//comes with refcount ==1
		ZVAL_ARR(mgr, h);
	}
	else if (htab_mgr::cowop(h))
	{
		//write back with rc == 1
		ZVAL_ARR(mgr, h);
	}
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

/*
zval*
htab_write::update(zend_long idx, zval* val)
{
	return zend_hash_index_update(ht_, idx, val);
}

zval* 
htab_write::update(zend_string* key, zval* val)
{
	return zend_hash_update(ht_, key, val);
}


zval* 
htab_write::append(zval* pz)
{
	return zend_hash_next_index_insert(ht_, pz); 
}


bool htab_write::remove(zend_string* skey)
{
	return (zend_hash_del(ht_, skey) == SUCCESS);
}

bool htab_write::remove(zend_long idx)
{
	return (zend_hash_index_del(ht_, idx) == SUCCESS);
}
*/
void htab_write::push_back(HashTable* t)
{
	zval tmp = {0};
	if (!t)
	{
		ZVAL_NULL(&tmp);
	}
	else if (t == (HashTable*) &zend_empty_array)
	{
		ZVAL_INDIRECT(&tmp, (zval*) zval_mgr::EmptyArray);
	}
	else {
		ZVAL_ARR(&tmp, t);
	}
	if (zend_hash_next_index_insert(ht_, &tmp))
	{
		zval_mgr::try_addref(&tmp);
	}
}

void htab_write::push_back(zend_string* zs)
{
	zval tmp = {0};
	if (zs)
		ZVAL_STR(&tmp, zs);
	else
		ZVAL_NULL(&tmp);

	if (zend_hash_next_index_insert(ht_, &tmp))
	{
		zval_mgr::try_addref(&tmp);
	}
}

void htab_write::push_back(zend_object* zo)
{
	zval tmp = {0};
	if (zo)
		ZVAL_OBJ(&tmp, zo);
	else
		ZVAL_NULL(&tmp);

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
		zval_mgr::try_addref(zv);
	}
}

void htab_write::push_back(const char* s, std::size_t slen)
{
	zstr_temp temp(s, slen);
	push_back((zend_string*)temp);
}

/*
void htab_write::push_back(zval_user ptr)
{
	zval* zv = (zval*) ptr;
	if (zend_hash_next_index_insert(ht_, zv))
	{
		zval_mgr::try_addref(zv);
	}
}


void htab_write::push_back(const zval_mgr& zo)
{
	zval* zv = (zval*) zo;
	if (zend_hash_next_index_insert(ht_, zv))
	{
		zval_mgr::try_addref(zv);
	}
}
*/
/*
void htab_write::push_back(zstr_user bs)
{
		zval temp = {0};
		ZVAL_STR(&temp, (zend_string*)bs);
		append(&temp);
}
*/

/*
void htab_write::push_back(zobj_user zo)
{
		zval temp = {0};
		ZVAL_OBJ(&temp, (zend_object*)zo);
		append(&temp);
}
*/

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
	if (value)
		ZVAL_STR(&temp, value);
	else
		ZVAL_NULL(&temp);

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
	if (!t)
	{
		ZVAL_NULL(&tmp);
	}
	else if (t == (HashTable*) &zend_empty_array)
	{
		showmem("empty array", zval_mgr::EmptyArray);
		ZVAL_INDIRECT(&tmp, (zval*) zval_mgr::EmptyArray);
		showmem("set indirect", &tmp);
	}
	else {
		ZVAL_ARR(&tmp, t);
	}

	if (zend_hash_update(ht_, key, &tmp))
	{
		if (Z_TYPE_P(&tmp) == IS_ARRAY)
			htab_mgr::try_addref(t);
	}
	//showarray("htab_write::set  HashTable* ", value);
}

void htab_write::set(zend_string* key, zend_object* obj)
{
	zval temp = {0};
	if (obj)
		ZVAL_OBJ(&temp, obj);
	else
		ZVAL_NULL(&temp);

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
		zval_mgr::try_addref(val);
		showdata("try_addref ", ht_);
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
	if (value)
		ZVAL_STR(&temp, value);
	else
		ZVAL_NULL(&temp);
	if (zend_hash_update(ht_, key, &temp))
	{
		if (value)
			zstr_mgr::try_addref(value);
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
	if (value)
		ZVAL_ARR(&temp, value);
	else
		ZVAL_NULL(&temp);
	if (zend_hash_index_update(ht_, idx, &temp));
	{
		if (value)
			htab_mgr::try_addref(value);
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



/**
 * This version "pulls out" the key and value
 * from the array, and returns a new array with the
 * extracted key => value found in key list exkeys.
 */
htab_mgr
htab_write::extract(htab_read exkeys)
{

	htab_mgr 		result;

	htab_write merger(result);
	//showarray("exkeys", exkeys);

	htab_walk wk;

	auto exkey = wk.value();

	for(wk.start(exkeys); wk.ok(); wk.next()) 
	{
		//showmem("extract key", exkey);

		zval_user v2 = this->get(exkey);

		if (v2.getStringData()) {
			merger.set(exkey, v2);
			this->unset(exkey);
		}
	}
	//showarray("extract result", result);
	return result;
}



}; // namespace zpp

#endif