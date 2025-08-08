#ifndef DAYTIME_H
#define DAYTIME_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif


namespace wcc {

	/**
	 * This class is meant to be extended
	 * by PHP script classes.
	 * 
	 */ 
	using namespace zpp;

	
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

		virtual void debug_info(htab_rw di);
		
		void construct(str_ptr sval);

		void str(str_ptr sval);
		void time(long hours, long mins, double seconds);

		void day(double dval);

		double value() const { return tval_; }

		void split(val_ptr hours, val_ptr mins, val_ptr seconds);

		str_rc format(int flags);

		str_rc toString();

		static bool 		day24_time(int hours, int mins, double secs, double& ret, bool except=false);
		static bool		 	day24_str(const char* s, long slen, double& ret, bool except=false);

		static void 		day24_split(double val, int* h24, int* min, double* secs);
		static zend_string* day24_format(double val, int flags);
	};

	
	class day24_obj : public obj_rc {
	protected:
		// return un-managed object
		static Day24* make_obj(str_ptr zs);
	public:
		
		
	/** just creates object with time string
	 *  Helps to have default time zone setup in php.ini
	 *  or by a set call.
	 * */
		day24_obj(str_ptr zs);
		str_rc format(int flags);
	};
} // namespace


//daytime.h
#endif
