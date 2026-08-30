#ifndef WCD_SIMPLEPARAMS_CPP
#define WCD_SIMPLEPARAMS_CPP

#ifndef WCC_DEBUGLOG_H
#include "wcc/debuglog.h"
#endif

#ifndef WCD_IPARAMS_H
#include "iparams.h"
#endif

#ifndef WCD_IPARAMS_ARGINFO
#define WCD_IPARAMS_ARGINFO
extern "C" {
	#include "stub/iparams_arginfo.h"
}
#endif

//#define DBG_LOG_IPARAMS
#ifdef DBG_LOG_IPARAMS
#	ifndef WCC_DEBUG_LOG_H
#		include "wcc/debuglog.h"
#	endif
#endif

namespace wcd {

base_obj_mgr<IParams> 	IParams::omg;

class IPInit : public state_init {
public:
	str_intern param_str;

	void init() override;

};

IPInit IPStr;

void 
IPInit::init()
{
	param_str = "?";
}

void IParams::debug_info(htab_rw di)
{
	if (!params_.isNull())
		di.set(SQSTR.params, params_);
	if (!ret_values_.isNull())
		di.set(SQSTR.returns_str, ret_values_);
	if (!sql_.isNull())
		di.set(SQSTR.sql, sql_);
	if (!val_params_.isNull())
		di.set(SQSTR.values_key, val_params_);
}

IParams::~IParams()
{

}

void IParams::destruct()
{
	#ifdef DBG_LOG_IPARAMS
	DebugLog* log = DebugLog::cpp_global();

	if (log)
	{
		log->dump("IParams __destruct", self_);
	}
	#endif
	wipe();
}

str_rc 
IParams::paramStr(int ct)
{
	return IPStr.param_str;
}

zend_class_entry* 
IParams::register_class()
{
	auto ce = register_class_Wcd_Sql_IParams();
	
	IParams::omg.classEntry(ce);

	STATE_INIT_ADD(IPStr);

	return ce;
}

str_rc 
IParams::addParamEquals(val_ptr value)
{
	str_rc result = IPStr.param_str;
	htab_rw hw(params_);

	hw.push_back(result);
	
	return result;
}

void 
IParams::setSql(str_ptr s)
{
	sql_ = s;
}

str_ptr 
IParams::getSql()
{
	return sql_;
}

void 
IParams::useOwnValues()
{
	val_params_ = params_;
}

str_return
IParams::paramLiteral(val_ptr value)
{
	str_return result;

	val_ptr temp;

	if (value.isObject())
	{
		obj_ptr obj(value.zobject());

		if (obj.instanceof(zclass_sql_ifipart)) 
		{
			SqlPartId* part = zobj_toc<SqlPartId>(obj);

			switch(part->getPartId())
			{
			case SqlPartId::PARAM_PID:
				temp = static_cast<Param*>(part)->getValue();
				result = this->addParamEquals(temp);
				break;
			case SqlPartId::LIT_PID:
				temp = static_cast<Literal*>(part)->getValue();
				result = val_ptr(temp).to_zstr();
				break;
			}
		}

	}
	else {
		result = this->addParamEquals(value);
	}
	if (!result.value_.size()) {
		result.error() << "Unhandled paramLiteral argument";
	}
	return result;
}

str_rc 
IParams::addParamList(htab_ptr values)
{
	str_buf buf;

	int ix = (int) params_.size();

	htab_walk wk;
	auto item = wk.value();

	int bufct = 0;

	htab_rw pw(params_);

	for(wk.start(values); wk.ok(); wk.next(), bufct++)
	{
		pw.push_back(item);
		ix++;
		if (bufct > 0)
		{
			buf << ',';
		}
		buf << paramStr(ix);
	}
	return buf.zstr();
}

str_rc 
IParams::makeList(int start, int count)
{
	str_buf buf;
	for(int ix = start; ix <= count; ix++)
	{
		if (ix > start)
		{
			buf << ',';
		}
		buf << paramStr(ix);
	}
	return buf.zstr();
}

void
IParams::wipe()
{
	
	sql_.init();
	params_ = htab_ptr::empty_array();
	ret_values_ = htab_ptr::empty_array();
	val_params_ = htab_ptr::empty_array();
}

}//namespace wcd


using namespace zpp;
using namespace wcd;


/* public function addParam(mixed $value) : void {} */
ZEND_METHOD(Wcd_Sql_IParams, addParamEquals)
{
	zarg_rd args(execute_data);
	zval* value = args.need(0);

	if (!args.throw_errors())
	{
		IParams* cobj = zval_toc<IParams>(ZEND_THIS);
		str_rc result = cobj->addParamEquals(value);
		result.move_zv(return_value);
	}
}


