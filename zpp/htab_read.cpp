#ifndef HTAB_READ_CPP
#define HTAB_READ_CPP

#ifndef ZSTR_MGR_H
#include "zstr_mgr.h"
#endif


#ifndef HTAB_READ_H
#include "htab_read.h"
#endif

#ifndef HTAB_WRITE_H
#include "htab_write.h"
#endif

#ifndef WC_PREG_H
#include "preg.h"
#endif


namespace zpp {

htab_read::htab_read(HashTable* ht) : ht_(ht)
{
}

htab_read::htab_read(const zval* p)
{
	if (p) {
		ht_ = zval_user((zval*)p).zarray();
	}
	else {
		ht_ = nullptr;
	}
}

const htab_read& 
htab_read::operator=(const zval* p)
{
	// htab_read doesn't do reference counting
	if (p) {
		ht_ = zval_user((zval*)p).zarray();
	}
	else {
		ht_ = nullptr;
	}
	return *this;
}

htab_read::htab_read(const zval_mgr& zw)
{
	ht_ = zval_user(zw).zarray();
}
/**
htab_read::htab_read(zval* p)
{
	if (Z_TYPE_P(p) == IS_ARRAY)
	{
		ht_ = 
	}
}
**/

bool  
htab_read::has_key(zend_string* skey) const
{
    if (!ht_ || !skey) {
            return false;
    }
    return (get(skey) != nullptr);
}

void 
htab_read::apply_all(fn_zval fn)
{
	if (!ht_)
	{
		return;
	}
	zend_hash_apply(ht_, fn);
}

 zstr_mgr 
 htab_read::print_kv(const char* label) const
 {
 	zstr_mgr result;

 	if (!ht_)
		return result;
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
		ss << zstr_user(skey) << " => " << zstr_user(vkey) << ", \n";
	}
	ss << "],\n";
	result.adopt(ss.zstr());
	return result;
}



zstr_mgr
htab_read::unhive(zstr_user subj)
{
	zstr_mgr result;

	if (!ht_)
		return result;

	preg sfind("#@([a-zA-Z][\\w\\d]*)#", preg::OFFSET_CAPTURE, true);

	int ct = sfind.matches(subj);
	if (ct > 0) {
		htab_read m = sfind.results();

		htab_read replace_list = m[(int)0];
		htab_read keys_list = m[1];

		std::string_view original = subj.vstr();

		zstr_buffer result;
		size_t ipos = 0;

		for(int i = 0; i < ct; i++)
		{
			htab_read  k1 = keys_list[i];
			zval_user fkey(k1[(int)0]);

			zval_user rval(get(fkey));

			zend_string* replace_str = rval.zstr();

			htab_read f1 = replace_list[i];
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
		result = subj;
		return result;
	}
}

htab_read::htab_read(zval_user zptr) 
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
htab_read::size() const {
	if (!ht_) {
		return 0;
	}
	return zend_array_count(ht_);
}


/*
zstr_mgr
htab_read::print_all(const char* label)
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
htab_read::get(zend_long idx) const
{
	if (!ht_)
		return nullptr;
	return zend_hash_index_find(ht_, idx);
}

zval*  
htab_read::get(zend_string* zkey) const
{
	if (!ht_)
		return nullptr;
	return zend_hash_find(ht_, zkey);
}

zval*  
htab_read::get(const std::string_view& key) const
{
	//zend_printf("get:string_view %s %d\n", key.data(), key.size());
	if (!ht_)
		return nullptr;
	zstr_temp skey(key.data(), key.size());
	//showstr("get string_view&", skey);

	return zend_hash_find(ht_, skey);
}

zval* 
htab_read::get(const char* key) const
{
	if (!ht_)
		return nullptr;
	zstr_temp skey(key);
	return zend_hash_find(ht_, skey);
}
/*
zval* 
htab_read::get(zval_user key) const
{
	if (!ht_)
		return nullptr;
	return get((zval*)key);
}
*/

zval* 
htab_read::get(zval* key) const
{
	if (!ht_)
		return nullptr;
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

bool htab_read::try_fetch(zend_long key, zval_user& store) const
{
	if (!ht_)
		return false;
	zval* temp = zend_hash_index_find(ht_, key);
	if (temp) 
	{
		store = temp;
		return true;
	}
	return false;
}

bool htab_read::has_index(zend_long key) const
{
	if (!ht_)
		return false;
	return (zend_hash_index_find(ht_, key) != nullptr);
}

bool htab_read::try_fetch(zval_user key, zval_user& store) const
{
	if (!ht_)
		return false;
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


bool htab_read::try_fetch(zend_string* key, zval_user& store) const
{
	//showstr("try_fetch zs key", key);
	if (!ht_)
		return false;
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



void htab_read::return_zv(zval* return_value) const
{
	 zval_user(return_value).bind_array(ht_);
}

//* return indexed array of values
htab_mgr
htab_read::getValues()
{
	 htab_mgr result;
	 if (!ht_)
		return result;
	 htab_write merge(result);

	 htab_walk wk;
	 auto val = wk.value();
	 for(wk.start(ht_); wk.ok(); wk.next())
	 {
	 		merge.push_back(val);
	 }
	 return result;
}

//* return indexed array of keys
htab_mgr 
htab_read::getKeys()
{
	 htab_mgr result;
	 if (!ht_)
		return result;
	 htab_write merge(result);

	 htab_walk wk;
	 auto val = wk.key();
	 for(wk.start(ht_); wk.ok(); wk.next())
	 {
	 		merge.push_back(val);
	 }
	 return result;
}

};

#endif