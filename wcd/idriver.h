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
		
		void construct(zobj_user icfg, zstr_user name);
		void destruct();

		virtual void afterConnect();
		virtual zstr_mgr getSqlType();

		static int pdo_type(unsigned int ztype);
		
		bool begin();
		
		void bind(zval_user stmt, htab_read params);
		void close();
		void closeStmt(zval_user stmt);
		bool commit();
		void connect();

		zstr_mgr escape(zstr_user value);
		zval_mgr execute(zval_user stmt, bool close = true, bool fetch = false);

		htab_mgr fetchAllRows(zval_user stmt, int mode);
		zval_mgr fetchRow(zval_user stmt, int mode);

		zval_mgr getAttribute(int key);

		zval_mgr getCaseAttribute();

		void setCaseAttribute(int value);

		htab_mgr getColumnNames(zstr_user tableName);
		htab_mgr getConnectOptions();

		zstr_mgr getDSN();

		

		zstr_mgr getDatabaseName();

		int getFetch();

		zobj_mgr getSchema();
		zstr_mgr getSchemaClass();
		htab_mgr getTableColumns(zstr_user tableName);
		zobj_mgr getTableMode(zstr_user tableName);

		htab_mgr getTableNames();

		zval_mgr handle();

		zobj_mgr isql();
		zobj_mgr iconfig();
		

		bool inTransaction();
		bool isAutoCommit();
		bool isConnected();


		zval_mgr lastInsertId();
		zval_mgr lastSeqValue(zstr_user name);

		void log(htab_read info);

		zstr_mgr modelClassName(zstr_user tableName);

		zobj_mgr newDmlBuild();
		zstr_mgr param(int pno);

		zval_mgr prepare(zstr_user query);
		void  	 prepareExecute(zstr_user query, htab_read values, htab_read bindTypes);
		zval_mgr prepareQuery(zstr_user query, htab_read values, htab_read bindTypes);

		zval_mgr query(zstr_user query, htab_read params);
		zval_mgr querySingle(zstr_user query);

		zstr_mgr quoteName(zstr_user name);

		zobj_mgr readSchema();

		bool rollback();

		bool setAttribute(int key, zval_user value);
		int  setFetch(int mode);

		bool transaction();

		zobj_mgr getTableModel(zstr_user tableName);

		
	protected:

		IConfig* 	icfg_c();
		ISql*       isql_c();

		zobj_mgr    icfg_;
		zstr_mgr    cfg_name_;
		zstr_mgr	db_name_;
		zobj_mgr    isql_;

		zval_mgr 	handle_;

		int         ifetch_;
		bool        logging_;
		zstr_mgr    lastsql_;

		htab_mgr    table_models_;

		zobj_mgr    schema_def_;

		



		friend class IBuild;

	};

};

#endif