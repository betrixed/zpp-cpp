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

	extern zend_class_entry* zclass_isql;
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
		
		static int getJoinType(str_ptr s);
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


		void addTable(obj_ptr icol);

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

		void order(str_ptr name, bool ascend);



		void setModel(str_ptr name)
		{
			model_ = name;
		}

	};



	class ParamList : public base_d {
	protected:
		/** IDriver object */
		obj_rc	driver_;
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

		void construct(obj_ptr driver);
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


	class ISql : public base_d 
	{
	protected:
		str_rc columns(htab_ptr bd);
		void columnsTC(IColumns* tc, htab_rw col_list); //str_buf& col_list);
		htab_ptr getTables(Bindings& bind);
		str_rc where(Bindings &bind, htab_ptr wtab);
		str_rc insert_col_params(Bindings& bind, htab_ptr rowbind);
		str_rc orderBy(htab_ptr obind);
		str_return limit(ParamList* plist, htab_ptr ltab);
		str_rc fromJT(Bindings& bind, JoinTables* jt);
		str_rc select_jt(Bindings& bind, JoinTables* jt);

	public:
		enum {
			SQL_OBJ    = 0, 
		    SQL_INSERT = 1,
		    SQL_UPDATE = 2,
		    SQL_DELETE = 3,
		    SQL_SELECT = 4,
		    SQL_WHERE = 5,
		    SQL_JOIN = 6,
		    SQL_RETURN = 7,
		    SQL_FROM = 8,
		    SQL_ORDER = 9,
		    SQL_LIMIT = 10,
		    SQL_AGGREGATE = 11,
		    SQL_DISTINCT = 12,
		    SQL_RENAME = 13,
		    FETCH_AS = 14,
    		MODEL_OBJ = 15,
    		MODEL_CLASS = 16,
    		NAME_LIST = 17
		};
		enum {
			QUOTE_SGL = '\'',
			QUOTE_DBL = '"'
		};

		static base_obj_mgr<ISql>  omg;

		static str_rc tableClass(str_ptr s);

		str_rc seqLastValue(str_ptr seq);

		str_rc setSeqValue(int value, htab_ptr data);
		
		str_rc entityClass(str_ptr s);

		obj_rc deleteSql(Bindings& bind);

		obj_rc insert(Bindings& bind);

		obj_rc select(Bindings& bind);

		str_rc truncate(Bindings& bind);

		obj_rc update(Bindings& bind);

		str_rc quoteName(str_ptr name);

		str_rc emit(val_ptr sp, Bindings* bind, str_ptr lalias, str_ptr ralias);

		str_rc getTruncateSql();

		str_rc valuesDefault();

	};


	class Bindings : public base_d
	{
	protected:
		htab_rc	data_;
		obj_rc  paramList_;
		obj_rc	isql_;
		obj_rc  db_;
		str_rc  dbname_;

		void addToArray(int key, val_ptr value);
		obj_rc getDb();
		IDriver& dbref();

	public:
		static base_obj_mgr<Bindings>  omg;

		void debug_info(htab_rw di) override;

		void construct(obj_ptr sql, obj_ptr connect);
		void destruct();

		void add(int key, val_ptr value);

		void addarray(int key, htab_ptr value);
		void addstr(int key, str_ptr value);

		bool_return addJoinData(htab_ptr data);

		zval* get(int key);

		obj_ptr isql() {
			return isql_;
		}

		

		bool aliasSelect();

		void limit(val_ptr limit, val_ptr offset);
		void limit(int limit, int offset=0);

		void whereKeyValue(val_ptr keys, val_ptr values);
		void where(val_ptr column, str_ptr opstr, val_ptr value, str_ptr blogic);

		htab_rc columnAlias(obj_ptr tcol);

		JoinTables* getJoinTables();

		obj_rc getJoins();

		bool getArray(int key, htab_rc& value);

		void offset(int value);

		void orderBy(val_ptr column, bool descend=false);

		void update(str_ptr column, val_ptr value);

		htab_ptr getData()
		{
			return data_;
		}

		val_rc select();

		/** Call setPrime of JoinTables, return JoinTables */
		obj_rc primeJoin(val_ptr tcol);
		/*
		ParamList* paramList()
		{
			return zobj_toc<ParamList>(paramList_);
		}
		*/

		obj_ptr getParamList();

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

