#ifndef WCD_IDRIVER_H
#define WCD_IDRIVER_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

#ifndef WCD_ICONFIG_H
#include "iconfig.h"
#endif

#ifndef PHP_PDO_DRIVER_H
#include <ext/pdo/php_pdo_driver.h>
#endif

namespace wcd {

	using namespace zpp;
	using namespace wcc;

	class IConfig;
	class ISql;

	class IDriver : public base_d {
	public:
		static void notImplementedMsg(str_buf& buf, const char* fn);
		static void throw_not_implemented(const char* fn);
		
		enum {
			FETCH_OBJECT = PDO_FETCH_OBJ,
			FETCH_ASSOC = PDO_FETCH_ASSOC,
			FETCH_NUM = PDO_FETCH_NUM,
			FETCH_COLUMN = PDO_FETCH_COLUMN
		};

		static base_obj_mgr<IDriver> omg;
		
		/** Check result set return expected types ?? */
		static error_return check_results(val_ptr val);

		void construct(obj_ptr icfg, str_ptr name);
		void destruct();

		virtual void afterConnect();

		// functions which cannot be "pure" = 0
		virtual str_rc getSqlType();
		virtual htab_return getTableNames();
		virtual str_return getDSN();

		virtual void debug_info(htab_rw di);
		
		virtual val_return lastSeqValue(str_ptr name);
		virtual str_rc getSchemaClass();

		static int pdo_type(unsigned int ztype);
		
		virtual bool begin();
		virtual bool commit();
		virtual bool rollback();

		virtual void bind(val_ptr stmt, htab_ptr params);

		virtual void close();

		virtual error_return connect();

		virtual bool closeStmt(val_ptr stmt);

		virtual str_rc escape(str_ptr value);

		virtual val_return execute(val_ptr stmt, bool close = true, bool fetch = false);

		virtual htab_return fetchAllRows(val_ptr stmt, int mode);
		virtual val_rc fetchRow(val_ptr stmt, int mode);

		virtual bool inTransaction();
		virtual bool isAutoCommit();
		virtual bool isConnected();
		virtual val_return lastInsertId(str_ptr name);

		htab_return getColumnNames(str_ptr tableName);
		
		str_rc getDatabaseName();

		int getFetch();

		obj_rc getSchema();
		
		htab_return getTableColumns(str_ptr tableName);
		obj_rc getTableMode(str_ptr tableName);

		

		val_return handle();

		obj_ptr isql();
		obj_ptr iconfig();
		




		
		

		void log(htab_ptr info);

		str_rc modelClassName(str_ptr tableName);

		obj_rc newBindings();
		obj_rc newDmlBuild();
		obj_rc newParamList();
		str_rc param(int pno);

		virtual val_return prepare(str_ptr query, htab_ptr options=htab_ptr());

		error_return  prepareExecute(str_ptr query, htab_ptr values, htab_ptr bindTypes);

		virtual val_return prepareQuery(str_ptr query, htab_ptr values, htab_ptr bindTypes);

		virtual val_return query(str_ptr query, htab_ptr params);

		virtual val_return querySingle(str_ptr query);

		weak_ref selfRef() 
		{
			return wkself_;
		}

		str_rc quoteName(str_ptr name);

		obj_rc readSchema();

		int  setFetch(int mode);

		virtual error_return transaction();

		obj_rc getTableModel(str_ptr tableName);

		str_rc lastSQL() const;
		
		IConfig* 	icfg_c();
		ISql*       isql_c();

		str_ptr getName() const;
	protected:


		weak_ref  wkself_; // hold own reference
		
		// POINTERS TO PROPERTY STORAGE for read
		val_ptr   name_ptr_;
		val_ptr   handle_ptr_;
		val_ptr   cfg_ptr_;
		val_ptr   logging_ptr_;
		val_ptr   lastsql_ptr_;

		/*
		obj_rc    icfg_;
		str_rc    name_;
		*/

		str_rc	  db_name_;
		obj_rc    isql_;
		int       ifetch_;


		htab_rc    table_models_;
		obj_rc     schema_def_;

		



		friend class IBuild;

	};


	class DBSInit : public state_init {
	public:

		str_intern  query_fn;
		str_intern  fetch_fn;
		str_intern  close_cursor;
		str_intern  pdo_prefix;
		str_intern  pdo_class;

		str_intern  errorcode_fn;
		str_intern  begin_trans;
		str_intern  bind_value;
		str_intern  commit_fn;

		str_intern  quote_fn;
		str_intern  regex_quoted;
		str_intern  rx_cap1;
		str_intern  execute_fn;
		str_intern  fetchall_fn;
		str_intern  rowcount_fn;

		str_intern  mysql_str;
		str_intern  intransaction_fn;
		str_intern  lastinsertid_fn;
		str_intern  place_holder;

		str_intern  prepare_fn;
		str_intern  error_str;
		str_intern  readschema_fn;
		str_intern  rollback_fn;

		str_intern  cfg_name;
		str_intern  db_name;
		str_intern  tbl_models;
		str_intern  iconfig_key;
		str_intern  schema_def;
		str_intern  sqlgen_s;
		
		str_intern  handle_s;
		str_intern  logging_s;
		str_intern  lastsql_s;


		void init() override;
	};

	extern DBSInit DBS;
}//wcd namespace

#endif