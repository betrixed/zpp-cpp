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

		zstr_intern  get_sql_params;	
		zstr_intern  idriver;
		zstr_intern  getfetch;
		zstr_intern  setfetch;

		zstr_intern  fetch_key;
		zstr_intern  modelclass;
		zstr_intern  join_tables;
		zstr_intern  valuesdefault;
		zstr_intern  eager_load;
		zstr_intern  and_str;
		zstr_intern  count_str;
		zstr_intern  statement;
		
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

		val_rc   lattr_; 
		val_rc   rattr_;
		int        op_;
		int        nextop_;

		static base_obj_mgr<JoinExpr> omg;

		virtual void debug_info(htab_rw di);

		static  str_ptr  opStr(int op);
		static  str_ptr  boolStr(int nextop);

		static  int       toLogic(str_ptr s);
		static  int       toOperator(str_ptr s);

		JoinExpr() : SqlPartId(JE_PID) {}


		void construct(val_ptr leftval, val_ptr rightval, int op, int nextop);

	
		/*zstr_own emit(int ix, zobj_own bindobj, 
				str_ptr Lalias, str_ptr Ralias);*/

		str_rc emit(int ix, Bindings* bind, 
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
	protected:
		val_rc  value_;
	public:

		static base_obj_mgr<Param> omg;

		virtual void debug_info(htab_rw di);
		
		Param() :  SqlPartId(PARAM_PID) {}

		void construct(val_ptr zp);

		val_ptr getValue() const {
			return value_;
		}
	};

	class Literal : public SqlPartId 
	{
	public:

		val_rc   value_;

		// Is this needed for an abstract class?
		static base_obj_mgr<Literal> omg;

		virtual void debug_info(htab_rw di);

		Literal() : SqlPartId(LIT_PID) {}

		void construct(val_ptr val);

		val_ptr getValue() const { return value_; }

		str_rc toString() const override;
	};
	
	class IColumns : public SqlPartId 
	{
	protected:
		str_rc 	alias_;
		htab_rc  	colnames_;
		htab_rc 	expr_;
		obj_rc 	owner_;
	public:

		static base_obj_mgr<IColumns> omg;

		virtual void debug_info(htab_rw di);

		IColumns() : SqlPartId(ICOL_PID) {}

		void construct(obj_ptr owner);

		void clear();

		obj_ptr getOwner() const
		{
			return owner_;
		}

		htab_rd getColNames() const
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

		void add(htab_rd columns);

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

		htab_rd getExpr() const
		{
			return expr_;
		}

	};

	class TColumns : public IColumns 
	{
	protected:
		str_rc 	name_;
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