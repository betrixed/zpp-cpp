#ifndef WCD_NAMEDPARAMS_CPP
#define WCD_NAMEDPARAMS_CPP

#ifndef WCD_NAMEDPARAMS_H
#include "namedparams.h"
#endif

#ifndef WCD_IPARAMS_ARGINFO
#define WCD_IPARAMS_ARGINFO
extern "C" {
	#include "stub/iparams_arginfo.h"
}
#endif

#define DBG_LOG_NAMEDPARAMS
#ifdef DBG_LOG_NAMEDPARAMS
#	ifndef WCC_DEBUG_LOG_H
#		include "wcc/debuglog.h"
#	endif
#endif

namespace wcd {

base_obj_mgr<NamedParams> 	NamedParams::omg;



NamedParams::NamedParams() : IParams()
{

}

NamedParams::~NamedParams()
{
	#ifdef DBG_LOG_NAMEDPARAMS

	#endif
}

str_rc 
NamedParams::n_param(int ct)
{
	str_rc result;
	str_buf buf;

	buf << ":p" << iform(Numf::DEC) << ct;
	result = buf.zstr();
	return result;
}

str_rc 
NamedParams::paramStr(int ct)
{
	return n_param(ct);
}

zend_class_entry* 
NamedParams::register_class(zend_class_entry* ce_iparams)
{
	auto ce = register_class_Wcd_Sql_NamedParams(ce_iparams);
	
	NamedParams::omg.classEntry(ce);

	return ce;
}

str_rc 
NamedParams::addParamEquals(val_ptr value)
{
	str_rc result;
	htab_rw hw(params_);

	unsigned nextid = params_.size()+1;
	result = paramStr(nextid);
	hw.set(result, value);

	return result;
}


}//namespace wcd


using namespace zpp;
using namespace wcd;



ZEND_METHOD(Wcd_Sql_NamedParams, n_param)
{
	zarg_rd args(execute_data);

	zend_long value = 0;

	args.zlong(value, args.need(0));

	if (!args.throw_errors())
	{
		str_rc result = NamedParams::n_param(value);
		result.move_zv(return_value);
	}
}

ZEND_METHOD(Wcd_Sql_NamedParams, paramStr)
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

ZEND_METHOD(Wcd_Sql_NamedParams, addParamEquals)
{
	zarg_rd args(execute_data);
	val_ptr  value = args.need(0);

	if (!args.throw_errors())
	{
		IParams* cobj = zval_toc<IParams>(ZEND_THIS);
		str_rc result = cobj->addParamEquals(value);
		result.move_zv(return_value);
	}
}
#endif//namedparams.cpp