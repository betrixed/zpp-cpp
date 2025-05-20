#ifndef WCD_IMODEL_H
#define WCD_IMODEL_H


namespace wcd {

using namespace zpp;

class IModel : public base_d {
public:
	enum {
		ALL_TS = 3,
		UPDATE_TS = 2,
		CREATE_TS = 1,
		NO_TS = 0
	};

protected:

	int  timestamps_;

	zobj_mgr connect_;
	zstr_mgr name_;
	htab_mgr pkey_options_;
	htab_mgr seq_defs_;
	htab_mgr class_cdefs_;

	htab_mgr pkey_;

public:

	
};

};

//itable.h
#endif