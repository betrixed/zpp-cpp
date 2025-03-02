#ifndef DATE_TIME_OBJ_CPP
#define DATE_TIME_OBJ_CPP

#ifndef ZOBJ_MGR_H
#include "zobj_mgr.h"
#endif

extern "C" {
#include <ext/date/php_date.h>
};

#ifndef FN_CALL_H
#include "fn_call.h"
#endif


namespace zpp {


class Date_init : public state_init {
public:
	Date_init() : state_init() {}

	zstr_intern formatkey;
	zstr_intern construct_key;
	zstr_intern settime;
	zstr_intern diff;
	zstr_intern date;
	zstr_intern setdate;
	zstr_intern strtotime;
	zstr_intern settimezone;


	virtual void init() 
	{
		strtotime = zstr_intern("strtotime");
		construct_key = zstr_intern("__construct");

		formatkey = zstr_intern("format");
		settime = zstr_intern("settime");
		diff = zstr_intern("diff");
		date = zstr_intern("date");
		setdate = zstr_intern("setdate");
		settimezone = zstr_intern("settimezone");

		
	}
};

Date_init DTData;

bool
datetime_obj::make_obj()
{
	zend_class_entry* ce = php_date_get_date_ce();

	zval  temp = {0};
	zval* rp;

	rp = php_date_instantiate(ce, &temp);

	if (rp)
	{
		obj_ = Z_OBJ(temp);
		return true;
	}
	return false;

}

zval_mgr
datetime_obj::strtotime(zval* value)
{
	fn_call_args<1> strtotime;

	strtotime.set_fname(DTData.strtotime);
	return strtotime.call1(value);
}

zstr_mgr 
datetime_obj::reformat(zval* fmt, zval* value)
{
	zstr_mgr result;

	zval_mgr timeval = strtotime(value);

	if (zval_user(timeval).isLong())
	{
		fn_call_args<2> datefmt;
		datefmt.set_fname(DTData.date);
		result = datefmt.call2(fmt, timeval);
	}
	return result;
}


datetime_obj::datetime_obj(zstr_user zs)
{
	if (!make_obj())
	{
		return;
	}

	std::string_view vs = zs.vstr();
	auto dobj = php_date_obj_from_obj(obj_);

	if (!php_date_initialize(dobj, vs.data(), vs.size(), nullptr, nullptr, 0) )
	{
		throw std::logic_error("Failed to init DateTime object");
	}

}

datetime_obj::datetime_obj(const std::string_view& c)
{
	if (!make_obj())
	{
		return;
	}

	auto dobj = php_date_obj_from_obj(obj_);

	if (! php_date_initialize(dobj, c.data(), c.size(), nullptr, nullptr, 0) )
	{
		throw std::logic_error("Failed to init DateTime object");
	}
}

datetime_obj::datetime_obj()
{
	if (!make_obj())
	{
		return;
	}
	auto dobj = php_date_obj_from_obj(obj_);
	if (! php_date_initialize(dobj, nullptr, 0, nullptr, nullptr, 0) )
	{
		throw std::logic_error("Failed to init DateTime object");
	}
}

void 
datetime_obj::setDate(int year, int month, int day)
{
	fn_call_args<3> set_date;

	zval_mgr arg1(year);
	zval_mgr arg2(month);
	zval_mgr arg3(day);

	set_date.set_fci(obj_, DTData.setdate);
	//throw away return result
	set_date.call3(arg1,arg2,arg3);
}

void 
datetime_obj::setTime(int hour, int minute, int second, int millisec)
{
	fn_call_args<4> set_time;

	zval_mgr arg1(hour);
	zval_mgr arg2(minute);
	zval_mgr arg3(second);
	zval_mgr arg4(millisec);

	set_time.set_fci(obj_, DTData.settime);
	set_time.call4(arg1,arg2,arg3,arg4);
}


zstr_mgr 
datetime_obj::format(zstr_user dfmt)
{
	zobj_user caller(obj_);
	zval_mgr  arg1(dfmt);
	zstr_mgr result = caller.call(DTData.formatkey, arg1);

	return zstr_mgr(std::move(result));
}

void datetime_obj::setTimeZone(zstr_user zone)
{
	timezone_obj tz(zone);
	zobj_user  temp(obj_);

	temp.call(DTData.settimezone, zval_mgr(tz));
}

datetime_obj::datetime_obj(const zval_mgr& fnret)
{
	obj_ = zval_user(fnret).zobject();
	//TODO: assert is datetime object
}

zobj_mgr 
datetime_obj::diff(datetime_obj& dtm)
{

	fn_call_args<1> diffobj;

	diffobj.set_fci(obj_, DTData.diff);

	zval_mgr arg1(dtm);

	return zobj_mgr(diffobj.call1(arg1));

}

timezone_obj::timezone_obj(zstr_user zone)
{
	if (!make_obj())
	{
		return;
	}
	zobj_user temp(obj_);
	zval_mgr  arg1(zone);
	
	temp.call(DTData.construct_key, arg1);
}

bool
timezone_obj::make_obj()
{
	zend_class_entry* ce = php_date_get_timezone_ce();

	zval  temp = {0};
	zval* rp;

	rp = php_date_instantiate(ce, &temp);

	if (rp)
	{
		obj_ = Z_OBJ(temp);
		return true;
	}
	return false;

}




}; // namespace

//datetime.cpp
#endif