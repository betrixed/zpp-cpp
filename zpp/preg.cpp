#ifndef WC_PREG_CPP
#define WC_PREG_CPP

#ifndef WC_PREG_H
#include "wc_preg.h"
#endif


extern "C" {
	#include <ext/standard/php_string.h>
};

namespace zpp {


zval_own explode(const zstr_base& sep,  const zstr_base&  split, long limit)
{
	zval_own  list;
	list.init_array();

	php_explode(sep, split, list, limit);

	return list;
}

zval_own implode(const zstr_base& sep, htab_ptr arr)
{
	zval_own result;

	php_implode(sep, arr, result);

	return result;
}

/** Return a list of unique values, from 2 arrays.
 * manage lists of names separated by space characters, eg class attributes
 * as array lists. Ensure only one instance of each name.
 * This merge 'filters' names through a keyed array. */
zval_own
union_values(htab_ptr list1, htab_ptr list2)
{
	htab_own keyset;

	zval_own one;
	one.set(1);

	htab_walk wk;
	// use values as keys
	auto& key = wk.value();

	for(wk.start(list1); wk.ok(); wk.next())
	{
		keyset.set(key, one);
	}

	for(wk.start(list2); wk.ok(); wk.next())
	{
		keyset.set(key, one);
	}
	// convert to list
	zval_own result;
	result.init_array();
	htab_ptr wlist(result);

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
htab_own  preg::captures()
{
	return htab_own(std::move(result_));
}

zstr_own  preg::capture(size_t ix)
{
	if (result_.isArray())
	{
		htab_ptr captures(result_.zarray());

		if (ix < captures.size()) 
		{
			return captures.get(ix);
		}
	}
	return zstr_own();
}

preg::preg(const char* expr, int flags, bool global)
	:  pce_(nullptr),global_(global),flags_(flags)
{
	regexp_ = std::move(zstr_temp(expr));
}

preg::preg(zend_string* expr, int flags, bool global)
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
		pce_ = pcre_get_compiled_regex_cache(regexp_);
		php_pcre_pce_incref(pce_);
		if (pce_ == nullptr)
		{
			//exception
			zend_printf("Unable to compile regular expression %s\n", ZSTR_VAL(regexp_.ptr()));
			return 0;
		}
	};
	return pce_;
}

zval_own 
preg::splits(zend_string* data, int limit)
{
	zval_own retval;
	/*PHPAPI void  php_pcre_split_impl(  
	 pcre_cache_entry *pce, zend_string *subject_str, zval *return_value,
	 zend_long limit_val, zend_long flags);*/

	php_pcre_split_impl(pce(), data, retval, limit, flags_);

	return retval;
}

int preg::matches(zend_string* subject, zend_long offset) 
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
	return count_.zlong();
}


zstr_own 
preg::replace_callback(preg_callback& callback, zend_string* subject)
{
	flags_ = preg::OFFSET_CAPTURE;
	global_ = true;

	if (matches(subject) > 0) {
		htab_ptr rtab_1(result_);
		zstr_ptr  subj(subject);

		std::string_view strview = subj.vstr();
		zstr_buffer ss;

		zval_own rlist = rtab_1.get(zend_long(0));
		htab_ptr replace(rlist);

		uint ipos = 0;
		size_t ct = replace.size();
		for(size_t i = 0; i < ct; i++)
		{
			htab_ptr cexp(replace.get(i));

			zval_own slen2 = cexp.get(zend_long(0));
			zval_own soffset2 = cexp.get(zend_long(1));

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
				ss << callback.replace_;
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
			return zstr_own();
		}
		return zstr_pass(ss.zstr());
	}
	return zstr_own(subject);
}	

zstr_own  
preg::replace(const char* rp, zend_string* subject)
{
	flags_ = preg::OFFSET_CAPTURE;
	global_ = true;
	

	if (matches(subject) > 0) {
		htab_ptr rtab_1(result_);
		zstr_ptr  subj(subject);

		std::string_view strview = subj.vstr();
		std::string_view rval(rp);

		//std::stringstream ss;
		zstr_buffer ss;
		//showstr("mod init", ss);

		zval_own rlist = rtab_1.get(zend_long(0));
		htab_ptr replace(rlist);
		uint ipos = 0;
		size_t ct = replace.size();
		for(size_t i = 0; i < ct; i++)
		{
			zval_own vh2 = replace.get(i);
			htab_ptr h2(vh2);

			zval_own slen2 = h2.get(zend_long(0));
			zval_own soffset2 = h2.get(zend_long(1));

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
			return zstr_own();
		}
		return zstr_pass(ss.zstr());
	}
	return zstr_own(subject);
}

zval_own 
preg_replace(const char* exp, const char* replace, zend_string* input)
{
	preg reg(exp, 0, true);

	return reg.replace(replace, input);
}

zval_own 
preg_split(const char* exp, zstr_ptr data, int limit, int flags)
{
	preg sp(exp, flags);

	return sp.splits(data, limit);

}



}; // end namespace zpp

//wc_preg.cpp
#endif