#ifndef SQL_ISQL_H
#define SQL_ISQL_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

#ifndef SQL_IPART_H
#include "sql_ipart.h"
#endif

namespace wcd {

	using namespace zpp;

	class Bindings;

	extern zend_class_entry* zclass_join_info;
	extern zend_class_entry* zclass_join_tables;
	extern zend_class_entry* zclass_bindings;
	extern zend_class_entry* zclass_param_list;

	class JoinInfo : public base_d {
	protected:
		obj_rc leftTable_; // IColumns
		obj_rc rightTable_; // ?IColumns
		int      joinType_;  
		htab_rc joinExpr_;

	public:
		enum {
			J_ERROR = -1,
			J_INNER = 0,
			J_LEFT = 1,
			J_RIGHT = 2,
			J_FULL = 3
		};

		friend class JoinTables;

		static base_obj_mgr<JoinInfo> omg;
		
		static int_return getJoinType(str_ptr s);
		static str_ptr  joinStr(int jid);

		IColumns* leftTable();
		IColumns* rightTable();

		void construct(obj_ptr ltable, const obj_ptr rtable, int jtype=J_INNER);
		void destruct();

		void add(val_ptr lexp, val_ptr rexp, int jtype=J_INNER, int logic = JoinExpr::B_NULL);

		void addExpr(obj_ptr jexpr);

		str_ptr joinTypeStr() const;

		htab_ptr getConditions() {
			return joinExpr_;
		}
	};


	class JoinTables : public base_d {
	protected:

		htab_rc byAlias_;
		htab_rc results_;
		
		htab_rc where_;
		htab_rc joins_;
		htab_rc orderby_;

		obj_rc prime_;
		str_rc model_;
	public:

		static base_obj_mgr<JoinTables> omg;

		static obj_rc rowSplit(htab_ptr row, htab_ptr rename);

		void destruct();

		void debug_info(htab_rw di) override;
		
		void setPrime(obj_ptr obj);

		obj_rc addJoin(obj_ptr jiobj);


		error_return addTable(obj_ptr icol);

		void addResult(obj_ptr ta);

		void addWhere(val_ptr leftAttr, val_ptr rightAttr, int op, int logic);

		htab_ptr getData()
		{
			return joins_;
		}

		str_ptr getModel()
		{
			return model_;
		}

		htab_ptr getOrder() 
		{
			return orderby_;
		}

	    obj_rc getPivot();

		obj_rc getPrime()
		{
			return prime_;
		}

		obj_rc getTable(str_ptr name);
		
		obj_rc getTableAlias(str_ptr name);

		htab_ptr getTables()
		{
			return byAlias_;
		}

		void order(str_ptr name, bool descend = false);



		void setModel(str_ptr name)
		{
			model_ = name;
		}

	};



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

	extern zend_class_entry* zintf_ce_Sql_IfSql;
	
}; // namespace

extern bool init_isql_module();

//sql_isql.h
#endif

