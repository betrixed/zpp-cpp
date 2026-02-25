#ifndef WCD_PDODRIVER_H
#define WCD_PDODRIVER_H

#ifndef WCD_IDRIVER_H
#include "idriver.h"
#endif

#ifndef PHP_PDO_DRIVER_H
#include <ext/pdo/php_pdo_driver.h>
#endif

namespace wcd {

	using namespace zpp;
	using namespace wcc;


	class PdoDriver : public IDriver {
	public:

		enum {
			FETCH_OBJECT = PDO_FETCH_OBJ,
			FETCH_ASSOC = PDO_FETCH_ASSOC,
			FETCH_NUM = PDO_FETCH_NUM,
			FETCH_COLUMN = PDO_FETCH_COLUMN
		};

		static base_obj_mgr<PdoDriver> omg;

		static zend_class_entry* register_class(zend_class_entry* idriver_ce);

		virtual void afterConnect();

		virtual htab_rc getConnectOptions();

		static int pdoType(val_ptr val);
		
		 bool begin() override;
		 bool commit() override;
		 bool rollback() override;

		htab_return fetchAllRows(val_ptr stmt, int mode) override;
		val_rc fetchRow(val_ptr stmt, int mode) override;

		 error_return transaction() override;

		void bind(val_ptr stmt, htab_ptr params) override;
		error_return connect() override;

		bool closeStmt(val_ptr stmt) override;

		str_rc escape(str_ptr value);
		val_return execute(val_ptr stmt, bool close = true, bool fetch = false);



		val_rc getAttribute(int key);

		val_rc getCaseAttribute();

		void setCaseAttribute(int value);

		htab_return getColumnNames(str_ptr tableName);
		

		str_return getDSN();

		

		str_rc getDatabaseName();

		int getFetch();

		obj_rc getSchema();
		
		htab_return getTableColumns(str_ptr tableName);
		obj_rc getTableMode(str_ptr tableName);

		

		val_return handle();

		obj_ptr isql();
		obj_ptr iconfig();
		

		bool inTransaction();
		bool isAutoCommit();
		bool isConnected();


		val_rc lastInsertId();
		

		void log(htab_ptr info);

		obj_rc newBindings();
		obj_rc newDmlBuild();
		obj_rc newParamList();
		str_rc param(int pno);

		val_return prepare(str_ptr query);

		error_return prepareExecute(str_ptr query, htab_ptr values, htab_ptr bindTypes);

		val_return prepareQuery(str_ptr query, htab_ptr values, htab_ptr bindTypes);

		val_return query(str_ptr query, htab_ptr params);

		val_return querySingle(str_ptr query);

		htab_return query_lcase(str_ptr sql, int fmode);

		weak_ref selfRef() 
		{
			return wkself_;
		}

		str_rc quoteName(str_ptr name);

		obj_rc readSchema();



		bool setAttribute(int key, val_ptr value);
		int  setFetch(int mode);

		

		obj_rc getTableModel(str_ptr tableName);

		str_rc lastSQL() const;
		
		IConfig* 	icfg_c();
		ISql*       isql_c();

		str_ptr getName() const;
	protected:


		

	};


}//wcd namespace

#endif