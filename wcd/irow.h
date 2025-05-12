#ifndef WCD_IROW_H
#define WCD_IROW_H



#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

#ifndef WCC_HMAP_H
#include "wcc/hmap.h"
#endif

#ifndef WCD_ITABLE_H
#include "itable.h"
#endif

namespace wcd {

	using namespace zpp;
	using namespace wcc;

	class IRow : public Hmap {
    protected:
    	zobj_mgr    table_model_;
    	htab_empty  original_;
    public:

    	static base_obj_mgr<IRow> omg;

    	void construct(zobj_user tmodel, zval_user data, bool exists = false);

    	bool create(bool reload = false);

    	void delete_row();

    	bool exists();

    	bool save(bool reload = false);

    	bool update(bool reload = false);

    	void reload();

    	htab_mgr getDataValues(zval_user attrlist);

    	bool hasValue(zstr_user key);
    	
    	htab_mgr getDirty();

    	zobj_user getModel() {
    		return table_model_;
    	}


    	bool isDirty(zstr_user colname);

    	void mergeData(zval_user attrlist);

    	void setData(zval_user data, bool exists = false);

    	void setExists();

    	htab_mgr stampTime(zstr_user value, int dtflags = ITable::ALL_TS);

	};



};

//irow.h
#endif
