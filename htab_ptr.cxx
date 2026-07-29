#ifndef htab_ptr_CPP
#define htab_ptr_CPP

/**
 * @file zpp/htab_ptr.cxx
 * @author Michael Rynn <michael.rynn.500@gmail.com>
 * @brief htab_ptr - non-owning pointer to HashTable
 * @copyright Copyright (c) 2025
 * @license BSD 3-Clause License
 * 
 */

#ifndef STR_RC_H
#include "str_rc.h"
#endif


#ifndef HTAB_PTR_H
#include "htab_ptr.h"
#endif

#ifndef HTAB_RW_H
#include "htab_rw.h"
#endif

#ifndef WC_PREG_H
#include "preg.h"
#endif

#ifndef HTAB_WALK_H
#include "htab_walk.h"
#endif

#ifndef STR_BUF_H
#include "str_buf.h"
#endif

namespace zpp {

htab_ptr::htab_ptr(HashTable* ht) : ht_(ht)
{
}

htab_ptr::htab_ptr(const zval* p)
{
	if (p) {
		ht_ = val_ptr((zval*)p).zarray();
	}
	else {
		ht_ = nullptr;
	}
}

const htab_ptr& 
htab_ptr::operator=(const zval* p)
{
	// htab_ptr doesn't do reference counting
	if (p) {
		ht_ = val_ptr((zval*)p).zarray();
	}
	else {
		ht_ = nullptr;
	}
	return *this;
}

htab_ptr::htab_ptr(const val_rc& zw)
{
	ht_ = val_ptr(zw).zarray();
}
/**
htab_ptr::htab_ptr(zval* p)
{
	if (Z_TYPE_P(p) == IS_ARRAY)
	{
		ht_ = 
	}
}
**/

bool  
htab_ptr::has_key(zend_string* skey) const
{
    if (!ht_ || !skey) {
            return false;
    }
    return (get(skey) != nullptr);
}

void 
htab_ptr::apply_all(fn_zval fn)
{
	if (!ht_)
	{
		return;
	}
	zend_hash_apply(ht_, fn);
}

 str_rc 
 htab_ptr::print_kv(const char* label) const
 {
 	str_rc result;

 	if (!ht_)
		return result;
	htab_walk walk;
	str_buf  ss;

	auto key = walk.key();
	auto value = walk.value();
	if (!label) {
		label = "array";
	}
	ss << label << " : [";

	for(walk.start(ht_) ; walk.ok(); walk.next())
	{
		str_rc skey(key.to_zstr());
		str_rc vkey(value.to_zstr());
		ss << str_ptr(skey) << " => " << str_ptr(vkey) << ", \n";
	}
	ss << "],\n";
	result.adopt(ss.zstr());
	return result;
}

str_rc
htab_ptr::unhive(str_ptr subj)
{
	str_rc result;

	if (!ht_)
		return result;

	preg sfind("#@([a-zA-Z][\\w\\d]*)#", preg::OFFSET_CAPTURE, true);

	int ct = sfind.matches(subj);
	if (ct > 0) {
		htab_ptr m = sfind.results();

		htab_ptr replace_list = m[(int)0];
		htab_ptr keys_list = m[1];

		std::string_view original = subj.vstr();

		str_buf buf;
		size_t ipos = 0;

		for(int i = 0; i < ct; i++)
		{
			htab_ptr  k1 = keys_list[i];
			val_ptr fkey(k1[(int)0]);

			val_ptr rval(get(fkey));

			zend_string* replace_str = rval.zstr();

			htab_ptr f1 = replace_list[i];
			val_ptr  slen_f1(f1[(int)0]);
			val_ptr  soffset_f1(f1[1]);

			size_t slen = slen_f1.size();
			zend_long soffset = soffset_f1.zlong();

			if (!replace_str)
			{
				buf << original.substr(ipos, soffset-ipos);
			} 
			else {
				buf << original.substr(ipos, soffset-ipos);
				buf << replace_str;
			}
			ipos = soffset + slen;
		}
		if (ipos < original.size()) 
		{
			buf << original.substr(ipos);
		}
		result = buf.zstr();
		return result;
	}
	else {
		result = subj;
		return result;
	}
}


uint32_t 
htab_ptr::size() const {
	if (!ht_) {
		return 0;
	}
	return zend_array_count(ht_);
}


/*
str_rc
htab_ptr::print_all(const char* label)
{
	htab_walk walk;
	str_buf  ss;

	auto key = walk.key();
	auto value = walk.value();
	if (!label) {
		label = "array";
	}
	ss << label << " : [";

	for(walk.start(ht_) ; walk.ok(); walk.next())
	{
		str_rc skey(key.to_zstr());
		str_rc vkey(value.to_zstr());
		ss << skey.vstr() << "=>" << vkey.vstr() << ", \n";
	}
	ss << "],\n";
	return str_rc(std::move(ss));
}
*/


zval* 
htab_ptr::get(zend_long idx) const
{
	if (!ht_)
		return nullptr;
	return zend_hash_index_find(ht_, idx);
}

zval*  
htab_ptr::get(zend_string* zkey) const
{
	zval* result = nullptr;
	if (ht_) {
		result =  zend_hash_find(ht_, zkey);
	}
	return result;
}

zval* htab_ptr::get(zend_string* zkey, val_ptr adef) const
{
	zval* result = get(zkey);
	if (!result)
	{	
		result = (zval*) adef;
	}
	return result;
}

zval*  
htab_ptr::get(const std::string_view& key) const
{
	if (!ht_)
		return nullptr;
	str_rc skey(key.data(), key.size());

	return zend_hash_find(ht_, skey);
}

zval* 
htab_ptr::get(const char* key) const
{
	if (!ht_) {
		return nullptr;
	}
	str_rc skey(key);
	return zend_hash_find(ht_, skey);
}
/*
zval* 
htab_ptr::get(val_ptr key) const
{
	if (!ht_)
		return nullptr;
	return get((zval*)key);
}
*/

zval* 
htab_ptr::get(zval* key) const
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

bool htab_ptr::try_fetch(zend_long key, val_ptr& store) const
{
	if (!ht_)
		return false;
	zval* temp = zend_hash_index_find(ht_, key);
	if (temp) 
	{
		store = temp;
		return true;
	}
	else {
		store.init();
	}
	return false;
}

bool htab_ptr::has_index(zend_long key) const
{
	if (!ht_)
		return false;
	return (zend_hash_index_find(ht_, key) != nullptr);
}

bool htab_ptr::try_fetch(zval* key, val_ptr& store) const
{
	if (!ht_)
		return false;
	val_ptr test(key);
	if (test.isString())
	{
		return try_fetch(test.zstr(), store);
	}
	else if (test.isLong())
	{
		return try_fetch(test.zlong(), store);
	}
	store.init();
	return false;
}


bool htab_ptr::try_fetch(zend_string* key, val_ptr& store) const
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
	else {
		store.init();
	}
	//zend_printf("not found\n");
	return false;
}

