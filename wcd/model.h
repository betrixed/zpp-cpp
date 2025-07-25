#ifndef WCD_MODEL_H
#define WCD_MODEL_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

namespace wcd {

using namespace zpp;

class Model : public base_d {
public:
	enum {
		ALL_TS = 3,
		UPDATE_TS = 2,
		CREATE_TS = 1,
		NO_TS = 0
	};

	enum Crud {
		ID_SET = 0,
	    LAST_ID = 1,
	    LAST_SEQ = 2,
	    ID_GEN = 4

	};

protected:

	int  timestamps_;

	zobj_mgr db_;
	//zstr_mgr name_;
	htab_mgr pkey_options_;
	htab_mgr seq_defs_;
	htab_mgr class_cdefs_;
	htab_mgr class_pkey_;

	zobj_mgr class_tdef_;

	zobj_mgr builder_;
	zobj_mgr builder_me_;



public:

	static base_obj_mgr<Model> omg;
	
	Model();

	zobj_mgr getBuilderForMe();

	zobj_mgr getBuilder();

	zobj_mgr newRow(htab_read rdata, bool isSaved = false);

	static Model* model_instance(zstr_user classname);
	
	static zstr_mgr classToTableName(zstr_user cname);

	static zval_mgr createFromResult(zstr_user classname, htab_read results);
	
	static zobj_mgr keyValue(zstr_user static_name, zval_user keynames, zval_user values);

	static zobj_mgr withValues(zstr_user static_name, zval_user keyvalues);

	static zval_mgr callStatic(zstr_user static_name, zstr_user method, zval_user params);

	static zobj_mgr find(zstr_user static_name, zval_user id);

	static zstr_mgr getTableName(zstr_user cname);

	static int importFromCSV(zstr_user static_name, zstr_user filename);

	static zobj_mgr modelBuild(zstr_user static_name);

	static zobj_mgr row(zstr_user static_name, htab_read data);

	static zobj_mgr rowSaved(zstr_user static_name, htab_read data);

	virtual void debug_info(htab_write di);

	zobj_mgr byKeyValue(zval_user keynames, zval_user values);

	zobj_mgr byPrimaryValue(zstr_user values);

	zobj_mgr getConnect();

	htab_mgr getPKey();

	zstr_mgr getName();

	htab_mgr getColDefs();

	htab_mgr getSeqDefs();

	zobj_mgr getTableDef();

	zstr_mgr createdAtName();

	zstr_mgr updatedAtName();
	
	bool hasTimeStamps() const;

	int getTSFlags() const;

	bool exists(zobj_user rowobj);

	void sequenceMax();

	bool saveRow(zobj_user irow, bool reload = false);

	bool deleteRow(zobj_user irow);

	zobj_mgr readRow(zobj_user irow);

	htab_mgr getKeyOptions();

	htab_mgr getFieldDef(zstr_user name);

	htab_mgr getForeignKey();

	zstr_mgr now() const;

	void setColDefs(htab_read options);

	void setKeyOptions(htab_read options);

	void setConnect(zobj_user db);

	void setPKey(htab_read options);

	void setName(zstr_user name);

	void setSeqDefs(htab_read options);

	void setTSFlags(int flags);

	htab_mgr stampTime(zstr_user str_datetime, int flags=ALL_TS);
	
	

};

};

//itable.h
#endif