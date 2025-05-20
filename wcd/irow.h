#ifndef WCD_IROW_H
#define WCD_IROW_H



#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

#ifndef WCC_HMAP_H
#include "wcc/hmap.h"
#endif

#ifndef WCD_IMODEL_H
#include "imodel.h"
#endif

namespace wcd {

	using namespace zpp;
	using namespace wcc;

	/** To properly inherit Hmap property access and array access, 
	 *   also have to override own special object handlers
	 *   with its Hmap_php static functions
	 */
	class IRow;

	class IRow_mgr : public base_obj_mgr<IRow>
	{
	protected:
		//typedef base_obj_mgr<T> mydef;

		virtual void init_class_fn() 
		{
		// base class
		mydef::init_class_fn();

		HmapIterator::setup_handlers(mydef::handlers_);
		HmapIterator::setup_class(mydef::class_entry_);
		}
	};

	class IRow : public wcc::Hmap {
    protected:
    	zobj_mgr    table_model_;
    	htab_mgr  original_;
    public:

    	static IRow_mgr omg;

    	IRow();
    	virtual ~IRow();
    	
    	virtual void debug_info(htab_write di);

    	void construct(zobj_user tmodel, zval_user data, bool exists = false);

    	bool create(bool reload = false);

    	void delete_row();

    	bool exists();

    	bool save(bool reload = false);

    	bool update(bool reload = false);

    	void read();

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
