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
		zobj_mgr leftTable_; // IColumns
		zobj_mgr rightTable_; // ?IColumns
		int      joinType_;  
		htab_mgr joinExpr_;

	public:
		enum {
			J_INNER = 0,
			J_LEFT = 1,
			J_RIGHT = 2,
			J_FULL = 3
		};

		friend class JoinTables;

		static base_obj_mgr<JoinInfo> omg;
		
		static int getJoinType(zstr_user s);
		static zstr_user  joinStr(int jid);

		IColumns* leftTable();
		IColumns* rightTable();

		void construct(zobj_user ltable, const zobj_user rtable, int jtype=J_INNER);

		void add(zval_user lexp, zval_user rexp, int jtype=J_INNER, int logic = JoinExpr::B_NULL);

		void addExpr(zobj_user jexpr);

		zstr_user joinTypeStr() const;

		htab_read getConditions() {
			return joinExpr_;
		}
	};


	class JoinTables : public base_d {
	protected:

		htab_mgr byAlias_;
		htab_mgr results_;
		
		htab_mgr where_;
		htab_mgr joins_;
		htab_mgr orderby_;

		zobj_mgr prime_;
		zstr_mgr model_;


	public:

		static base_obj_mgr<JoinTables> omg;

		static zobj_mgr rowSplit(htab_read row, htab_read rename);

		void debug_info(htab_write di) override;
		
		void setPrime(zobj_user obj);

		zobj_mgr addJoin(zobj_user jiobj);


		void addTable(zobj_user icol);

		void addResult(zobj_user ta);

		void addWhere(zval_user leftAttr, zval_user rightAttr, int op, int logic);

		const htab_mgr& getData()
		{
			return joins_;
		}

		const zstr_mgr& getModel()
		{
			return model_;
		}

		const htab_mgr& getOrder() 
		{
			return orderby_;
		}

	    zobj_mgr getPivot();

		zobj_mgr getPrime()
		{
			return prime_;
		}

		zobj_mgr getTable(zstr_user name);
		
		zobj_mgr getTableAlias(zstr_user name);

		const htab_mgr& getTables()
		{
			return byAlias_;
		}

		void order(zstr_user name, bool ascend);



		void setModel(zstr_user name)
		{
			model_ = name;
		}

	};



	class ParamList : public base_d {
	protected:
		/** IDriver object */
		zobj_mgr	driver_;
		/** SQL param values */
		htab_mgr    params_;
		/** generated SQL */
		zstr_mgr    sql_;
		/** values of params */
		htab_mgr    val_params_;
		/** return values */
		htab_mgr	ret_values_;

		zstr_mgr paramStr(int ct);

	public:

		static base_obj_mgr<ParamList>  omg;

		void debug_info(htab_write di) override;

		void construct(zobj_user driver);
		zstr_mgr addParam(zval_user value);
		//zstr_mgr addParam(zval_user value);
		zstr_mgr addParamList(htab_read values);
		zstr_mgr makeList(int start, int count);

		void setParams(htab_read replace)
		{
			params_ = replace;
		}

		void wipe();

		/** add parameter or constant */
		zstr_mgr paramLiteral(zval_user value);

		void setReturns(htab_read rets)
		{
			ret_values_ = rets;
		}

		void setValues(htab_read vals)
		{
			
			val_params_ = vals;
		}

		void useOwnValues();

		void setSql(zstr_user s)
		{
			sql_ = s;
		}

		zstr_user getSql() 
		{
			return sql_;
		}

		htab_read getReturns() 
		{
			return ret_values_;
		}

		htab_read getValues() 
		{
			return val_params_;
		}

		htab_read getParams() 
		{
			return params_;
		}
	};


	class ISql : public base_d 
	{
	protected:
		zstr_mgr columns(htab_read bd);
		void columnsTC(IColumns* tc, htab_write col_list); //zstr_buffer& col_list);
		htab_read getTables(Bindings& bind);
		zstr_mgr where(Bindings &bind, htab_read wtab);
		zstr_mgr insert_col_params(Bindings& bind, htab_read rowbind);
		zstr_mgr orderBy(htab_read obind);
		zstr_mgr limit(ParamList* plist, htab_read ltab);
		zstr_mgr fromJT(Bindings& bind, JoinTables* jt);
		zstr_mgr select_jt(Bindings& bind, JoinTables* jt);

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

		static zstr_mgr tableClass(zstr_user s);

		zstr_mgr seqLastValue(zstr_user seq);

		zstr_mgr setSeqValue(int value, htab_read data);
		
		zstr_mgr entityClass(zstr_user s);

		zobj_mgr deleteSql(Bindings& bind);

		zobj_mgr insert(Bindings& bind);

		zobj_mgr select(Bindings& bind);

		zstr_mgr truncate(Bindings& bind);

		zobj_mgr update(Bindings& bind);

		zstr_mgr quoteName(zstr_user name);

		zstr_mgr emit(zval_user sp, Bindings* bind, zstr_user lalias, zstr_user ralias);

		zstr_mgr getTruncateSql();

		zstr_mgr valuesDefault();

	};


	class Bindings : public base_d
	{
	protected:
		htab_mgr	data_;
		zobj_mgr    paramList_;
		zobj_mgr	sql_;
		zobj_mgr    db_;

		void addToArray(int key, zval_user value);

	public:
		static base_obj_mgr<Bindings>  omg;

		void debug_info(htab_write di) override;

		void construct(zobj_user sql, zobj_user connect);

		void add(int key, zval_user value);

		void addarray(int key, htab_read value);
		void addstr(int key, zstr_user value);

		bool addJoinData(htab_read data);

		zval* get(int key);

		const zobj_mgr& isql() {
			return sql_;
		}

		bool aliasSelect();

		void limit(zval_user limit, zval_user offset);
		void limit(int limit, int offset=0);

		void whereKeyValue(zval_user keys, zval_user values);
		void where(zval_user column, zstr_user opstr, zval_user value, zstr_user blogic);

		htab_mgr columnAlias(zobj_user tcol);

		JoinTables* getJoinTables();

		zobj_mgr getJoins();

		bool getArray(int key, htab_mgr& value);

		void offset(int value);

		void orderBy(zstr_user column, bool descend=false);

		void update(zstr_user column, zval_user value);

		const htab_mgr& getData()
		{
			return data_;
		}

		zval_mgr select();

		/** Call setPrime of JoinTables, return JoinTables */
		zobj_mgr primeJoin(zval_user tcol);
		/*
		ParamList* paramList()
		{
			return zobj_toc<ParamList>(paramList_);
		}
		*/

		zobj_user getParamList();

		void set(int key, zval_user value);
		void set(int key, int value);
		void set(int key, htab_read value);
		void set(int key, const zval_mgr& value);

		void setParamList(zobj_user obj)
		{
			paramList_ = obj;
		}

		void unset(int key);

		void wipe(int key = 0);
	};

	extern zend_class_entry* zintf_ce_Sql_IfSql;
	
}; // namespace

extern bool init_isql_module();

//sql_isql.h
#endif

