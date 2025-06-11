#ifndef WCD_IDRIVER_H
#define WCD_IDRIVER_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

#ifndef WCD_ICONFIG_H
#include "iconfig.h"
#endif

namespace wcd {

	using namespace zpp;
	using namespace wcc;

	class IDriver : public base_d {

	public:
		static base_obj_mgr<IDriver> omg;
		
		void construct(zobj_user icfg, zstr_user name);
		void destruct();
		void begin();
		void bind(zval_user stmt, htab_read params);
		void close();
		void closeStmt(zval_user stmt);
		bool commit();
		void connect();

		zstr_mgr escape(zstr_user value);
		zstr_mgr execute(zval_user stmt, bool close, bool fetch);

		htab_mgr fetchAllRows(zval_user stmt, int mode);
		zval_mgr fetchRow(zval_user stmt, int mode);

		zval_mgr getAttribute(int key);
		zval_mgr getCaseAttribute();
		zval_mgr getColumnNames(zstr_mgr tableName);
		htab_mgr getConnectOptions();

		zstr_mgr getDSN();

		zstr_mgr getDatabaseName();

		int getFetch();

		zobj_mgr getSchema();
		zstr_mgr getSchemeClass();
		htab_mgr getTableColumns(zstr_mgr tableName);
		zobj_mgr getTableMode(zstr_mgr tableName);

		htab_mgr getTableNames();

		zval_mgr handle();

		zobj_mgr iSql();

		bool inTransaction();
		bool isAutoCommit();
		bool isConnected();


		zval_mgr lastInsertId();
		zval_mgr lastSeqValue(zstr_mgr name);

		void log(htab_read info);

		zstr_mgr modelClassName(zstr_mgr tableName);

		zobj_mgr newDmlBuild();
		zstr_mgr param(int pno);

		zval_mgr prepare(zstr_user query);
		void  	 prepareExecute(zstr_user query, htab_read values, htab_read bindTypes);
		zval_mgr  prepareQuery(zstr_user query, htab_read values, htab_read bindTypes);

		zval_mgr query(zstr_user query, htab_read params);
		zval_mgr querySingle(zstr_user query);

		zstr_mgr quoteName(zstr_user name);

		zobj_mgr readSchema();

		bool rollback();

		void setAttribute(int key, zval_user value);
		void setFetch(int mode);

		bool transaction();

		
	protected:
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

		IConfig*    iconfig();



		friend class IBuild;

	};

};

#endif