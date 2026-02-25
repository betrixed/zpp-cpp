#ifndef WCD_BINDINGS_H
#define WCD_BINDINGS_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

namespace wcd
{
	using namespace zpp;

	class JoinTables;
	
	class Bindings : public base_d
	{
	protected:
		htab_rc	  data_;
		obj_rc    paramList_;
		obj_rc	  isql_;
		obj_rc    db_;
		weak_ref  dbref_;

		void addToArray(int key, val_ptr value);
		obj_return getDb();

	public:
		static base_obj_mgr<Bindings>  omg;

		void debug_info(htab_rw di) override;

		void construct(obj_ptr sql, weak_ref& connect);
		void destruct();

		void add(int key, val_ptr value);

		void addarray(int key, htab_ptr value);
		void addstr(int key, str_ptr value);

		bool_return addJoinData(htab_ptr data);

		zval* get(int key);

		obj_ptr isql() {
			return isql_;
		}

		bool_return aliasSelect();

		void limit(val_ptr limit, val_ptr offset);
		void limit(int limit, int offset=0);

		error_return whereKeyValue(val_ptr keys, val_ptr values);
		void where(val_ptr column, str_ptr opstr, val_ptr value, str_ptr blogic);

		htab_return columnAlias(obj_ptr tcol);

		JoinTables* getJoinTables();

		obj_rc getJoins();

		bool getArray(int key, htab_rc& value);

		void offset(int value);

		void orderBy(val_ptr column, 
			bool descend=false, 
			bool nullslast=false);

		void update(str_ptr column, val_ptr value);

		htab_ptr getData()
		{
			return data_;
		}

		val_return select();

		/** Call setPrime of JoinTables, return JoinTables */
		obj_rc primeJoin(val_ptr tcol);
		/*
		ParamList* paramList()
		{
			return zobj_toc<ParamList>(paramList_);
		}
		*/

		obj_return getParamList();

		void set(int key, val_ptr value);
		void set(int key, int value);
		void set(int key, htab_ptr value);
		void set(int key, const val_rc& value);

		void setParamList(obj_ptr obj)
		{
			paramList_ = obj;
		}

		void unset(int key);

		void wipe(int key = -1);
	};

}//namespace wcd

#endif
	