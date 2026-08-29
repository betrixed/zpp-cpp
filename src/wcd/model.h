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

	str_rc 	 dbname_;
	weak_ref dbref_;
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
	
	obj_return getBuilderForMe();

	obj_return getBuilder();

	obj_rc newRow(htab_ptr rdata, bool isSaved = false);

	static obj_return model_instance(str_ptr classname);
	
	static str_rc classToTableName(str_ptr cname);

	static val_rc createFromResult(str_ptr classname, htab_ptr results);
	
	static obj_return keyValue(str_ptr static_name, val_ptr keynames, val_ptr values);

	static obj_return withValues(str_ptr static_name, val_ptr keyvalues);

	static val_return callStatic(str_ptr static_name, str_ptr method, val_ptr params);

	static obj_return find(str_ptr static_name, val_ptr id);

	static str_rc getTableName(str_ptr cname);

	static int_return importFromCSV(str_ptr static_name, str_ptr filename);

	static obj_return modelBuild(str_ptr static_name);

	static obj_rc row(str_ptr static_name, htab_ptr data);

	static obj_rc rowSaved(str_ptr static_name, htab_ptr data);

	virtual void debug_info(htab_rw di);

	obj_return byKeyValue(val_ptr keynames, val_ptr values);

	obj_return byPrimaryValue(str_ptr values);

	obj_return getConnect();

	htab_return getPKey();

	str_rc getName();

	htab_return getColDefs();

	htab_return getSeqDefs();

	obj_return getTableDef();

	str_rc createdAtName();

	str_rc updatedAtName();
	
	bool hasTimeStamps() const;

	int getTSFlags() const;

	bool_return exists(obj_ptr rowobj);

	error_return sequenceMax();

	bool_return saveRow(obj_ptr irow, bool reload = false);

	bool_return deleteRow(obj_ptr irow);

	obj_return readRow(obj_ptr irow);

	htab_return getKeyOptions();

	htab_return getFieldDef(str_ptr name);

	htab_return getForeignKey();

	str_rc now() const;

	void setColDefs(htab_ptr options);

	void setKeyOptions(htab_ptr options);

	void setConnect(const weak_ref& db);

	void setPKey(htab_ptr options);

	void setName(str_ptr name);

	void setSeqDefs(htab_ptr options);

	void setTSFlags(int flags);

	htab_rc stampTime(str_ptr str_datetime, int flags=ALL_TS);
	
};

	class Model_init : public state_init {
	public:

		str_intern find_first;
		str_intern find_all;
		str_intern by_str;

		str_intern eq_str;
		str_intern u_model;
		str_intern r_arg;
		str_intern escape_key;
		str_intern escape_str;
		str_intern get_tables;
		str_intern columns_str;
		str_intern m_updated_at;
		str_intern m_created_at;
		str_intern m_datetime_type;
		str_intern type_str;
		str_intern name_str;
		str_intern returns_key;
		str_intern get_primary_key;
		str_intern fn_getseqcols;
		str_intern k_created_at;
		str_intern k_updated_at;

		str_intern k_pkey_options;
		str_intern k_seq_defs;
		str_intern k_col_defs;
		str_intern k_pkey;
		str_intern k_tdef;
		str_intern k_buildme;
		str_intern k_driver;

		str_intern k_table;
		str_intern k_field;
		
		void init() override;

	};

	extern Model_init MIS;

};

//itable.h
#endif