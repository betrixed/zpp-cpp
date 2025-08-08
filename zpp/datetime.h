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
	
	class Date_init : public state_init {
	public:
		Date_init() : state_init() {}
		void init() override;

		zstr_intern formatkey;
		zstr_intern construct_key;
		zstr_intern settime;
		zstr_intern diff;
		zstr_intern date;
		zstr_intern setdate;
		zstr_intern strtotime;
		zstr_intern settimezone;

		zstr_intern y_prop;
		zstr_intern m_prop;
		zstr_intern d_prop;
		zstr_intern h_prop;
		zstr_intern i_prop;
		zstr_intern s_prop;
		zstr_intern f_prop;
		zstr_intern days_prop;
		zstr_intern invert_prop;
		zstr_intern from_string;
		zstr_intern date_string;
		zstr_intern now_format;
	};

	extern Date_init DTData;

	class timezone_obj : public obj_rc {
	protected:
		bool make_obj();
	public:
		timezone_obj(str_ptr zone);
	};


	class diff_dt : public obj_rc {
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
		
		diff_dt diff(datetime_obj& dtm);

		str_rc format(str_ptr dfmt);

		void setTime(int hour, int minute, int secs, int microsec);
		void setDate(int year, int month, int day);
		
		void setTimeZone(str_ptr zone);

		static val_rc  strtotime(zval* value);
		static str_rc  date(str_ptr dfmt, zval* value);

	};
}; // namespace

//datetime.h
#endif
