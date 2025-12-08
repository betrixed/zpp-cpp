#ifndef WCD_IBUILD_H
#define WCD_IBUILD_H



#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

#ifndef BIRETURN_H
#include  "zpp/bireturn.h"
#endif

#ifndef SQL_PART_H
#include "sql_ipart.h"
#endif



namespace wcd {

	using namespace zpp;

	typedef bireturn<val_rc> val_return;

	class Bindings;
	class ISql;

	class IBuild : public base_d {
	public:

		static base_obj_mgr<IBuild> omg;

		virtual void debug_info(htab_rw di);
		
		void construct(obj_ptr driver);
		void destruct();

		obj_return    getDb();

		val_return aggregate(str_ptr agfn, htab_ptr columns);
		val_return get(htab_ptr columns);

		val_return oneRow();
		val_return allRows();

		val_return first(htab_ptr columns);

		error_return where(val_ptr column, str_ptr bop, val_ptr value, str_ptr bval);
		error_return whereKeyValue(val_ptr key, val_ptr value);
		error_return whereRaw(obj_ptr rawobj, htab_ptr values, str_ptr bval);
		
		obj_return getInsertSql(htab_ptr columns);
		val_return update(obj_ptr irow, htab_ptr dirty);

		void setReturns(htab_ptr names);
		val_return insert(val_ptr rdata);

		void limit(int lim, int offset=0);
		void offset(int value);
		void orderBy(val_ptr colname, bool descend=false);
		
		val_return deleteRow(obj_ptr rowobj);
		
		int_return count(val_ptr columns);

		void table(str_ptr table, bool wipe=true);

		void distinct(bool set = true);

		bool hasModel() const {
			return model_.ok();
		}

		str_rc now();

		val_return seqLastValue(str_ptr seqname);

		val_return setSeqValue(int value, htab_ptr data);

		void set(str_ptr cname, val_ptr value);

		int setFetch(int mode);

		void setInsert(htab_ptr data);

		error_return setModel(obj_ptr obj, bool bind = true);

		

		void wipe();
		
		// weak_reference object
		weak_ref dbref_; 
		obj_rc   driver_;
		obj_rc 	 isql_;
		obj_rc   bindings_;

		obj_rc model_;
		str_rc modelClass_;

		htab_rc columns_;

	protected:

		Bindings& bindings();
		ISql&     isql();
		IDriver&  idb();


		int  ifetch_;

		error_return where(val_ptr column, val_ptr bop, val_ptr value, val_ptr bval);
		error_return where_unpack(htab_ptr aw);
		error_return where_list(htab_ptr aw);

		
		val_return get_first();

		friend class Model;
	};


	
}; //  namespace


#endif