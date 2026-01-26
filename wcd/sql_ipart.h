#ifndef SQL_PART_H
#define SQL_PART_H


#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

#include <vector>

namespace wcd {

	using namespace zpp;
	

	class sql_strtab : public state_init {
	public:
		str_intern  str_space;
		str_intern  partid;
		str_intern  value;
		str_intern  values_key;


		str_intern  dot_char;
		str_intern  comma_char;
		str_intern  single_quote;
		str_intern  single_q2;

		str_intern  mysql_empty;
		str_intern  default_values;
		str_intern  delete_from;

		str_intern  emit;

		str_intern  lhs_val;
		str_intern  rhs_val;
		str_intern  val_op;
		str_intern  next_op;

		str_intern  namekey;
		str_intern  valuekey;
		str_intern  quotename;
		str_intern  alias;
		str_intern  columns;

		str_intern  colexpr;
		str_intern  owner;
		str_intern  table;
		str_intern  attr;

		str_intern  isql;
		str_intern  param;

		str_intern  ok;
		str_intern  asc;
		str_intern  desc;

		str_intern  limit;
		str_intern  offset;

		str_intern  column;
		str_intern  boolean;
		str_intern  operator_key;
		str_intern  nested;

		str_intern  function;
		str_intern  seq_key;

		str_intern  condition1;
		str_intern  condition2;

		str_intern  not_key;
		str_intern  asterisk;
		str_intern  basic;
		str_intern  jointype;

		str_intern  typekey;
		str_intern  data_key;

		str_intern  is_null;
		str_intern  is_not_null;
		str_intern  op_like;
		str_intern  op_and;
		str_intern  op_or;
		str_intern  cmp_equal;

		str_intern getTableModel;
		str_intern getColDefs;

		str_intern param_list;
		str_intern connect;

		str_intern driver;
		str_intern params;
		str_intern sql;
		str_intern returns_str;
		str_intern return_str;
		str_intern bind_key;
		str_intern id_key;
		str_intern default_key;
		str_intern auto_inc;

		str_intern  results;
		str_intern  by_alias;
		str_intern  model;
		str_intern  joins;
		str_intern  prime;
		str_intern  orderby;
		str_intern  where;

		str_intern  get_sql_params;	
		str_intern  idriver;
		str_intern  getfetch;
		str_intern  setfetch;

		str_intern  fetch_key;
		str_intern  modelclass;
		str_intern  join_tables;
		str_intern  valuesdefault;
		str_intern  eager_load;
		str_intern  and_str;
		str_intern  count_str;
		str_intern  statement;
		str_intern  db_ref;

		str_intern  queryString;
		str_intern  icolumns;
		str_intern  auto_alias;
		
		std::vector<str_intern> opstr;
		std::vector<str_intern> boolstr;
		std::vector<str_intern> joinstr;


		void init() override;
	};


	/*
	enum {
		OP_ERROR = -1,
	    OP_EQ = 0,
	    OP_NEQ = 1,
	    OP_GT = 2,
	    OP_LT = 3,
	    OP_GE = 4,
	    OP_LE = 5,
	    OP_LIKE = 6,
	    OP_AND = 7,
	    OP_OR = 8,
	    OP_ISNULL = 9,
	    OP_NOTNULL = 10,
	    OP_NOP = 11
	};
	*/
	
	class SqlPartId : public base_d
	{
	protected:
		int partid_;
	public:
		enum {
		// from ext/fileinfo/libmagic/magic.h
			LIT_PID = 1,
			EXPR_PID = 2,
			JE_PID = 3,
			TA_PID = 4,
			PARAM_PID = 5,
			ICOL_PID = 6,
			TCOL_PID = 7
		};

		SqlPartId(int id) {
			partid_ = id;
		}

		int getPartId() 
		{
			return partid_;
		}

	};

	typedef bireturn<SqlPartId*> SqlPart_return;



	class Expr : public SqlPartId 
	{
	public:

		str_rc   expr_;

		// Is this needed for an abstract class?
		Expr() : SqlPartId(EXPR_PID) {}

		static base_obj_mgr<Expr> omg;

		virtual void debug_info(htab_rw di);

		void construct(str_ptr val);

		str_rc toString() const override { return expr_; }
	};

	class Bindings;
	
	class JoinExpr : public SqlPartId 
	{
	public:
		enum {
			OP_ERROR = -1,
		    OP_EQ = 0,
		    OP_NEQ = 1,
		    OP_GT = 2,
		    OP_LT = 3,
		    OP_GE = 4,
		    OP_LE = 5,
		    OP_LIKE = 6,
		    OP_AND = 7,
		    OP_OR = 8,
		    OP_ISNULL = 9,
		    OP_NOTNULL = 10,
		    OP_NOP = 11
		};
		
