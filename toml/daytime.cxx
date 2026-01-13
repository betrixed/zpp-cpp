#ifndef DAYTIME_CPP
#define DAYTIME_CPP

#ifndef DAYTIME_H
#include "daytime.h"
#endif

#ifndef TOML_ARGINFO_H
#define TOML_ARGINFO_H
extern "C" {
	#include "toml_arginfo.h"
}
#endif

namespace wcc {

class D24Init : public state_init {
public:
	D24Init() : state_init() {}

	void init() override 
	{
		value_key = "value";
		format_str = "format_str";
	}

	str_intern value_key;
	str_intern format_str;

};

D24Init D24;

day24_obj::day24_obj(str_ptr zs) : obj_rc()
{
	*((obj_rc*)(this)) = Day24::omg.new_zobj();
	Day24* cobj = zobj_toc<Day24>(obj_);
	cobj->construct(zs);
}

str_rc 
day24_obj::format(int flags)
{
	Day24* cobj = zobj_toc<Day24>(obj_);
	return cobj->format(flags);
}


base_obj_mgr<Day24> omg;




bool
Day24::day24_time(int hour, int min, double sec, double& ret, bool except) {

	const char* error;

	if (hour < 0 || min < 0 || sec < 0.0) {
		error = "daytime values cannot be negative";
	}
	else if (hour >= 24) {
		error = "hours must be less than 24";
	}
	else if (min >= 60) {
		error = "minutes must be less than 60";
	}
	else if (sec >= 60.0) {
		error = "seconds must be less than 60";
	}
	else {
		error = nullptr;
	}
	if (error && except) 
	{
		zend_throw_error(zend_ce_exception,"bad day24 string %s",error);
		ret = 0.0;
		return false;
	}
	else {
		ret = (hour*60.0*60.0 + min*60.0 + sec)/(24.0*60*60);
		return true;
	}
}

bool //static
Day24::day24_str(const char* s, long slen, double& ret, bool except) 
{
	long hours = 0;
	long mins = 0;
	double secs = 0.0;
	char* eptr;
	const char* iptr;

    iptr = s;
	hours = strtol(iptr, &eptr, 10);
	if (*eptr == ':' && ((eptr - iptr) == 2)) {
		iptr = eptr+1;
		mins = strtol(iptr, &eptr, 10);
		if (*eptr == ':' && ((eptr - iptr) == 2)) {
			iptr = eptr+1;
			secs = strtod(iptr, &eptr);
		}
	}

	return day24_time(hours, mins, secs, ret, except);

}

void //static
Day24::day24_split(double val, int* h24, int* min, double* sec) {
	double frs = val*24.0*60*60;
	*h24 = floor(frs/(60.0*60));
	*min = floor(frs/60.0) - (*h24)*60;
	*sec = frs - (*h24)*60.0*60 - (*min)*60;
}

zend_string* //static
Day24::day24_format(double val, int flags){
	int h24 = 0;
	int min = 0;
	double sec = 0.0;
	day24_split(val, &h24, &min, &sec);
	switch(flags) {
		case SEC_INT:
			return strpprintf(0,"%02d:%02d:%02.0f",h24,min,sec);
			break;
		case SEC_DEC:
			return strpprintf(0,"%02d:%02d:%08.6f",h24,min,sec);
			break;
		case SEC_NONE:
			return strpprintf(0,"%02d:%02d",h24,min);
			break;
		case SEC_AUTO:
		default:
			if (sec != 0.0) {
                if (sec - floor(sec) > 0.0) {
                     return strpprintf(0,"%02d:%02d:%8.6f",h24,min,sec);
                }
                else {
                    return strpprintf(0,"%02d:%02d:%02.0f",h24,min,sec);
                }
            }
            return strpprintf(0,"%02d:%02d", h24,min);
            break;
	}
}

void 
Day24::construct(str_ptr sval) 
{
	if (sval.isNull())
	{
		tval_ = 0.0;
	}
	else {
		this->str(sval);	
	}
}

void 
Day24::str(str_ptr sval) {
	day24_str((const char*) sval.data(), sval.size(), tval_, true);
}

void 
Day24::time(long hours, long mins, double seconds)
{
	day24_time(hours, mins, seconds, tval_, true);
}

void 
Day24::day(double dval) 
{
	if (dval < 0.0 || dval >= 1.0) {
		zend_throw_exception(zend_ce_exception,"Bounds 0.0 <= %lg <= 1.0", dval);
	}
	tval_ = dval;
}

void 
Day24::split(val_ptr hours, val_ptr mins, val_ptr seconds)
{
	int h24;
	int m60;
	double s60;

	day24_split(tval_, &h24, &m60, &s60);
	ZVAL_LONG(Z_REFVAL_P(hours), h24);
	ZVAL_LONG(Z_REFVAL_P(mins), m60);
	ZVAL_DOUBLE(Z_REFVAL_P(seconds), s60);
}

str_rc 
Day24::format(int flags)
{
	str_rc result;

	result.adopt(day24_format(tval_,flags));
	return  result;
}

str_rc 
Day24::toString() const
{
	str_rc result;
	result.adopt(day24_format(tval_, SEC_AUTO));
	return result;
}


void Day24::debug_info(htab_rw di)
{
	di.set(D24.value_key, (double) tval_);
	di.set(D24.format_str, this->toString());
}

};//namespace wcc

