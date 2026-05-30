#ifndef DATE_TIME_OBJ_CPP
#define DATE_TIME_OBJ_CPP

/**
 *  @file datetime.cpp
 *  @brief Wrapper for zend_object which is a DateTime
 *  @author Michael Rynn <michael.rynn.500@gmail.com>
 * 
 *  copyright (c) 2025 Michael Rynn
 *  Distributed under the BSD 3-Clause License
 */
#ifndef OBJ_RC_H
#include "obj_rc.h"
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

void   register_datetime()
{
	STATE_INIT_ADD(DTData);
}

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

	y_prop = "y";
	m_prop = "m";
	d_prop = "d";
	h_prop = "h";
	i_prop = "i";
	s_prop = "s";
	f_prop = "f";
	days_prop = "days";
	invert_prop = "invert";
	from_string = "from_string";
	date_string = "date_string";
	now_format = "Y-m-d H:i:s";
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

val_rc //static
datetime_obj::strtotime(zval* value)
{
	fn_call fn(DTData.strtotime);

	fn_params<1> strtotime(fn);

	ZVAL_COPY_VALUE(&strtotime.params[0], value);

	return strtotime.mixed();
}

str_rc 
datetime_obj::date(str_ptr dfmt, zend_long timeval)
{
	fn_call 	   fnc(DTData.date);
	fn_params<2>   datefmt(fnc);

	val_ptr::string_bind(&datefmt.params[0], dfmt);
	ZVAL_LONG(&datefmt.params[1], timeval);

	return datefmt.str();
}

str_rc //static
datetime_obj::date(str_ptr dfmt, zval* value)
{
	str_rc result;

	val_rc timeval = strtotime(value);

	if (val_ptr(timeval).isLong())
	{
		result = datetime_obj::date(dfmt, timeval.zlong());
	}
	return result;
}

static void lodge_error(const char* msg)
{
	zend_throw_error(zend_ce_error,"datetime_obj error %s", msg);
}

datetime_obj::datetime_obj(str_ptr zs)
{
	if (!make_obj())
	{
		return;
	}

	std::string_view vs = zs.vstr();
	auto dobj = php_date_obj_from_obj(obj_);

	if (!php_date_initialize(dobj, vs.data(), vs.size(), nullptr, nullptr, 0) )
	{
		lodge_error("Failed to init");
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
		lodge_error("Failed to init");
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
		lodge_error("Failed to init");
	}
	/*if (! php_date_initialize(dobj, nullptr, 0, nullptr, nullptr, 0) )
	{
		throw std::logic_error("Failed to init DateTime object");
	}
	*/
}

void 
datetime_obj::setDate(int year, int month, int day)
{
	fn_call      fnc(DTData.setdate, obj_);

	fn_params<3> set_date(fnc);

	ZVAL_LONG(&set_date.params[0], year);
	ZVAL_LONG(&set_date.params[1], month);
	ZVAL_LONG(&set_date.params[2], day);
	set_date.call_fn();
}

void 
datetime_obj::setTime(int hour, int minute, int second, int millisec)
{
	fn_call  fnc(DTData.settime, obj_);

	fn_params<4> set_time(fnc);

	ZVAL_LONG(&set_time.params[0], hour);
	ZVAL_LONG(&set_time.params[1], minute);
	ZVAL_LONG(&set_time.params[2], second);
	ZVAL_LONG(&set_time.params[3], millisec);
	set_time.call_fn();
}


str_rc 
datetime_obj::format(str_ptr dfmt)
{
	obj_ptr caller(obj_);
	val_rc  arg1(dfmt);

	str_rc result = caller.call(DTData.formatkey, arg1);

	return result;
}

void datetime_obj::setTimeZone(str_ptr zone)
{
	timezone_obj tz(zone);
	obj_ptr  temp(obj_);

	temp.call(DTData.settimezone, val_rc(tz));
}

datetime_obj::datetime_obj(obj_rc fnret)
{
	obj_ = fnret;
	//TODO: assert is datetime object
}

dt_interval 
datetime_obj::diff(datetime_obj& dtm)
{
	dt_interval result;

	fn_call fnc(DTData.diff, obj_);
	fn_params<1> diffobj(fnc);

	val_ptr::object_bind(&diffobj.params[0],(zend_object*) dtm);

	if (diffobj.call_fn())
	{
		result = std::move(diffobj.result_);
	}
	else {
		diffobj.throw_failed();
	}
	return result;

}

timezone_obj::timezone_obj(str_ptr zone)
{
	if (!make_obj())
	{
		return;
	}
	obj_ptr temp(obj_);
	val_rc  arg1(zone);
	
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

dt_interval::dt_interval() : obj_rc() {}

dt_interval::dt_interval(zval&& m) : obj_rc() 
{	
	zval* p = (zval*) &m;
	val_ptr grab(p);
	obj_ptr test =  grab.zobject();

	if (test.instanceof(php_date_get_interval_ce()))
	{
		obj_ = test;
		*p = {0};
	}
	else {
		zend_throw_error(zend_ce_error,"Object not a datetime_interval");
	}
	// ?? else m is unchanged
}

long 
dt_interval::years()
{
	val_rc temp = obj_ptr(obj_).property(DTData.y_prop);
	return val_ptr(temp).zlong();
}

long 
dt_interval::months()
{
	val_rc temp = obj_ptr(obj_).property(DTData.m_prop);
	return val_ptr(temp).zlong();
}
long 
dt_interval::days()
{
	val_rc temp = obj_ptr(obj_).property(DTData.d_prop);
	return val_ptr(temp).zlong();
}
long 
dt_interval::hours()
{
	val_rc temp = obj_ptr(obj_).property(DTData.h_prop);
	return val_ptr(temp).zlong();
	
}
long 
dt_interval::minutes()
{
	val_rc temp = obj_ptr(obj_).property(DTData.i_prop);
	return val_ptr(temp).zlong();
	
}
long 
dt_interval::seconds()
{
	val_rc temp = obj_ptr(obj_).property(DTData.s_prop);
	return val_ptr(temp).zlong();
}

double 
dt_interval::fraction()
{
	val_rc temp = obj_ptr(obj_).property(DTData.f_prop);
	return val_ptr(temp).zdouble();
	
}

val_rc 
dt_interval::daystotal()
{
	return obj_ptr(obj_).property(DTData.days_prop);
}

 double  //static - taken from ext/standard/microtime.c
 datetime_obj::microtime()
 {
 	struct timeval tp = {0};
 	gettimeofday(&tp, nullptr);
	return (double)(tp.tv_sec + tp.tv_usec / 1.0e6);
 }

}; // namespace

//datetime.cpp
#endif