#ifndef WC_PREG_H
#define WC_PREG_H


#ifndef ZVAL_USER_H
#include "zval_user.h"
#endif

#ifndef ZSTR_USER_H
#include "zstr_user.h"
#endif

extern "C" {
	#include "ext/pcre/php_pcre.h"
};

namespace zpp {

	class preg_callback {
	protected:
		size_t 		call_count_;
		zstr_mgr    replace_; // set this in get_replace
	public:
		preg_callback() : call_count_(0)
		{
		}

		virtual ~preg_callback() 
		{ }

		virtual bool get_replace(htab_read captures) = 0; 

		friend class preg;
	};


	class preg {
	protected:
		pcre_cache_entry* pce_;
		zstr_mgr      	  regexp_;
		zval_mgr          result_; //hold result, array of matches
		zend_long         count_;
		bool          	  global_; //match all
		int			 	  flags_;

		pcre_cache_entry*  pce();

	public:

		enum {
			 // found in pcre.c
			 PATTERN_ORDER = 1,
			 SET_ORDER = 2,
			 OFFSET_CAPTURE = (1<<8),
			 SPLIT_NO_EMPTY = (1<<0),
			 SPLIT_DELIM_CAPTURE = (1<<1),
			 SPLIT_OFFSET_CAPTURE = (1<<2)
		};

		preg(zstr_user expr, int flags=0, bool global = false);
		preg(const char* expr, int flags=0, bool global = false);

		~preg();

		int 	   matches(zstr_user subject, zend_long offset = 0);
		zval_mgr   splits(zstr_user data, int limit = -1);


		void setGlobal(bool b) {
			global_ = b;
		}

		void setFlags(int f) {
			flags_ = f;
		}

		zstr_mgr  replace_callback(preg_callback& callback, zstr_user subject);
		
		zstr_mgr  replace(const char* rv, zstr_user subject);

		zstr_user regex() const {
			return regexp_;
		}
		zstr_mgr  capture(size_t ix);
		htab_read  captures(){ return htab_read(result_); };
		
		zval_mgr& results() { return result_; }

		//htab_read  array() { return htab_read(result_); }

	};

	zval_mgr preg_split(const char* sv, zstr_user data, int limit, int flags);

	zval_mgr preg_replace(const char* exp, const char* replace, zstr_user input);

	zval_mgr explode(zstr_user sep,  zstr_user split, long limit = 0);

	zval_mgr implode(zstr_user sep, htab_read w);

	zval_mgr union_values(htab_read list1, htab_read list2);

	zval_mgr preg_quote(zstr_user regex, zval_user delim);

}; // end namespace zpp

//wc_preg.h
#endif