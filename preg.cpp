#ifndef ZPP_PREG_CPP
#define ZPP_PREG_CPP

/**
 * @file preg.cpp
 * @author Michael Rynn <michael.rynn.500@gmail.com>
 * @brief zpp::preg - Regular expression support
 * @copyright Copyright (c) 2025 Michael Rynn
 * @license Artistic License 2.0	
 */
#ifndef ZPP_PREG_H
#include "preg.h"
#endif



#ifndef STR_BUF_H
#include "str_buf.h"
#endif

#ifndef HTAB_RC_H
#include "htab_rc.h"
#endif

#ifndef HTAB_WALK_H
#include "htab_walk.h"
#endif


extern "C" {
	#include <ext/standard/php_string.h>
};

namespace zpp {


val_rc explode(str_ptr sep,  str_ptr  split, long limit)
{
	val_rc  list;
	list.new_array();

	php_explode(sep, split, list, limit);

	return list;
}

str_rc implode(str_ptr sep, htab_ptr arr)
{
	val_rc temp;

	php_implode(sep, arr, temp);

	str_rc result(temp.zstr());
	return result;
}

/** Return a list of unique values, from 2 arrays.
 * manage lists of names separated by space characters, eg class attributes
 * as array lists. Ensure only one instance of each name.
 * This merge 'filters' names through a keyed array. */
val_rc
union_values(htab_ptr list1, htab_ptr list2)
{
	htab_rc   keyset;
	htab_rw ks(keyset);

	val_rc one(1);

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
	val_rc result;
	result.new_array();
	htab_rw wlist(result);

	for(wk.start(keyset); wk.ok(); wk.next())
	{
		wlist.push_back(key);
	}
	return result;
}

/**
 * Calling this moves the result array.
 * Call only once for each iteration.
 
htab_rc  
preg::captures()
{
	return htab_rc(std::move(result_));
}
*/ 
str_rc  
preg::capture(size_t ix)
{
	val_ptr test(result_);
	str_rc  result;

	if (test.isArray())
	{
		htab_rw captures(test);

		if (ix < captures.size()) 
		{
			result = captures.get(ix);
		}
	}
	return result;
}

preg::preg() : pce_(nullptr), global_(false), flags_(0)
{
}

preg::preg(const char* expr, int flags, bool global)
	:  pce_(nullptr),global_(global),flags_(flags)
{
	regexp_ =  str_temp(expr);
	//showstr("regexp_",regexp_);
}

preg::preg(str_ptr expr, int flags, bool global)
	:  pce_(nullptr), regexp_(expr),global_(global),flags_(flags)
{
}

void
preg::setExpr(str_ptr expr)
{
	if (pce_ != nullptr)
	{
		 php_pcre_pce_decref(pce_);
		 pce_ = nullptr;
	}
	regexp_ = expr;
}

void 
preg::init(str_ptr expr, int flags, bool global)
{
	setExpr(expr);
	flags_ = flags;
	global_ = global;
}

preg::~preg() {
	if (pce_ != nullptr) {
		//zend_printf("Release PCE %lx\n", pce_);
	    php_pcre_pce_decref(pce_);
	}
}

pcre_cache_entry*  
preg::pce()
{
	if (pce_ == nullptr) 
	{
		pce_ = pcre_get_compiled_regex_cache(regexp_);
		
		if (pce_ == nullptr)
		{
			//exception
			zend_throw_error(zend_ce_error, "Unable to compile regular expression %s\n", 
			     str_ptr(regexp_).data());
			return nullptr;
		}
		//zend_printf("Hold PCE %lx\n", pce_);
		php_pcre_pce_incref(pce_);
	};
	return pce_;
}

val_rc 
preg::splits(str_ptr data, int limit)
{
	val_rc retval;
	/*PHPAPI void  php_pcre_split_impl(  
	 pcre_cache_entry *pce, zend_string *subject_str, zval *return_value,
	 zend_long limit_val, zend_long flags);*/
	pcre_cache_entry* cre = pce();
	if (cre)
		php_pcre_split_impl(cre, data, retval, limit, flags_);

	return retval;
}

int 
preg::matches(str_ptr subject, zend_long offset) 
{
	result_.set_null();

	val_rc ret_val;

	int isglobal = global_ ? 1 : 0;

	pcre_cache_entry* cre = pce();
	if (!cre)
	{
		return 0;
	}

#if PHP_VERSION_ID >= 80400
/**
   PHPAPI void php_pcre_match_impl(pcre_cache_entry *pce, 
   	zend_string *subject_str, zval *return_value,
	zval *subpats, bool global, zend_long flags, zend_off_t start_offset)
*/
	php_pcre_match_impl(cre, subject, ret_val, result_,
		 isglobal,  flags_,  /*offset*/ offset);

#else
	int useflags = (flags_ == 0) ? 0 : 1;
	php_pcre_match_impl(cre, subject, ret_val, result_,
		 isglobal,  useflags,  flags_,  /*offset*/ offset);
#endif
	val_ptr test(ret_val);

	count_ = test.isLong() ? test.zlong() : 0;

	test = result_;

	if (!test.isArray())
	{
		count_ = 0;
	}
	return count_;
}


str_rc
preg_callback::replace(str_ptr subject)
{
	//showstr("replace ", subject);

	regexp_.setFlags(preg::OFFSET_CAPTURE);
	regexp_.setGlobal(true);

	str_rc  result;

	int ct = regexp_.matches(subject);

	//zend_printf("\nmatched %d\n", ct);
	if (ct > 0) 
	{
		val_rc& test = regexp_.results();


		all_list_ = test.zarray();

		//showdata("all_list_", all_list_);

		wholes_ = all_list_.get(int(0));
		captures_ = all_list_.get(int(1));


		std::string_view strview = subject.vstr();

		str_buf ss;

		uint ipos = 0;
		ct = wholes_.size();

		for(call_ct_ = 0; call_ct_ < ct; call_ct_++)
		{
			htab_ptr w1 = wholes_.get(int(call_ct_));

 			str_ptr ctext    = w1.get(zend_long(0));
			val_ptr coffset  = w1.get(zend_long(1));

			size_t slen =    ctext.size();
			size_t soffset = coffset.zlong();
		// add prior text first

			int prior_len = soffset - ipos;
			if (prior_len > 0)
			{
				ss << strview.substr(ipos, soffset - ipos);
				ipos += prior_len;
			}
		
			/* Work with callback to create replace_ string 
			   from the current match data.
			*/
			if (this->callback())
			{
				ss << str_ptr(replace_);
			}
				
			// callback to get replace;

			ipos = soffset + slen;

			// add remaining unmatched text
			if (ipos < strview.size()) {
				ss << strview.substr(ipos);
			}
		}
		if (ss.size() == 0)
		{
			//zend_printf("empty replace result\n");
			return result;
		}

		result = ss.zstr();
		return result;
	}
	result = subject;
	return result;
}	

str_rc 
preg::replace(const char* rp, str_ptr subject)
{
	flags_ = preg::OFFSET_CAPTURE;
	global_ = true;
	str_rc result;

	if (matches(subject) > 0) {
		htab_ptr rtab_1(result_);
		str_ptr  subj(subject);

		std::string_view strview = subj.vstr();
		std::string_view rval(rp);

		//std::stringstream ss;
		str_buf ss;
		//showstr("mod init", ss);

		val_rc rlist = rtab_1.get(zend_long(0));
		htab_ptr replace(rlist);

		uint ipos = 0;
		size_t ct = replace.size();
		for(size_t i = 0; i < ct; i++)
		{
			val_ptr vh2 = replace.get(i);
			htab_ptr h2(vh2);

			val_ptr slen2 = h2.get(zend_long(0));
			val_ptr soffset2 = h2.get(zend_long(1));

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


val_rc 
reg_replace(const char* exp, const char* replace, str_ptr input)
{
	preg reg(exp, 0, true);

	str_rc rs = reg.replace(replace, input);
	return val_rc(std::move(rs));
}

str_rc 
preg_replace(str_ptr rexpr, str_ptr replace, str_ptr input, int limit, size_t* rcount)
{
	if (rcount)
	{
		*rcount = 0;
	}
	str_rc result;
/* call the PHPAPI:  zend_string *php_pcre_replace(zend_string *regex, zend_string *subject_str, 
 const char *subject, size_t subject_len, 
 zend_string *replace_str, size_t limit, size_t *replace_count);
 */
	zend_string* zs = php_pcre_replace(rexpr, input, input.data(), input.size(), replace, limit, rcount);

	result.adopt(zs);

	return result;
}

val_rc 
preg_split(const char* exp, str_ptr data, int limit, int flags)
{
	preg sp(exp, flags);

	return sp.splits(data, limit);

}


}; // end namespace zpp

//wc_preg.cpp
#endif