htab_rc 
htab_ptr::slice(int offset, int length, bool preserve_keys)
{
	htab_rc result_mgr;
	htab_rw hw(result_mgr);

	auto src_len = size();

	if (offset < 0) {
		offset = src_len + offset;
	}

	unsigned int pstart = offset;
	if (length == 0)
	{
		length = src_len;
	}
	else if (length < 0) 
	{
		length = -length;
		if ((unsigned int) length < src_len)
		{
			src_len -= length;
		}
	}
	else {
		src_len = length;
	}

	if ((unsigned int)offset > src_len)
	{
		return htab_ptr::empty_array();
	}

	htab_walk wk;
	unsigned int pos = 0;
	auto key = wk.key();
	auto value = wk.value();

	val_rc new_key;

	for(wk.start(ht_); wk.ok(); wk.next(), pos++)
	{

	  if ((pos >= pstart) && (pos < src_len))
	  {
	  	int newpos = pos - pstart;
	  	if (key.isLong())
	  	{
	  		if (preserve_keys)
	  		{
	  			new_key = key;
	  		}
	  		else {
	  			new_key = (zend_long) newpos;	
	  		}
	  	}
	  	else {
			new_key = key;
	  	}
	  	hw.set(new_key, value);
	  }	
	}
	return result_mgr;
}

void htab_ptr::copy_zv(zval* return_value) const
{
	 val_ptr(return_value).bind_array(ht_);
}


// logic is from zend code for array_is_list
// All keys must be integer from 0 and in sequence
bool
htab_ptr::is_list() const
{
	zend_long expected_idx = 0;

	/* Empty arrays are lists regardless of pack status*/
	if (size()==0) {
		return true;
	}
	
	if (HT_IS_PACKED(ht_)) {
		if (HT_IS_WITHOUT_HOLES(ht_)) {
			return true;
		}
		for_key_value ikv;

		for(ikv.start(ht_); ikv.ok(); ikv.next())
		{
			if (expected_idx != ikv.index())
			{
				// found a hole!
				return false;
			}
			expected_idx++;
		}
	}
	else {

		for_key_value ikv;

		for(ikv.start(ht_); ikv.ok(); ikv.next())
		{
			if ((ikv.key() != nullptr) || (expected_idx != ikv.index()))
			{
				// found a sequence error or string key!
				return false;
			}
			expected_idx++;
		}
	}
	return true;
}

int
htab_ptr::value_index(str_ptr fvalue) const
{
	for_key_value ikv;

	for (ikv.start(ht_); ikv.ok(); ikv.next())
	{
	    val_ptr test(ikv.value());
	    if (!test.isString())
	    {
	    	continue;
	    }
	    if (zs_cmp(test.zstr(), fvalue) == 0)
	    {
	    	return ikv.index();
	    }
	}
	return -1;
}

zval* 
htab_ptr::operator[]  (const val_rc& key) const
        {
                //zend_printf("[zval_own&]\n");
                return get((zval*)key);
        }

zval* 
htab_ptr::operator[]  (str_ptr skey) const
{
        //zend_printf("[zstr_ptr&]\n");
        return get( (zend_string*) skey);
}
//* return indexed array of values

HashTable* 
htab_ptr::empty_array()
{
	return (HashTable*) &zend_empty_array;
}


htab_ptr 
htab_ptr::globals()
{
	return htab_ptr(&EG(symbol_table));
}

//! static, get (or not) from _GLOBALS table
zval*  
htab_ptr::get_global(str_ptr key)
{

	return (zend_hash_find_ind(&EG(symbol_table), key));

}

void 
htab_ptr::set_global(str_ptr key, zval* value)
{
	// pre-emptive try reference count boost
    val_ptr::try_addref(value); 
    str_rc::try_addref(key);    
    
    // make it exist in $GLOBALS
    zend_symtable_update_ind(&EG(symbol_table), key, value);
}

};

#endif