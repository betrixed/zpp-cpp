#ifndef DATE_TIME_OBJ_H
#define DATE_TIME_OBJ_H

#ifndef PHP_EXTERN_H
#include "php_extern.h"
#endif

#ifndef STATE_INIT_H
#include "state_init.h"
#endif

#ifndef STR_RC_H
#include "str_rc.h"
#endif

namespace zpp {
	
	/**
	@class Date_init
	*/
	class Date_init : public state_init {
	public:
		Date_init() : state_init() {}
		void init() override;

		str_intern formatkey;
		str_intern construct_key;
		str_intern settime;
		str_intern diff;
		str_intern date;
		str_intern setdate;
		str_intern strtotime;
		str_intern settimezone;

		str_intern y_prop;
		str_intern m_prop;
		str_intern d_prop;
		str_intern h_prop;
		str_intern i_prop;
		str_intern s_prop;
		str_intern f_prop;
		str_intern days_prop;
		str_intern invert_prop;
		str_intern from_string;
		str_intern date_string;
		str_intern now_format;
	};

	extern Date_init DTData;

	/** 
	@class timezone_obj
	@brief Wrapper for zend_object which is a DateTimeZone
	*/
	class timezone_obj : public obj_rc {
	protected:
		bool make_obj();
	public:
		timezone_obj(str_ptr zone);
	};

/** 
@class dt_interval
@brief Wrapper for zend_object which is a DateInterval

*/
	class dt_interval : public obj_rc {
	public:
		long years();
		long months();
		long days();
		long hours();
		long minutes();
		long seconds();
		double fraction();
		val_rc daystotal();
	};

	/**
	@class datetime_obj
	@brief Wrapper for zend_object which is a DateTime
	*/
	class datetime_obj : public obj_rc {
	protected:
		bool make_obj();

	public:
		/** just creates object with time string
		 *  Helps to have default time zone setup in php.ini
		 *  or by a set call.
		 * */
		datetime_obj(const std::string_view& timestr);
		datetime_obj(str_ptr zs);
		datetime_obj(obj_rc obj); 

		/** Now constructor */
		datetime_obj();
		
		dt_interval diff(datetime_obj& dtm);

		str_rc format(str_ptr dfmt);

		void setTime(int hour, int minute, int secs, int microsec);
		void setDate(int year, int month, int day);
		
		void setTimeZone(str_ptr zone);

		static val_rc  strtotime(zval* value);
		static str_rc  date(str_ptr dfmt, zval* value);
		static double  microtime();

	};
}; // namespace

//datetime.h
#endif
