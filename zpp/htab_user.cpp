#ifndef HTAB_USER_CPP
#define HTAB_USER_CPP

// clean this zval of its reference counted value and reinitialize
#include "htab_user.h"

#ifndef WC_PREG_H
#include "preg.h"
#endif

namespace zpp {

htab_user::htab_user(HashTable* ht) : ht_(ht)
{
}

htab_user::htab_user(const zval* p)
{
	if (p) {
		ht_ = zval_user((zval*)p).zarray();
	}
	else {
		ht_ = nullptr;
	}
}

const htab_user& 
htab_user::operator=(const zval* p)
{
	// htab_user doesn't do reference counting
	if (p) {
		ht_ = zval_user((zval*)p).zarray();
	}
	else {
		ht_ = nullptr;
	}
	return *this;
}

htab_user::htab_user(const zval_mgr& zw)
{
	ht_ = zval_user(zw).zarray();
}
/**
htab_user::htab_user(zval* p)
{
	if (Z_TYPE_P(p) == IS_ARRAY)
	{
		ht_ = 
	}
}
**/

/**
 * This version "pulls out" the key and value
 * from the array, and returns a new array with the
 * extracted key => value found in key list exkeys.
 */
htab_mgr
htab_user::extract(htab_user exkeys)
{

	htab_init result;

	htab_user merger(result);
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


static void check_vcow(HashTable* h)
{
	if ((GC_REFCOUNT(h) > 1) && ((h->u.flags & htab_user::COW_VIOLATE)==0) )
	{
		  //showarray("error: check_vcow", h);
			throw std::runtime_error("htab_user: GC > 1 && no cow");
	}
}



void 
htab_user::apply_all(fn_zval fn)
{
	if (!ht_)
	{
		return;
	}
	zend_hash_apply(ht_, fn);
}

void 
htab_user::cow_violate(bool on)
{
    if (!ht_)
        return;
    if (on)
      ht_->u.flags = (ht_->u.flags | COW_VIOLATE);
    else
      ht_->u.flags = (ht_->u.flags & ~COW_VIOLATE);  
}

zstr_mgr
htab_user::unhive(zstr_user subj)
{
	preg sfind("#@([a-zA-Z][\\w\\d]*)#", preg::OFFSET_CAPTURE, true);

	int ct = sfind.matches(subj);
	if (ct > 0) {
		htab_user m = sfind.results();

		htab_user replace_list = m[(int)0];
		htab_user keys_list = m[1];

		std::string_view original = subj.vstr();

		zstr_buffer result;
		size_t ipos = 0;

		for(int i = 0; i < ct; i++)
		{
			htab_user  k1 = keys_list[i];
			zval_user fkey(k1[(int)0]);

			zval_user rval(get(fkey));

			zend_string* replace_str = rval.zstr();

			htab_user f1 = replace_list[i];
			zval_user  slen_f1(f1[(int)0]);
			zval_user  soffset_f1(f1[1]);

			size_t slen = slen_f1.size();
			zend_long soffset = soffset_f1.zlong();

			if (!replace_str)
			{
				result << original.substr(ipos, soffset-ipos);
			} 
			else {
				result << original.substr(ipos, soffset-ipos);
				result << replace_str;
			}
			ipos = soffset + slen;
		}
		if (ipos < original.size()) {
			result << original.substr(ipos);
		}
		return std::move(result);
	}
	else {
		return zstr_mgr(subj);
	}
}

htab_user::htab_user(zval_user zptr) 
{
	if (zptr.isArray())
	{
		ht_ = zptr.zarray();
	}
	if (ht_)
	{
		return;
	}
	if (!ht_)
	{
		throw std::logic_error("Expected zval_ptr with HashTable");
	}
}

uint32_t 
htab_user::size() const {
	if (!ht_) {
		return 0;
	}
	return zend_array_count(ht_);
}

void htab_user::update(zend_long idx, zval* val)
{
	check_vcow(ht_);
	if(zend_hash_index_update(ht_, idx, val))
	{
		Z_TRY_ADDREF(*val);
	};
}

void htab_user::update(zend_string* key, zval* val)
{
	check_vcow(ht_);
	if (zend_hash_update(ht_, key, val))
	{
		Z_TRY_ADDREF(*val);
	}
}

void htab_user::append(zval* pz)
{
	check_vcow(ht_);
	if (zend_hash_next_index_insert(ht_, pz))
	{
		Z_TRY_ADDREF_P(pz);
	};	 
}


bool htab_user::remove(zend_string* skey)
{
	check_vcow(ht_);
	return (zend_hash_del(ht_, skey) == SUCCESS);
}

bool htab_user::remove(zend_long idx)
{
	check_vcow(ht_);
	return (zend_hash_index_del(ht_, idx) == SUCCESS);
}

void htab_user::push_back(HashTable* t)
{
	zval tmp = {0};
	ZVAL_ARR(&tmp, t);
	append(&tmp);
}

void htab_user::push_back(zend_string* zs)
{
	zval tmp = {0};
	ZVAL_STR(&tmp, zs);
	append(&tmp);
}

void htab_user::push_back(zend_object* zo)
{
	zval tmp = {0};
	ZVAL_OBJ(&tmp, zo);
	append(&tmp);
}

void htab_user::clear()
{
	if (ht_) {
		//showarray("htab_user clear", ht_);
		zend_hash_clean(ht_);
	}
}

void htab_user::push_back(zval* zv)
{
	append(zv);
}

void htab_user::push_back(const char* s, std::size_t slen)
{
		zstr_perm w(s, slen);
		push_back((zend_string*)w);
}

void htab_user::push_back(zval_user ptr)
{
		append((zval*)ptr);
}

void htab_user::push_back(const zval_mgr& zo)
{
	  append((zval*)zo);
}
void htab_user::push_back(zstr_user bs)
{
		zval temp = {0};
		ZVAL_STR(&temp, (zend_string*)bs);
		append(&temp);
}
void htab_user::push_back(zobj_user zo)
{
		zval temp = {0};
		ZVAL_OBJ(&temp, (zend_object*)zo);
		append(&temp);
}

void 
htab_user::set(zval_user key, zval_user value)
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
htab_user::set(zend_string* key, zend_string* value)
{
	zval_own temp(value);
	set(key, temp);
}
*/


void htab_user::set(zend_string* key, HashTable* value)
{

	zval_mgr temp(value);
	//showmem("htab_user::set htab_own&", temp);
	update(key, temp);
}

void htab_user::set(zend_string* key, zend_object* obj)
{
	zval_mgr value(obj);
	update(key, value);
}

void htab_user::set(zend_string* key, double value)
{
	zval tmp = {0};
	ZVAL_DOUBLE(&tmp, value);
	update(key, &tmp);
}

void htab_user::set(zend_string* key, int val)
{
	//showstr("set zs key", key);
	zval_mgr value(val);
	update(key, value);
}

void htab_user::set(zend_string* key, zval* val)
{
		update(key,val);
}

void 
htab_user::set(zend_string* key, zend_string* value)
{
	//showstr("htab_user::set key", key);
	//showstr("htab_user::set value", value);
	zval_mgr temp(value);
	update(key, temp);
}
/*
zstr_mgr
htab_user::print_all(const char* label)
{
	htab_walk walk;
	zstr_buffer  ss;

	auto key = walk.key();
	auto value = walk.value();
	if (!label) {
		label = "array";
	}
	ss << label << " : [";

	for(walk.start(ht_) ; walk.ok(); walk.next())
	{
		zstr_mgr skey(key.to_zstr());
		zstr_mgr vkey(value.to_zstr());
		ss << skey.vstr() << "=>" << vkey.vstr() << ", \n";
	}
	ss << "],\n";
	return zstr_mgr(std::move(ss));
}
*/


zval* 
htab_user::get(zend_long idx) const
{
	return zend_hash_index_find(ht_, idx);
}

zval*  
htab_user::get(zend_string* zkey) const
{
	return zend_hash_find(ht_, zkey);
}

zval*  
htab_user::get(const std::string_view& key) const
{
	//zend_printf("get:string_view %s %d\n", key.data(), key.size());

	zstr_temp skey(key.data(), key.size());
	//showstr("get string_view&", skey);

	return zend_hash_find(ht_, skey);
}

zval* 
htab_user::get(const char* key) const
{
	zstr_temp skey(key);
	return zend_hash_find(ht_, skey);
}

zval* 
htab_user::get(zval_user key) const
{
	return get(key);
}

zval* 
htab_user::get(zval* key) const
{
	uint8_t ktype = Z_TYPE_P(key);

	if (ktype == IS_STRING) {
		return zend_hash_find(ht_, Z_STR_P(key));
	}
	else if (ktype == IS_LONG) {
		return zend_hash_index_find(ht_, Z_LVAL_P(key));
	}
	//* TODO: exception?
	return nullptr;
}

bool htab_user::try_fetch(zend_long key, zval_user& store) const
{
	zval* temp = zend_hash_index_find(ht_, key);
	if (temp) 
	{
		store = temp;
		return true;
	}
	return false;
}

bool htab_user::has_index(zend_long key) const
{
	return (zend_hash_index_find(ht_, key) != nullptr);
}

bool htab_user::try_fetch(zval_user key, zval_user& store) const
{
	if (key.isString())
	{
		return try_fetch(key.zstr(), store);
	}
	else if (key.isLong())
	{
		return try_fetch(key.zlong(), store);
	}
	return false;
}


bool htab_user::try_fetch(zend_string* key, zval_user& store) const
{
	//showstr("try_fetch zs key", key);

	zval* temp = zend_hash_find(ht_, key);
	if (temp) {
		//showmem("found ", temp);
		store = temp;
		
		//showmem("copied ", store);
		return true;
	}
	//zend_printf("not found\n");
	return false;
}


bool htab_user::unset(zval_user key)
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


void htab_user::set(zend_long idx, HashTable* value)
{
		zval temp = {0};
		ZVAL_ARR(&temp, value);
		update(idx, &temp);
}

bool htab_user::unset(zend_string* skey)
{
	return remove(skey);
}

bool htab_user::unset(zend_long idx)
{
	return remove(idx);
}


void htab_user::return_zv(zval* return_value) const
{
	 ZVAL_ARR(return_value, ht_);
	 Z_TRY_ADDREF_P(return_value);
}

//* return indexed array of values
htab_mgr
htab_user::getValues()
{
	 htab_init result;
	 htab_user merge(result);

	 htab_walk wk;
	 auto val = wk.value();
	 for(wk.start(ht_); wk.ok(); wk.next())
	 {
	 		merge.append(val);
	 }
	 return result;
}

//* return indexed array of keys
htab_mgr 
htab_user::getKeys()
{
	 htab_init result;
	 htab_user merge;

	 htab_walk wk;
	 auto val = wk.key();
	 for(wk.start(ht_); wk.ok(); wk.next())
	 {
	 		merge.append(val);
	 }
	 return result;
}

void
htab_user::merge(HashTable* src)
{

	check_vcow(src);
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

};

#endif