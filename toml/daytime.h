#ifndef DAYTIME_H
#define DAYTIME_H

#ifndef WC_BASE_H
#include "wc_base.h"
#endif


namespace wcc {

	/**
	 * This class is meant to be extended
	 * by PHP script classes.
	 * 
	 */ 

	class Day24 : public base_d {
	protected:
		double tval_;
	public:

		enum {
			SEC_AUTO = 0,
			SEC_INT = 1,
			SEC_DEC = 2,
			SEC_NONE = 4
		};

		static base_obj_mgr<Day24> omg;

		virtual void debug_info(HashTable* h);
		
		void construct(const zstr_base& sval);

		void str(const zstr_base& sval);
		void time(long hours, long mins, double seconds);

		void day(double dval);

		double value() const { return tval_; }

		void split(zval_ptr hours, zval_ptr mins, zval_ptr seconds);

		zstr_own format(int flags);

		zstr_own toString();

		static bool 		day24_time(int hours, int mins, double secs, double& ret, bool except=false);
		static bool		 	day24_str(const char* s, long slen, double& ret, bool except=false);

		static void 		day24_split(double val, int* h24, int* min, double* secs);
		static zend_string* day24_format(double val, int flags);
	};


}; //namespace wcc;


namespace zpp {

	using namespace wcc;
	
	class day24_obj : public zobj_own {
	public:
		static Day24* make_obj(const zstr_ptr& zs);
	/** just creates object with time string
	 *  Helps to have default time zone setup in php.ini
	 *  or by a set call.
	 * */
	day24_obj(const zstr_ptr& zs);
	zstr_own format(int flags);
	};
}


//daytime.h
#endif
