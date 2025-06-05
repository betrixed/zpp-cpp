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
	Model();
	virtual ~Model();

	zobj_mgr getBuilderForMe();

	zobj_mgr getBuilder();

	zobj_mgr newRow(htab_read rdata, bool isSaved = false);

	static zval_mgr createFromResult(zstr_user classname, htab_read results);
	
	static zobj_mgr keyValue(zstr_user static_name, htab_read keynames, htab_read values);

	static zobj_mgr withValues(htab_read keyvalues);

	zobj_mgr byKeyValue(htab_read keynames, htab_read values);

	zobj_mgr byPrimaryValue(htab_read values);

	zobj_mgr getConnect();

};

};

//itable.h
#endif