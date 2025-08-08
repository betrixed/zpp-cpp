#ifndef WCD_IROW_H
#define WCD_IROW_H



#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

#ifndef WCC_HMAP_H
#include "wcc/hmap.h"
#endif

#ifndef WCD_IMODEL_H
#include "model.h"
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
    	obj_rc    table_model_;
    	htab_rc  	original_;
    public:

    	static IRow_mgr omg;

    	IRow();
    	virtual ~IRow();
    	
    	virtual void debug_info(htab_rw di);

    	void construct(obj_ptr tmodel, htab_rd data = htab_rd(), bool exists = false);

    	bool create(bool reload = false);

    	void delete_row();

    	bool exists();

    	bool save(bool reload = false);

    	bool update(bool reload = false);

    	void copy(obj_rc recobj);
    	
    	void read();

    	htab_rd getData() const;

    	htab_rc getDataValues(htab_rd attrlist);

    	bool hasValue(str_ptr key);
    	
    	htab_rc getDirty();

    	obj_ptr getModel() {
    		return table_model_;
    	}


    	bool isDirty(str_ptr colname);

    	void mergeData(htab_rd attrlist);

    	void setData(htab_rd data, bool exists = false);

    	void setExists();

    	htab_rc stampTime(str_ptr value, int dtflags = Model::ALL_TS);

	};



};

//irow.h
#endif