using namespace wcc;



ZEND_METHOD(Wcc_Day24, __construct)
{
	zend_string* s;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(s)
	ZEND_PARSE_PARAMETERS_END();
	auto cobj = zval_toc<Day24>(ZEND_THIS);
	cobj->construct(s);
}

ZEND_METHOD(Wcc_Day24, str)
{
	zend_string* s;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(s)
	ZEND_PARSE_PARAMETERS_END();
	auto cobj = zval_toc<Day24>(ZEND_THIS);
	cobj->str(s);
}

ZEND_METHOD(Wcc_Day24, day)
{
	double dval;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_DOUBLE(dval)
	ZEND_PARSE_PARAMETERS_END();
	auto cobj = zval_toc<Day24>(ZEND_THIS);
	cobj->day(dval);
}
ZEND_METHOD(Wcc_Day24, value)
{
	ZEND_PARSE_PARAMETERS_NONE();
	auto cobj = zval_toc<Day24>(ZEND_THIS);
	double dval = cobj->value();
	RETURN_DOUBLE(dval);
}

ZEND_METHOD(Wcc_Day24, time)
{
	
	zend_long hours;
	zend_long mins;
	double seconds;

	ZEND_PARSE_PARAMETERS_START(3, 3)
	Z_PARAM_LONG(hours)
	Z_PARAM_LONG(mins)
	Z_PARAM_DOUBLE(seconds)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Day24>(ZEND_THIS);
	cobj->time(hours, mins, seconds);
}


ZEND_METHOD(Wcc_Day24, split)
{
	zval* hours;
	zval* mins;
	zval* seconds;

	ZEND_PARSE_PARAMETERS_START(3, 3)
	Z_PARAM_ZVAL(hours)
	Z_PARAM_ZVAL(mins)
	Z_PARAM_ZVAL(seconds)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Day24>(ZEND_THIS);
	cobj->split(hours, mins, seconds);
}
ZEND_METHOD(Wcc_Day24, format)
{
	zend_long flags;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_LONG(flags)
	ZEND_PARSE_PARAMETERS_END();
	auto cobj = zval_toc<Day24>(ZEND_THIS);
	str_rc result = cobj->format(flags);
	result.move_zv(return_value);
}
ZEND_METHOD(Wcc_Day24, __toString)
{
	ZEND_PARSE_PARAMETERS_NONE();

	auto cobj = zval_toc<Day24>(ZEND_THIS);
	str_rc result = cobj->toString();
	result.move_zv(return_value);
}
ZEND_METHOD(Wcc_Day24, day24_time)
{
	zend_long hours;
	zend_long mins;
	double seconds;

	ZEND_PARSE_PARAMETERS_START(3, 3)
	Z_PARAM_LONG(hours)
	Z_PARAM_LONG(mins)
	Z_PARAM_DOUBLE(seconds)
	ZEND_PARSE_PARAMETERS_END();

	double result;
	Day24::day24_time(hours, mins, seconds, result, true);
	RETURN_DOUBLE(result);
}

ZEND_METHOD(Wcc_Day24, day24_str)
{
	zend_string* s;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(s)
	ZEND_PARSE_PARAMETERS_END();

	double result;
	Day24::day24_str(ZSTR_VAL(s), ZSTR_LEN(s), result);
	RETURN_DOUBLE(result);
}

ZEND_METHOD(Wcc_Day24, day24_split)
{
	double dval;
	zval* hours;
	zval* mins;
	zval* seconds;

	ZEND_PARSE_PARAMETERS_START(4, 4)
	Z_PARAM_DOUBLE(dval)
	Z_PARAM_ZVAL(hours)
	Z_PARAM_ZVAL(mins)
	Z_PARAM_ZVAL(seconds)
	ZEND_PARSE_PARAMETERS_END();

	int h24;
	int m60;
	double s60;

	Day24::day24_split(dval, &h24, &m60, &s60);
	ZVAL_LONG(Z_REFVAL_P(hours), h24);
	ZVAL_LONG(Z_REFVAL_P(mins), m60);
	ZVAL_DOUBLE(Z_REFVAL_P(seconds), s60);
}

ZEND_METHOD(Wcc_Day24, day24_format)
{
	zend_long flags = Day24::SEC_AUTO;
	double    dval;

	ZEND_PARSE_PARAMETERS_START(2, 2)
	Z_PARAM_DOUBLE(dval)
	Z_PARAM_OPTIONAL
	Z_PARAM_LONG(flags)
	ZEND_PARSE_PARAMETERS_END();

	str_rc result;

	result.adopt(Day24::day24_format(dval, flags));
	result.move_zv(return_value);
}


PHP_MINIT_FUNCTION(Day24_reg)
{
	zend_class_entry* ce = register_class_Day24();

	Day24::omg.classEntry(ce);

	return SUCCESS;

}

//daytime.cpp
#endif