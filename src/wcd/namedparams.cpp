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

//#define DBG_LOG_NAMEDPARAMS
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
NamedParams::n_param(size_t ct)
{
	str_rc result;
	str_buf buf;

	buf << ":p" << iform(Numf::DEC) << ct;
	result = buf.zstr();
	return result;
}

str_rc 
NamedParams::paramStr(size_t ct)
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

str_rc //override
NamedParams::addParamEquals(val_ptr value)
{
	str_rc result;
	htab_cow hw(params_);

	unsigned nextid = params_.size()+1;
	result = paramStr(nextid);
	hw.set(result, value);

	return result;
}


str_rc //override
NamedParams::addParamList(htab_ptr list)
{
	str_rc result;

		#ifdef DBG_LOG_NAMEDPARAMS
			DebugLog* log = DebugLog::cpp_global();
			if (log) log->dump("NamedParams addlist",list);
		#endif

	str_buf text;



	size_t ct = params_.size();

	htab_cow hw(params_);
	
	bool first = true;
	//for_key_value loop;
	htab_walk wk;
	auto listval = wk.value();
	for(wk.start(list); wk.ok(); wk.next())
	{
		ct++;
		str_rc pstr = paramStr(ct);
		hw.set(pstr, listval);
		if (first)
		{
			first = false;
		}
		else {
			text << ',';
		}
		text << pstr;
	}
	#ifdef DBG_LOG_NAMEDPARAMS
		if (log) log->dump("params_",params_,5);
	#endif
	result = text.zstr();
	return result;
}


}//namespace wcd


using namespace zpp;
using namespace wcd;



ZEND_METHOD(Wcd_Sql_NamedParams, n_param)
{
	zarg_rd args(execute_data);

	zend_long value = args.zlong(args.need(0));

	if (!args.throw_errors())
	{
		str_rc result = NamedParams::n_param(value);
		result.move_zv(return_value);
	}
}



#endif//namedparams.cpp