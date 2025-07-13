#ifndef WCD_IBUILD_H
#define WCD_IBUILD_H



#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif


namespace wcd {

	using namespace zpp;
	using namespace wcc;

	class Bindings;
	class ISql;

	class IBuild : public base_d {
	public:

		static base_obj_mgr<IBuild> omg;

		void construct(zval_user driver);
		void destruct();

		zval_mgr aggregate(zstr_user agfn, htab_read columns);
		zval_mgr get(htab_read columns);
		
		void setModel(zobj_user m);

		

		zval_mgr oneRow();
		zval_mgr allRows();

		zval_mgr first(htab_read columns);
		void where(zval_user column, zstr_user bop, zval_user value, zstr_user bval);


		void whereKeyValue(zval_user key, zval_user value);
		void whereRaw(zobj_user rawobj, htab_read values, zstr_user bval);
		
		zobj_mgr getInsertSql(htab_read columns);
		zval_mgr update(zobj_user irow, htab_read dirty);

		void setReturns(htab_read names);
		zval_mgr insert(zval_user rdata);

		void limit(int lim, int offset=0);
		void offset(int value);
		void orderBy(zstr_user colname, bool descend=false);
		
		zval_mgr deleteRow(zobj_user rowobj);
		
		int count(zval_user columns);

		void table(zstr_user table, bool wipe=true);

		void distinct(bool set = true);

		bool hasModel() const {
			return model_.ok();
		}

		zstr_mgr now();

		zobj_mgr driver_;
		zobj_mgr isql_;
		zobj_mgr params_;
		zobj_mgr bindings_;

		zobj_mgr model_;
		zstr_mgr modelClass_;

		htab_mgr columns_;

	protected:

		Bindings& bindings();
		ISql&     isql();
		IDriver&  idb();



		int  ifetch_;

		void where(zval_user column, zval_user bop, zval_user value, zval_user bval);

		void where_unpack(htab_read aw);
		void where_list(htab_read aw);

		
		zval_mgr get_first();

		friend class Model;
	};
}; //  namespace


#endif