#ifndef DATE_TIME_OBJ_H
#define DATE_TIME_OBJ_H

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
	};

	extern Date_init DTData;

	class timezone_obj : public zobj_mgr {
	protected:
		bool make_obj();
	public:
		timezone_obj(zstr_user zone);
	};


	class diff_dt : public zobj_mgr {
	public:
		long years();
		long months();
		long days();
		long hours();
		long minutes();
		long seconds();
		double fraction();
		zval_mgr daystotal();
	};

	class datetime_obj : public zobj_mgr {
	protected:
		bool make_obj();

	public:
		/** just creates object with time string
		 *  Helps to have default time zone setup in php.ini
		 *  or by a set call.
		 * */
		datetime_obj(const std::string_view& timestr);
		datetime_obj(zstr_user zs);
		datetime_obj(const zval_mgr& fnret); 

		/** Now constructor */
		datetime_obj();
		
		diff_dt diff(datetime_obj& dtm);

		zstr_mgr format(zstr_user dfmt);

		void setTime(int hour, int minute, int secs, int microsec);
		void setDate(int year, int month, int day);
		
		void setTimeZone(zstr_user zone);

		static zval_mgr  strtotime(zval* value);
		static zstr_mgr  date(zstr_user dfmt, zval* value);

	};
}; // namespace

//datetime.h
#endif
