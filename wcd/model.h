#ifndef WCD_MODEL_H
#define WCD_MODEL_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

#ifndef SQL_PART_H
#include "sql_ipart.h"
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

	str_rc db_;
	//str_rc name_;
	htab_rc pkey_options_;
	htab_rc seq_defs_;
	htab_rc class_cdefs_;
	htab_rc class_pkey_;

	obj_rc class_tdef_;

	obj_rc builder_;
	obj_rc builder_me_;



public:

	static base_obj_mgr<Model> omg;
	
	Model();

	void construct();
	
	void destruct();
	
	obj_rc getBuilderForMe();

	obj_rc getBuilder();

	obj_rc newRow(htab_ptr rdata, bool isSaved = false);

	static Model* model_instance(str_ptr classname);
	
	static str_rc classToTableName(str_ptr cname);

	static val_rc createFromResult(str_ptr classname, htab_ptr results);
	
	static obj_rc keyValue(str_ptr static_name, val_ptr keynames, val_ptr values);

	static obj_rc withValues(str_ptr static_name, val_ptr keyvalues);

	static val_rc callStatic(str_ptr static_name, str_ptr method, val_ptr params);

	static obj_rc find(str_ptr static_name, val_ptr id);

	static str_rc getTableName(str_ptr cname);

	static int importFromCSV(str_ptr static_name, str_ptr filename);

	static obj_rc modelBuild(str_ptr static_name);

	static obj_rc row(str_ptr static_name, htab_ptr data);

	static obj_rc rowSaved(str_ptr static_name, htab_ptr data);

	virtual void debug_info(htab_rw di);

	obj_rc byKeyValue(val_ptr keynames, val_ptr values);

	obj_rc byPrimaryValue(str_ptr values);

	obj_rc getConnect();

	htab_rc getPKey();

	str_rc getName();

	htab_rc getColDefs();

	htab_rc getSeqDefs();

	obj_rc getTableDef();

	str_rc createdAtName();

	str_rc updatedAtName();
	
	bool hasTimeStamps() const;

	int getTSFlags() const;

	bool exists(obj_ptr rowobj);

	void sequenceMax();

	bool saveRow(obj_ptr irow, bool reload = false);

	bool_return deleteRow(obj_ptr irow);

	obj_rc readRow(obj_ptr irow);

	htab_rc getKeyOptions();

	htab_rc getFieldDef(str_ptr name);

	htab_rc getForeignKey();

	str_rc now() const;

	void setColDefs(htab_ptr options);

	void setKeyOptions(htab_ptr options);

	void setConnect(obj_ptr db);

	void setPKey(htab_ptr options);

	void setName(str_ptr name);

	void setSeqDefs(htab_ptr options);

	void setTSFlags(int flags);

	htab_rc stampTime(str_ptr str_datetime, int flags=ALL_TS);
	
	

};

};

//itable.h
#endif