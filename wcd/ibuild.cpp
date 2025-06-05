#ifndef WCD_IBUILD_CPP
#define WCD_IBUILD_CPP

#ifndef WCD_IBUILD_H
#include "ibuild.h"
#endif


namespace wcd {
using namespace zpp;


	void IBuild::construct(zval_user driver)
	{
		idriver_ = driver;
		params_ = ParamList::omg.new_zobj();
		ParamList* plist = zobj_toc<ParamList>(params_);

		plist->construct(driver);

		IDriver* db = zobj_toc<IDriver>(idriver_);
		isql_ = db->isql_;

		bindings_ = Bindings::omg.new_zobj();
		Bindings* bind = zobj_toc<Bindings>(bindings_);

		zval_mgr sqlmgr(isql_);

		bind->construct(sqlmgr, driver);
		bind->setParamList(params_);

	}

	void IBuild::destruct()
	{
		isql_.init();
		driver_.init();
		params_.init();
		bindings_.init();
		model_.init();
	}

	zval_mgr
	IBuild::aggregate(zstr_user agfn, htab_read columns)
	{
		model_.init();

		htab_mgr args_mgr;
		htab_write args(args_mgr);

		args.set(IBS.function_key, afgfn);
		args.set(IBS.columns_key, columns);

		Bindings* bind = zobj_toc<Bindings>(bindings_);
		bind->set(ISql::SQL_AGGREGATE, args_mgr);

		ifetch_ = 

	}

	zval_mgrIBuild:: get(htab_read columns)
	{

	}
};

#endif