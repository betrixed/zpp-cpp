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
		zstr_intern  str_space;
		zstr_intern  partid;
		zstr_intern  value;
		zstr_intern  values_key;


		zstr_intern  dot_char;
		zstr_intern  comma_char;
		zstr_intern  single_quote;
		zstr_intern  single_q2;

		zstr_intern  mysql_empty;
		zstr_intern  default_values;
		zstr_intern  delete_from;

		zstr_intern  emit;

		zstr_intern  lhs_val;
		zstr_intern  rhs_val;
		zstr_intern  val_op;
		zstr_intern  next_op;

		zstr_intern  namekey;
		zstr_intern  valuekey;
		zstr_intern  quotename;
		zstr_intern  alias;
		zstr_intern  columns;

		zstr_intern  colexpr;
		zstr_intern  owner;
		zstr_intern  table;
		zstr_intern  attr;

		zstr_intern  isql;
		zstr_intern  param;

		zstr_intern  ok;
		zstr_intern  ascend;
		zstr_intern  descend;

		zstr_intern  limit;
		zstr_intern  offset;

		zstr_intern  column;
		zstr_intern  boolean;
		zstr_intern  operator_key;
		zstr_intern  nested;

		zstr_intern  function;
		zstr_intern  seq_key;

		zstr_intern  condition1;
		zstr_intern  condition2;

		zstr_intern  not_key;
		zstr_intern  asterisk;
		zstr_intern  basic;
		zstr_intern  jointype;

		zstr_intern  typekey;
		zstr_intern  data_key;

		zstr_intern  is_null;
		zstr_intern  is_not_null;
		zstr_intern  op_like;
		zstr_intern  op_and;
		zstr_intern  op_or;
		zstr_intern  cmp_equal;


		zstr_intern getTableModel;
		zstr_intern getColDefs;

		zstr_intern param_list;
		zstr_intern connect;

		zstr_intern driver;
		zstr_intern params;
		zstr_intern sql;
		zstr_intern returns_str;
		zstr_intern return_str;
		zstr_intern bind_key;
		zstr_intern id_key;
		zstr_intern default_key;
		zstr_intern auto_inc;

		zstr_intern  results;
		zstr_intern  by_alias;
		zstr_intern  model;
		zstr_intern  joins;
		zstr_intern  prime;
		zstr_intern  orderby;
		zstr_intern  where;

		zstr_intern  getsql;	
		zstr_intern  idriver;
		zstr_intern  getfetch;
		zstr_intern  setfetch;

		zstr_intern  fetch_key;
		zstr_intern  modelclass;

		zstr_intern  valuesdefault;
		zstr_intern  eager_load;
		zstr_intern  and_str;
		zstr_intern  count_str;
		
		std::vector<zstr_intern> opstr;
		std::vector<zstr_intern> boolstr;
		std::vector<zstr_intern> joinstr;


		void init() override;
	};



	enum {
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



	class Expr : public SqlPartId 
	{
	public:

		zstr_mgr   expr_;

		// Is this needed for an abstract class?
		Expr() : SqlPartId(EXPR_PID) {}

		static base_obj_mgr<Expr> omg;

		virtual void debug_info(htab_write di);

		void construct(zstr_user val);

		zstr_mgr toString() const override { return expr_; }
	};

	class Bindings;
	
	class JoinExpr : public SqlPartId 
	{
	public:
		enum {
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
		    B_NULL = 0,
		    B_AND = 1,
		    B_OR = 2
		};

		zval_mgr   lattr_; 
		zval_mgr   rattr_;
		int        op_;
		int        nextop_;

		static base_obj_mgr<JoinExpr> omg;

		virtual void debug_info(htab_write di);

		static  zstr_user  opStr(int op);
		static  zstr_user  boolStr(int nextop);

		static  int       toLogic(zstr_user s);
		static  int       toOperator(zstr_user s);

		JoinExpr() : SqlPartId(JE_PID) {}


		void construct(zval_user leftval, zval_user rightval, int op, int nextop);

	
		/*zstr_own emit(int ix, zobj_own bindobj, 
				zstr_user Lalias, zstr_user Ralias);*/

		zstr_mgr emit(int ix, Bindings* bind, 
				zstr_user Lalias, zstr_user Ralias);
	};

	class TableAttr : public SqlPartId 
	{
	protected:
		zstr_mgr table_;
		zstr_mgr attr_;
	public:

		static zobj_mgr makeTA(zstr_user t, zstr_user a);
		static zval_mgr splitDot(zstr_user s);

		static base_obj_mgr<TableAttr> omg;

		virtual void debug_info(htab_write di);

		TableAttr() : SqlPartId(TA_PID) {}

		void construct(zstr_user t, zstr_user a);

		zstr_mgr toString() const override;


		zstr_user getTable() const
		{
			return table_;
		}

		zstr_user getAttr() const
		{
			return attr_;
		}

	};

	class Param : public SqlPartId
	{
	protected:
		zval_mgr  value_;
	public:

		static base_obj_mgr<Param> omg;

		virtual void debug_info(htab_write di);
		
		Param() :  SqlPartId(PARAM_PID) {}

		void construct(zval_user zp);

		zval_user getValue() const {
			return value_;
		}
	};

	class Literal : public SqlPartId 
	{
	public:

		zval_mgr   value_;

		// Is this needed for an abstract class?
		static base_obj_mgr<Literal> omg;

		virtual void debug_info(htab_write di);

		Literal() : SqlPartId(LIT_PID) {}

		void construct(zval_user val);

		zval_user getValue() const { return value_; }

		zstr_mgr toString() const override;
	};
	
	class IColumns : public SqlPartId 
	{
	protected:
		zstr_mgr 	alias_;
		htab_mgr  	colnames_;
		htab_mgr 	expr_;
		zobj_mgr 	owner_;
	public:

		static base_obj_mgr<IColumns> omg;

		virtual void debug_info(htab_write di);

		IColumns() : SqlPartId(ICOL_PID) {}

		void construct(zval_user owner);

		void clear();

		zobj_user getOwner() const
		{
			return owner_;
		}

		htab_read getColNames() const
		{
			return colnames_;
		}

		void setAlias(zstr_user name)
		{
			alias_ = name;
		}

		zstr_user getAlias() const
		{
			return alias_;
		}

		// The alias is the name here
		virtual zstr_user getName() const
		{
			return alias_;
		}

		void setColAlias(zstr_user name, zstr_user alias);

		void add(zval_user columns);

		void unsetCol(zstr_user key)
		{
			htab_write names(colnames_);
			names.unset(key);
		}

		bool has(zstr_user key) const
		{
			return colnames_.has_key(key);
		}

		bool hasColNames() const
		{
			return (colnames_.size() > 0);
		}

		void addExpr(zstr_user alias, zstr_user expr)
		{
			htab_write exp_w(expr_);
			exp_w.set(alias, expr);
		}

		htab_read getExpr() const
		{
			return expr_;
		}

	};

	class TColumns : public IColumns 
	{
	protected:
		zstr_mgr 	name_;
		htab_mgr 	attr_map_;
	public:
		virtual void debug_info(htab_write di);

		static base_obj_mgr<TColumns> omg; // can't be same name as base??

		static zval_mgr tableCol(zstr_user expr);

		TColumns() : IColumns()
		{
			partid_ = TCOL_PID;
		}

		void construct(zstr_user tname, zstr_user talias,  zval_user tcol);

		// return/create TableAttr of name
		zval_mgr attr(zstr_user name); 

		void setName(zstr_user name);

		virtual zstr_user getName() const
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