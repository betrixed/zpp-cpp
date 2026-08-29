#ifndef MONEY_FMT_H
#define MONEY_FMT_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif


namespace wcc {

	/**
	 * This simple object supports a local currency formatter.
	 * It a ssumes a dollar currency.
	 * Values are assumed to be in cents.
	 * Division by 100 required for formatting.
	 * Language string eg 'en-AU' for setup.
	 */
	class MoneyFmt : public base_d {
	protected:
		str_rc lang_str_;

		str_rc money_sym_;

		// money format object for language and currency
		obj_rc money_fmt_; 

	public:

		static base_obj_mgr<MoneyFmt> omg;

		virtual void debug_info(htab_rw hw);
		
		void construct(str_ptr slang);

		str_rc formatNoSym(val_ptr value);

		str_rc fmtValue(val_ptr value);
		
		str_rc format(val_ptr value);

		str_ptr symbol() const;

		str_ptr language() const;

		VIRTUAL_ZOBJPTR
	};

	
}; // namespace

//money_fmt.h
#endif