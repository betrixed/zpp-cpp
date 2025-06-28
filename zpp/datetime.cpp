#ifndef DATE_TIME_OBJ_CPP
#define DATE_TIME_OBJ_CPP

#ifndef ZOBJ_MGR_H
#include "zobj_mgr.h"
#endif

#ifndef DATE_TIME_OBJ_H
#include "datetime.h"
#endif

extern "C" {
#include <ext/date/php_date.h>
};

#ifndef FN_CALL_H
#include "fn_call.h"
#endif


namespace zpp {

Date_init DTData;

void Date_init::init()
{
	strtotime = "strtotime";
	construct_key = "__construct";

	formatkey = "format";
	settime = "settime";
	diff = "diff";
	date = "date";
	setdate = "setdate";
	settimezone = "settimezone";

	zstr_intern y_prop = "y";
	zstr_intern m_prop = "m";
	zstr_intern d_prop = "d";
	zstr_intern h_prop = "h";
	zstr_intern i_prop = "i";
	zstr_intern s_prop = "s";
	zstr_intern f_prop = "f";
	zstr_intern days_prop = "days";
	zstr_intern invert_prop = "invert";
	zstr_intern from_string = "from_string";
	zstr_intern date_string = "date_string";
	zstr_intern now_format = "Y-m-d H:i:s";
}


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
	ZVAL_COPY_VALUE(strtotime.argsptr(), value);
	return strtotime.call_fn();
}

zstr_mgr //static
datetime_obj::date(zstr_user fmt, zval* value)
{
	zstr_mgr result;

	zval_mgr timeval = strtotime(value);

	if (zval_user(timeval).isLong())
	{
		fn_call_args<2> datefmt;
		datefmt.set_fname(DTData.date);
		zval* pz = datefmt.argsptr();
		ZVAL_STR(pz, fmt);
		ZVAL_COPY_VALUE(pz+1, timeval);
		result = datefmt.call_fn();
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

	zval* pz = set_date.argsptr();
	ZVAL_LONG(pz, year);
	ZVAL_LONG(pz+1, month);
	ZVAL_LONG(pz+2, day);

	set_date.set_fci(obj_, DTData.setdate);
	//throw away return result
	set_date.call_fn();
}

void 
datetime_obj::setTime(int hour, int minute, int second, int millisec)
{
	fn_call_args<4> set_time;
	zval* pz = set_time.argsptr();
	ZVAL_LONG(pz, hour);
	ZVAL_LONG(pz+1, minute);
	ZVAL_LONG(pz+2, second);
	ZVAL_LONG(pz+3, millisec);

	set_time.set_fci(obj_, DTData.settime);
	set_time.call_fn();
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

datetime_obj::datetime_obj(zobj_mgr fnret)
{
	obj_ = fnret;
	//TODO: assert is datetime object
}

diff_dt 
datetime_obj::diff(datetime_obj& dtm)
{

	fn_call_args<1> diffobj;

	diffobj.set_fci(obj_, DTData.diff);

	ZVAL_OBJ(diffobj.argsptr(),(zend_object*) dtm);

	return diff_dt(diffobj.call_fn());

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

long 
diff_dt::years()
{
	zval_mgr temp = zobj_user(obj_).property(DTData.y_prop);
	return zval_user(temp).zlong();
}

long 
diff_dt::months()
{
	zval_mgr temp = zobj_user(obj_).property(DTData.m_prop);
	return zval_user(temp).zlong();
}
long 
diff_dt::days()
{
	zval_mgr temp = zobj_user(obj_).property(DTData.d_prop);
	return zval_user(temp).zlong();
}
long 
diff_dt::hours()
{
	zval_mgr temp = zobj_user(obj_).property(DTData.h_prop);
	return zval_user(temp).zlong();
	
}
long 
diff_dt::minutes()
{
	zval_mgr temp = zobj_user(obj_).property(DTData.i_prop);
	return zval_user(temp).zlong();
	
}
long 
diff_dt::seconds()
{
	zval_mgr temp = zobj_user(obj_).property(DTData.s_prop);
	return zval_user(temp).zlong();
}

double 
diff_dt::fraction()
{
	zval_mgr temp = zobj_user(obj_).property(DTData.f_prop);
	return zval_user(temp).zdouble();
	
}

zval_mgr 
diff_dt::daystotal()
{
	return zobj_user(obj_).property(DTData.days_prop);
}



}; // namespace

//datetime.cpp
#endif