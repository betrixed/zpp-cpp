#ifndef WCD_IBUILD_H
#define WCD_IBUILD_H



#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif


namespace wcd {

	using namespace zpp;
	using namespace wcc;

	class Bindings;

	class IBuild : public base_d {
	public:

		base_obj_mgr<IBuild> omg;

		void construct(zval_user driver);
		void destruct();

		zval_mgr aggregate(zstr_user agfn, htab_read columns);
		zval_mgr get(htab_read columns);
		
		void setModel(zobj_user m);

		Bindings& bindings();

		zval_mgr oneRow();
		zval_mgr allRows();

		zval_mgr first(htab_read columns);
		void where(zval_user column, zstr_user bop, zval_user value, zstr_user bval);

		zobj_mgr getInsertSql(htab_read columns);
		

	protected:

		zobj_mgr driver_;
		zobj_mgr isql_;
		zobj_mgr params_;
		zobj_mgr bindings_;

		zobj_mgr model_;
		zstr_mgr modelClass_;

		htab_mgr columns_;

		int  ifetch_;


		friend class Model;
	};
}; //  namespace


#endif