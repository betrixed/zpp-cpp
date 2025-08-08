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

		virtual void debug_info(htab_wr di);
		
		void construct(val_ptr driver);
		void destruct();

		val_rc aggregate(str_ptr agfn, htab_rd columns);
		val_rc get(htab_rd columns);

		val_rc oneRow();
		val_rc allRows();

		val_rc first(htab_rd columns);
		void where(val_ptr column, str_ptr bop, val_ptr value, str_ptr bval);


		void whereKeyValue(val_ptr key, val_ptr value);
		void whereRaw(obj_ptr rawobj, htab_rd values, str_ptr bval);
		
		obj_rc getInsertSql(htab_rd columns);
		val_rc update(obj_ptr irow, htab_rd dirty);

		void setReturns(htab_rd names);
		val_rc insert(val_ptr rdata);

		void limit(int lim, int offset=0);
		void offset(int value);
		void orderBy(val_ptr colname, bool descend=false);
		
		val_rc deleteRow(obj_ptr rowobj);
		
		int count(val_ptr columns);

		void table(str_ptr table, bool wipe=true);

		void distinct(bool set = true);

		bool hasModel() const {
			return model_.ok();
		}

		str_rc now();

		val_rc seqLastValue(str_ptr seqname);

		void set(str_ptr cname, val_ptr value);

		int setFetch(int mode);

		void setInsert(htab_rd data);

		void setModel(obj_ptr obj, bool bind = true);

		val_rc setSeqValue(int value, htab_rd data);

		void wipe();
		
		obj_rc driver_;
		obj_rc isql_;
		obj_rc bindings_;

		obj_rc model_;
		str_rc modelClass_;

		htab_rc columns_;

	protected:

		Bindings& bindings();
		ISql&     isql();
		IDriver&  idb();



		int  ifetch_;

		void where(val_ptr column, val_ptr bop, val_ptr value, val_ptr bval);

		void where_unpack(htab_rd aw);
		void where_list(htab_rd aw);

		
		val_rc get_first();

		friend class Model;
	};
}; //  namespace


#endif