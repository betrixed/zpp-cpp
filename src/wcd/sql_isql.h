#ifndef SQL_ISQL_H
#define SQL_ISQL_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

#ifndef SQL_IPART_H
#include "sql_ipart.h"
#endif

#ifndef WCD_BINDINGS_H
#include "bindings.h"
#endif

#ifndef WCD_IPARAMS_H
#include "iparams.h"
#endif

namespace wcd {

	using namespace zpp;

	extern zend_class_entry* zclass_join_info;
	extern zend_class_entry* zclass_join_tables;
	extern zend_class_entry* zclass_bindings;


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

	extern zend_class_entry* zintf_ce_Sql_IfSql;
	
}; // namespace

extern bool init_isql_module();

//sql_isql.h
#endif

