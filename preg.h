#ifndef ZPP_PREG_H
#define ZPP_PREG_H

/**
 * @file zpp/preg.h 
 * @author Michael Rynn <michael.rynn.500@gmail.com>
 * @brief class and functions using PHP API of its PCRE extension.
 * @copyright Copyright (c) 2025 Michael Rynn
 * @license BSD 3-Clause License
 */

#ifndef VAL_PTR_H
#include "val_ptr.h"
#endif

#ifndef STR_PTR_H
#include "str_ptr.h"
#endif

#ifndef VAL_RC_H
#include "val_rc.h"
#endif

#ifndef STR_RC_H
#include "str_rc.h"
#endif

#ifndef HTAB_PTR_H
#include "htab_ptr.h"
#endif

extern "C" {
	#include "ext/pcre/php_pcre.h"
};

namespace zpp {

	
    /**
	 * @class preg
	 * @brief Wrapper for PHP preg functions
	 * Uses PHP internal pcre functions.
	 * Can be used for preg_match, preg_match_all, preg_split, preg_replace
	 */


	class preg {
	protected:
		pcre_cache_entry* pce_; // cached generated entry
		str_rc      	  regexp_;
		val_rc            result_; //hold result, array of matches
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

		preg(str_ptr expr, int flags=0, bool global = false);
		preg(const char* expr, int flags=0, bool global = false);
		preg();

		~preg();

		void release(); // release refcount on pce;

		int 	 matches(str_ptr subject, zend_long offset = 0);
		val_rc   splits(str_ptr data, int limit = -1);

		void init(str_ptr expr, int flags = 0, bool global = false);
		
		void setExpr(str_ptr expr);
		
		void setGlobal(bool b) {
			global_ = b;
		}

		void setFlags(int f) {
			flags_ = f;
		}

		
		
		str_rc  replace(const char* rv, str_ptr subject);

		str_ptr regex() const {
			return regexp_;
		}
		str_rc  capture(size_t ix);
		htab_ptr  captures(){ return htab_ptr(result_); };
		
		val_rc& results() { return result_; }

		//htab_ptr  array() { return htab_ptr(result_); }

	};

	/**
	 * @class preg_callback
	 * @brief Base class for preg_replace_callback
	 * Derive from this class and implement the callback() method.
	 * The replace() method will call the callback for each match
	 * and return the final replaced string.
	 * The captures are available in the htab_ptr captures_ member.
	 * The whole matched string is in wholes_ member.
	 * The replace_ member is set by the callback to the replacement string.
	 */
	class preg_callback {
	protected:
		preg      regexp_;

		htab_ptr  all_list_;	
		// update before each callback

		htab_ptr  wholes_;  
		htab_ptr  captures_;
		// set by callback
		str_rc    replace_; 
		// update after callback
		int 	  call_ct_;
		
	public:
		preg_callback(const char* expr) 
		    : regexp_(expr)
		{
		}

		virtual ~preg_callback() 
		{ }

		virtual str_rc replace(str_ptr subject);

		virtual bool callback() = 0; 

		friend class preg;
	};


	val_rc preg_split(const char* sv, str_ptr data, int limit, int flags);

	// str_ptr version
	str_rc preg_replace(str_ptr rexpr, str_ptr replace, str_ptr input, int limit = -1, size_t* rcount = nullptr);

	val_rc explode(str_ptr sep,  str_ptr split, long limit = INT_MAX);

	str_rc implode(str_ptr sep, htab_ptr w);

	val_rc union_values(htab_ptr list1, htab_ptr list2);

	val_rc preg_quote(str_ptr regex, val_ptr delim);

	

}; // end namespace zpp

//wc_preg.h
#endif