		enum {
			B_ERROR = -1,
		    B_NULL = 0,
		    B_AND = 1,
		    B_OR = 2
		};

		val_rc   lattr_; 
		val_rc   rattr_;
		int        op_;
		int        nextop_;

		static base_obj_mgr<JoinExpr> omg;

		virtual void debug_info(htab_rw di);

		static  str_ptr  opStr(int op);
		static  str_ptr  boolStr(int nextop);

		static  int_return       toLogic(str_ptr s);
		static  int_return       toOperator(str_ptr s);

		JoinExpr() : SqlPartId(JE_PID) {}


		void construct(val_ptr leftval, val_ptr rightval, int op, int nextop);
		void destruct();
	
		/*zstr_own emit(int ix, zobj_own bindobj, 
				str_ptr Lalias, str_ptr Ralias);*/

		str_return emit(int ix, Bindings* bind, 
				str_ptr Lalias, str_ptr Ralias);
	};

	class TableAttr : public SqlPartId 
	{
	protected:
		str_rc table_;
		str_rc attr_;
	public:

		static obj_rc makeTA(str_ptr t, str_ptr a);
		static val_rc splitDot(str_ptr s);

		static base_obj_mgr<TableAttr> omg;

		virtual void debug_info(htab_rw di);

		TableAttr() : SqlPartId(TA_PID) {}

		void construct(str_ptr t, str_ptr a);

		str_rc toString() const override;


		str_ptr getTable() const
		{
			return table_;
		}

		str_ptr getAttr() const
		{
			return attr_;
		}

	};

	class Param : public SqlPartId
	{
	public:

		static base_obj_mgr<Param> omg;

		virtual void debug_info(htab_rw di);
		
		Param() :  SqlPartId(PARAM_PID) {}

		void construct(val_ptr zp);

		val_ptr getValue() const;
	};

	class Literal : public SqlPartId 
	{
	public:
		// Is this needed for an abstract class?
		static base_obj_mgr<Literal> omg;

		virtual void debug_info(htab_rw di);

		Literal() : SqlPartId(LIT_PID) {}

		void construct(val_ptr val);

		val_ptr getValue() const;

		str_rc toString() const override;
	};
	
	class IColumns : public SqlPartId 
	{
	protected:
		str_rc 		alias_;
		htab_rc  	colnames_;
		htab_rc 	expr_;
		weak_ref 	owner_; // a WeakReference
	public:

		static base_obj_mgr<IColumns> omg;

		virtual void debug_info(htab_rw di);

		IColumns() : SqlPartId(ICOL_PID) {}

		void construct(obj_ptr owner);

		virtual void destruct();

		void clear();

		obj_rc getOwner();

		void setOwner(obj_ptr obj);

		htab_ptr getColNames() const
		{
			return colnames_;
		}

		void setAlias(str_ptr name)
		{
			alias_ = name;
		}

		str_ptr getAlias() const
		{
			return alias_;
		}

		// The alias is the name here
		virtual str_ptr getName() const
		{
			return alias_;
		}

		void setColAlias(str_ptr name, str_ptr alias);

		void add(htab_ptr columns);

		void unsetCol(str_ptr key)
		{
			htab_rw names(colnames_);
			names.unset(key);
		}

		bool has(str_ptr key) const
		{
			return colnames_.has_key(key);
		}

		bool hasColNames() const
		{
			return (colnames_.size() > 0);
		}

		void addExpr(str_ptr alias, str_ptr expr)
		{
			htab_rw exp_w(expr_);
			exp_w.set(alias, expr);
		}

		htab_ptr getExpr() const
		{
			return expr_;
		}
	};

	class TColumns : public IColumns 
	{
	protected:
		str_rc 		name_;
		htab_rc 	attr_map_;
	public:
		virtual void debug_info(htab_rw di);

		static base_obj_mgr<TColumns> omg; // can't be same name as base??

		static val_rc tableCol(str_ptr expr);

		TColumns() : IColumns()
		{
			partid_ = TCOL_PID;
		}

		void construct(str_ptr tname, str_ptr talias,  val_ptr tcol);
		void destruct() override;

		// return/create TableAttr of name
		val_rc attr(str_ptr name); 

		void setName(str_ptr name);

		virtual str_ptr getName() const
		{
			return this->name_;
		}
	};
	extern sql_strtab SQSTR;

	extern zend_class_entry* zclass_sql_ifipart;

	extern zend_class_entry* zclass_join_expr;

	extern zend_class_entry* zclass_table_attr;

	extern zend_class_entry* zclass_sql_icolumns;

	extern zend_class_entry* zclass_sql_tcolumns;
};

#endif