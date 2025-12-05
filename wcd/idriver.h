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

		enum {
			FETCH_OBJECT = PDO_FETCH_OBJ,
			FETCH_ASSOC = PDO_FETCH_ASSOC,
			FETCH_NUM = PDO_FETCH_NUM,
		};

		static base_obj_mgr<IDriver> omg;
		
		void construct(obj_ptr icfg, str_ptr name);
		void destruct();

		virtual void afterConnect();
		virtual str_rc getSqlType();
		virtual void debug_info(htab_rw di);
		
		static int pdo_type(unsigned int ztype);
		
		bool begin();
		
		void bind(val_ptr stmt, htab_ptr params);
		void close();
		
		bool commit();
		
		error_return connect();

		bool closeStmt(val_ptr stmt);

		str_rc escape(str_ptr value);
		val_return execute(val_ptr stmt, bool close = true, bool fetch = false);

		htab_rc fetchAllRows(val_ptr stmt, int mode);
		val_rc fetchRow(val_ptr stmt, int mode);

		val_rc getAttribute(int key);

		val_rc getCaseAttribute();

		void setCaseAttribute(int value);

		htab_return getColumnNames(str_ptr tableName);
		htab_rc getConnectOptions();

		str_return getDSN();

		

		str_rc getDatabaseName();

		int getFetch();

		obj_rc getSchema();
		str_rc getSchemaClass();
		htab_return getTableColumns(str_ptr tableName);
		obj_rc getTableMode(str_ptr tableName);

		htab_rc getTableNames();

		val_rc handle();

		obj_rc isql();
		obj_rc iconfig();
		

		bool inTransaction();
		bool isAutoCommit();
		bool isConnected();


		val_rc lastInsertId();
		val_return lastSeqValue(str_ptr name);

		void log(htab_ptr info);

		str_rc modelClassName(str_ptr tableName);

		obj_rc newBindings();
		obj_rc newDmlBuild();
		obj_rc newParamList();
		str_rc param(int pno);

		val_return prepare(str_ptr query);

		void   prepareExecute(str_ptr query, htab_ptr values, htab_ptr bindTypes);

		val_return prepareQuery(str_ptr query, htab_ptr values, htab_ptr bindTypes);

		val_return query(str_ptr query, htab_ptr params);

		val_return querySingle(str_ptr query);

		

		str_rc quoteName(str_ptr name);

		obj_rc readSchema();

		bool rollback();

		bool setAttribute(int key, val_ptr value);
		int  setFetch(int mode);

		error_return transaction();

		obj_rc getTableModel(str_ptr tableName);

		str_rc lastSQL() const;
		
		IConfig* 	icfg_c();
		ISql*       isql_c();

		str_ptr getName() const { return name_; }
	protected:



		obj_rc    icfg_;
		str_rc    name_;
		str_rc	  db_name_;
		obj_rc    isql_;

		val_rc 	   handle_;

		int         ifetch_;
		bool        logging_;
		str_rc    	lastsql_;

		htab_rc    table_models_;
		obj_rc     schema_def_;

		



		friend class IBuild;

	};

};

#endif