#ifndef WC_PREG_CPP
#define WC_PREG_CPP

#ifndef WC_PREG_H
#include "wc_preg.h"
#endif


extern "C" {
	#include <ext/standard/php_string.h>
};

namespace zpp {


zval_mgr explode(zstr_user sep,  zstr_user  split, long limit)
{
	zval_mgr  list;
	list.new_array();

	php_explode(sep, split, list, limit);

	return list;
}

zval_mgr implode(zstr_user sep, htab_read arr)
{
	zval_mgr result;

	php_implode(sep, arr, result);

	return result;
}

/** Return a list of unique values, from 2 arrays.
 * manage lists of names separated by space characters, eg class attributes
 * as array lists. Ensure only one instance of each name.
 * This merge 'filters' names through a keyed array. */
zval_mgr
union_values(htab_read list1, htab_read list2)
{
	htab_init keyset;
	htab_write ks(keyset);

	zval_mgr one(1);

	htab_walk wk;
	// use values as keys
	auto key = wk.value();

	for(wk.start(list1); wk.ok(); wk.next())
	{
		ks.set(key, one);
	}

	for(wk.start(list2); wk.ok(); wk.next())
	{
		ks.set(key, one);
	}
	// convert to list
	zval_mgr result;
	result.new_array();
	htab_write wlist(result);

	for(wk.start(keyset); wk.ok(); wk.next())
	{
		wlist.push_back(key);
	}
	return result;
}

/**
 * Calling this moves the result array.
 * Call only once for each iteration.
 */ 
htab_mgr  
preg::captures()
{
	return htab_mgr(std::move(result_));
}

zstr_mgr  
preg::capture(size_t ix)
{
	zval_user test(result_);
	zstr_mgr  result;

	if (test.isArray())
	{
		htab_write captures(test.zarray());

		if (ix < captures.size()) 
		{
			result = captures.get(ix);
		}
	}
	return result;
}

preg::preg(const char* expr, int flags, bool global)
	:  pce_(nullptr),global_(global),flags_(flags)
{
	regexp_ = std::move(zstr_temp(expr));
}

preg::preg(zstr_user expr, int flags, bool global)
	:  pce_(nullptr), regexp_(expr),global_(global),flags_(flags)
{
}

preg::~preg() {
	if (pce_ != nullptr) {
	    php_pcre_pce_decref(pce_);
	}
}

pcre_cache_entry*  
preg::pce()
{
	if (pce_ == nullptr) {

		zstr_user rex(regexp_);

		pce_ = pcre_get_compiled_regex_cache(rex);
		php_pcre_pce_incref(pce_);
		if (pce_ == nullptr)
		{
			//exception
			zend_printf("Unable to compile regular expression %s\n", 
				rex.data(),0);
			return 0;
		}
	};
	return pce_;
}

zval_mgr 
preg::splits(zstr_user data, int limit)
{
	zval_mgr retval;
	/*PHPAPI void  php_pcre_split_impl(  
	 pcre_cache_entry *pce, zend_string *subject_str, zval *return_value,
	 zend_long limit_val, zend_long flags);*/

	php_pcre_split_impl(pce(), data, retval, limit, flags_);

	return retval;
}

int 
preg::matches(zstr_user subject, zend_long offset) 
{
	result_.set_null();
	count_.set_null();

	int isglobal = global_ ? 1 : 0;

#if PHP_VERSION_ID >= 80400
    //PHPAPI void php_pcre_match_impl(pcre_cache_entry *pce, zend_string *subject_str, zval *return_value,
	// zval *subpats, bool global, zend_long flags, zend_off_t start_offset)
	php_pcre_match_impl(pce(), subject, count_, result_,
		 isglobal,  flags_,  /*offset*/ offset);

#else
	int useflags = (flags_ == 0) ? 0 : 1;
	php_pcre_match_impl(pce(), subject, count_, result_,
		 isglobal,  useflags,  flags_,  /*offset*/ offset);
#endif
	return zval_user(count_).zlong();
}


zstr_mgr
preg::replace_callback(preg_callback& callback, zstr_user subject)
{
	flags_ = preg::OFFSET_CAPTURE;
	global_ = true;
	zstr_mgr  result;

	if (matches(subject) > 0) {
		htab_read  rtab_1(result_);
		zstr_user  subj(subject);

		std::string_view strview = subj.vstr();
		zstr_buffer ss;

		zval_mgr rlist = rtab_1.get(zend_long(0));
		htab_read replace(rlist);

		uint ipos = 0;
		size_t ct = replace.size();

		for(size_t i = 0; i < ct; i++)
		{
			htab_read cexp(replace.get(i));

			zval_user slen2 = cexp.get(zend_long(0));
			zval_user soffset2 = cexp.get(zend_long(1));

			size_t slen = slen2.size();
			size_t soffset = soffset2.zlong();
			// prior text first

			int prior_len = soffset - ipos;
			if (prior_len > 0)
			{
				ss << strview.substr(ipos, soffset - ipos);
				ipos += prior_len;
			}
			
			if (callback.get_replace(cexp))
			{
				ss << zstr_user(callback.replace_);
				callback.call_count_++;
			}
			else {
				break;
			}
					
			// callback to get replace;

			ipos = soffset + slen;
		}
		if (ipos < strview.size()) {
			ss << strview.substr(ipos);
		}

		if (ss.size() == 0)
		{
			//zend_printf("empty replace result\n");
			return result;
		}
		result.adopt(ss.zstr());
		return result;
	}
	result = subject;
	return result;
}	

zstr_mgr 
preg::replace(const char* rp, zstr_user subject)
{
	flags_ = preg::OFFSET_CAPTURE;
	global_ = true;
	zstr_mgr result;

	if (matches(subject) > 0) {
		htab_read rtab_1(result_);
		zstr_user  subj(subject);

		std::string_view strview = subj.vstr();
		std::string_view rval(rp);

		//std::stringstream ss;
		zstr_buffer ss;
		//showstr("mod init", ss);

		zval_mgr rlist = rtab_1.get(zend_long(0));
		htab_read replace(rlist);

		uint ipos = 0;
		size_t ct = replace.size();
		for(size_t i = 0; i < ct; i++)
		{
			zval_user vh2 = replace.get(i);
			htab_read h2(vh2);

			zval_user slen2 = h2.get(zend_long(0));
			zval_user soffset2 = h2.get(zend_long(1));

			size_t slen = slen2.size();
			size_t soffset = soffset2.zlong();
			// prior text first

			auto prior = strview.substr(ipos, soffset - ipos);
			ss << prior << rval;
			ipos = soffset + slen;
		}
		if (ipos < strview.size()) {
			ss << strview.substr(ipos);
		}

		if (ss.size() == 0)
		{
			//zend_printf("empty replace result\n");
			return result;
		}
		result.adopt(ss.zstr());
		return result;
	}
	result = subject;
	return result;
}

zval_mgr 
preg_replace(const char* exp, const char* replace, zstr_user input)
{
	preg reg(exp, 0, true);

	zstr_mgr rs = reg.replace(replace, input);
	return zval_mgr(std::move(rs));
}

zval_mgr 
preg_split(const char* exp, zstr_user data, int limit, int flags)
{
	preg sp(exp, flags);

	return sp.splits(data, limit);

}



}; // end namespace zpp

//wc_preg.cpp
#endif