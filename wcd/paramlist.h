#ifndef WCD_PARAMLIST_H
#define WCD_PARAMLIST_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

namespace wcd
{
	using namespace zpp;

	class ParamList : public base_d {
	protected:
		/** IDriver reference */
		weak_ref   dbref_;

		/** Temporary de-ref driver */
		obj_rc     driver_; 

		/** SQL param values */
		htab_rc    params_;

		/** generated SQL */
		str_rc     sql_;

		/** values of params */
		htab_rc    val_params_;

		/** return values */
		htab_rc	ret_values_;

		str_rc paramStr(int ct);

	public:

		static base_obj_mgr<ParamList>  omg;

		void debug_info(htab_rw di) override;

		void construct(weak_ref& driver);
		str_rc addParam(val_ptr value);
		//str_rc addParam(val_ptr value);
		str_rc addParamList(htab_ptr values);
		str_rc makeList(int start, int count);

		void setParams(htab_ptr replace)
		{
			params_ = replace;
		}

		void wipe();

		/** add parameter or constant */
		str_return paramLiteral(val_ptr value);

		void setReturns(htab_ptr rets)
		{
			ret_values_ = rets;
		}

		void setValues(htab_ptr vals)
		{	
			val_params_ = vals;
		}

		void useOwnValues();

		void setSql(str_ptr s);

		str_ptr getSql();

		htab_ptr getReturns() 
		{
			return ret_values_;
		}

		htab_ptr getValues() 
		{
			return val_params_;
		}

		htab_ptr getParams() 
		{
			return params_;
		}
	};

} //namespace wcd

#endif