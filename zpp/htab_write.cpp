#ifndef HTAB_WRITE_CPP
#define HTAB_WRITE_CPP


#ifndef HTAB_MGR_H
#include "htab_mgr.h"
#endif

namespace zpp {

void 
htab_write::giveback(zval* mgr)
{
	zval_user test(mgr);
	HashTable* h = test.zarray();
	if (!h)
	{
		// try to clean
		Z_TRY_DELREF_P(mgr);
		*mgr = {0};
		h = zend_new_array(HT_MIN_SIZE);
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
	ht_ = h;
}

void htab_write::update(zend_long idx, zval* val)
{
	if(zend_hash_index_update(ht_, idx, val))
	{
		Z_TRY_ADDREF(*val);
	};
}

void htab_write::update(zend_string* key, zval* val)
{
	if (zend_hash_update(ht_, key, val))
	{
		Z_TRY_ADDREF(*val);
	}
}

void htab_write::append(zval* pz)
{
	if (zend_hash_next_index_insert(ht_, pz))
	{
		Z_TRY_ADDREF_P(pz);
	};	 
}


bool htab_write::remove(zend_string* skey)
{
	return (zend_hash_del(ht_, skey) == SUCCESS);
}

bool htab_write::remove(zend_long idx)
{
	return (zend_hash_index_del(ht_, idx) == SUCCESS);
}

void htab_write::push_back(HashTable* t)
{
	zval tmp = {0};
	ZVAL_ARR(&tmp, t);
	append(&tmp);
}

void htab_write::push_back(zend_string* zs)
{
	zval tmp = {0};
	ZVAL_STR(&tmp, zs);
	append(&tmp);
}

void htab_write::push_back(zend_object* zo)
{
	zval tmp = {0};
	ZVAL_OBJ(&tmp, zo);
	append(&tmp);
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
	append(zv);
}

void htab_write::push_back(const char* s, std::size_t slen)
{
		zstr_perm w(s, slen);
		push_back((zend_string*)w);
}

void htab_write::push_back(zval_user ptr)
{
		append((zval*)ptr);
}

void htab_write::push_back(const zval_mgr& zo)
{
	  append((zval*)zo);
}
void htab_write::push_back(zstr_user bs)
{
		zval temp = {0};
		ZVAL_STR(&temp, (zend_string*)bs);
		append(&temp);
}
void htab_write::push_back(zobj_user zo)
{
		zval temp = {0};
		ZVAL_OBJ(&temp, (zend_object*)zo);
		append(&temp);
}

void 
htab_write::set(zval_user key, zval_user value)
{
	 if (key.isLong())
	 {
	 		update(key.zlong(), value);
	 }
	 else if (key.isString())
	 {
	 		update(key.zstr(), value);
	 }
}


/*
void 
htab_write::set(zend_string* key, zend_string* value)
{
	zval_own temp(value);
	set(key, temp);
}
*/


void htab_write::set(zend_string* key, HashTable* value)
{
	//showstr("htab_write::set  key", key);
	zval_mgr temp(value);
	update(key, (zval*)temp);
}

void htab_write::set(zend_string* key, zend_object* obj)
{
	zval_mgr value(obj);
	update(key, value);
}

void htab_write::set(zend_string* key, double value)
{
	zval tmp = {0};
	ZVAL_DOUBLE(&tmp, value);
	update(key, &tmp);
}

void htab_write::set(zend_string* key, int val)
{
	//showstr("set zs key", key);
	zval_mgr value(val);
	update(key, value);
}

void htab_write::set(zend_string* key, zval* val)
{
		update(key,val);
}

void 
htab_write::set(zend_string* key, zend_string* value)
{
	//showstr("htab_write::set key", key);
	//showstr("htab_write::set value", value);
	zval_mgr temp(value);
	update(key, temp);
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

void htab_write::set(zend_long idx, zval_user value)
{
	update(idx, value);
}

void htab_write::set(zend_long idx, HashTable* value)
{
		zval temp = {0};
		ZVAL_ARR(&temp, value);
		update(idx, &temp);
}

bool htab_write::unset(zend_string* skey)
{
	return remove(skey);
}

bool htab_write::unset(zend_long idx)
{
	return remove(idx);
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
			 this->append(value);
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