ZEND_METHOD(Wcd_Sql_IParams, param)
{
	zarg_rd args(execute_data);

	zend_long value = 0;

	args.zlong(value, args.need(0));

	if (!args.throw_errors())
	{
		IParams* cobj = zval_toc<IParams>(ZEND_THIS);
		str_rc result = cobj->paramStr(value);
		result.move_zv(return_value);
	}
}

ZEND_METHOD(Wcd_Sql_IParams, paramStr)
{
	zarg_rd args(execute_data);
	zend_long value = 0;

	args.zlong(value, args.need(0));


	if (!args.throw_errors())
	{
		IParams* cobj = zval_toc<IParams>(ZEND_THIS);
		str_rc result = cobj->paramStr(value);
		result.move_zv(return_value);
	}
}

/* public function addParamList(array $values): string {} */
ZEND_METHOD(Wcd_Sql_IParams, addParamList)
{
	zval* value;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(value)
	ZEND_PARSE_PARAMETERS_END();
	IParams* cobj = zval_toc<IParams>(ZEND_THIS);
	str_rc result = cobj->addParamList(value);
	result.move_zv(return_value);
}

/* public function getParams() : array {} */
ZEND_METHOD(Wcd_Sql_IParams, getParams)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IParams* cobj = zval_toc<IParams>(ZEND_THIS);
	htab_ptr htab = cobj->getParams();
	htab.copy_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_IParams, getReturns)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IParams* cobj = zval_toc<IParams>(ZEND_THIS);
	htab_ptr htab = cobj->getReturns();
	htab.copy_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_IParams, getSql)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IParams* cobj = zval_toc<IParams>(ZEND_THIS);
	str_ptr sql = cobj->getSql();
	sql.copy_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_IParams, getValues)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IParams* cobj = zval_toc<IParams>(ZEND_THIS);
	htab_ptr htab = cobj->getValues();
	htab.copy_zv(return_value);
}

/*public function makeList(int $start, int $count): string {}*/
ZEND_METHOD(Wcd_Sql_IParams, makeList)
{
	zend_long start;
	zend_long count;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_LONG(start)
	Z_PARAM_LONG(count)
	ZEND_PARSE_PARAMETERS_END();

	IParams* cobj = zval_toc<IParams>(ZEND_THIS);

	str_rc result = cobj->makeList(start, count);
	result.move_zv(return_value);
}

/* public function paramLiteral(mixed $value) : string {} */
ZEND_METHOD(Wcd_Sql_IParams, paramLiteral)
{
	zval* value;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	IParams* cobj = zval_toc<IParams>(ZEND_THIS);

	str_return result = cobj->paramLiteral(value);
	result.throw_errors();
	result.value_.move_zv(return_value);
}

/* public function setParams(array $replace) : void {} */
ZEND_METHOD(Wcd_Sql_IParams, setParams)
{
	zval* value;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(value)
	ZEND_PARSE_PARAMETERS_END();

	IParams* cobj = zval_toc<IParams>(ZEND_THIS);

	cobj->setParams(value);
}

/* public function setReturns(array $replace) : void {}*/
ZEND_METHOD(Wcd_Sql_IParams, setReturns)
{
	zval* value;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(value)
	ZEND_PARSE_PARAMETERS_END();

	IParams* cobj = zval_toc<IParams>(ZEND_THIS);

	cobj->setReturns(value);
}

ZEND_METHOD(Wcd_Sql_IParams, setSql)
{
	str_ptr sql;

	zarg_rd args(execute_data);

	args.zstring(sql, args.need(0));

	if (!args.throw_errors())
	{
		IParams* cobj = zval_toc<IParams>(ZEND_THIS);
		cobj->setSql(sql);
	}	
}

ZEND_METHOD(Wcd_Sql_IParams, setValues)
{
	zval* values;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(values)
	ZEND_PARSE_PARAMETERS_END();

	IParams* cobj = zval_toc<IParams>(ZEND_THIS);

	cobj->setValues(values);
}

/* public function useOwnValues() : void {} */
ZEND_METHOD(Wcd_Sql_IParams, useOwnValues)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IParams* cobj = zval_toc<IParams>(ZEND_THIS);
	cobj->useOwnValues();
}

/* public function wipe() : void {} */
ZEND_METHOD(Wcd_Sql_IParams, wipe)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IParams* cobj = zval_toc<IParams>(ZEND_THIS);
	cobj->wipe();
}

ZEND_METHOD(Wcd_Sql_IParams, __destruct)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IParams* cobj = zval_toc<IParams>(ZEND_THIS);
	cobj->destruct();
}
#endif//iparams.cpp