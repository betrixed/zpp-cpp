#ifndef DATE_TIME_OBJ_H
#define DATE_TIME_OBJ_H

namespace zpp {

class timezone_obj : public zobj_mgr {
	protected:
		bool make_obj();
	public:
		timezone_obj(zstr_user zone);
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
		
		zobj_mgr diff(datetime_obj& dtm);

		zstr_mgr format(zstr_user dfmt);

		void setTime(int hour, int minute, int secs, int microsec);
		void setDate(int year, int month, int day);
		
		void setTimeZone(zstr_user zone);

		static zval_mgr  strtotime(zval* value);
		static zstr_mgr  reformat(zval* fmt, zval* value);

	};
}; // namespace

//datetime.h
#endif
