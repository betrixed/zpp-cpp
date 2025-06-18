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

protected:

	int  timestamps_;

	zobj_mgr db_;
	zstr_mgr name_;
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
	virtual ~Model();

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


	zobj_mgr byKeyValue(zval_user keynames, zval_user values);

	zobj_mgr byPrimaryValue(zstr_user values);

	zobj_mgr getConnect();

	htab_mgr getPKey();

	zstr_mgr getName();

	htab_mgr getColDefs();

	htab_mgr getSeqDefs();

	zobj_mgr getTableDef();

	void sequenceMax();




};

};

//itable.h
#endif