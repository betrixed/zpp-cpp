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
		zstr_mgr lang_str_;

		zstr_mgr money_sym_;

		// money format object for language and currency
		zobj_mgr money_fmt_; 

	public:

		static base_obj_mgr<MoneyFmt> omg;

		virtual void debug_info(htab_write hw);
		
		void construct(zstr_user slang);

		zstr_mgr formatNoSym(zval_user value);

		zstr_mgr format(zval_user value);

		zstr_user symbol() const;

		zstr_user language() const;

#ifndef BASE_ZOBJPTR
		VIRTUAL_ZOBJPTR
#endif
	};

	
}; // namespace

//money_fmt.h
#endif