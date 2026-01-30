#ifndef WCD_ISQL_H
#define WCD_ISQL_H


#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

namespace wcd {

	using namespace zpp;

	class Bindings;
	class IColumns;
	class ParamList;


	extern zend_class_entry* zclass_isql;

	class ISql : public base_d 
	{
	protected:
		str_rc columns(htab_ptr bd);
		void columnsTC(IColumns* tc, htab_rw col_list); //str_buf& col_list);
		htab_ptr getTables(Bindings& bind);

		str_return where(Bindings &bind, htab_ptr wtab);
		str_return insert_col_params(Bindings& bind, htab_ptr rowbind);
		str_rc orderBy(htab_ptr obind);
		
		str_return limit(ParamList* plist, htab_ptr ltab);
		str_return fromJT(Bindings& bind, JoinTables* jt);
		str_return select_jt(Bindings& bind, JoinTables* jt);

	public:
		// should mirror constants defined in IfSql
		enum {
			SQL_OBJ    = 0, 
		    SQL_INSERT = 1,
		    SQL_UPDATE = 2,
		    SQL_DELETE = 3,
		    SQL_SELECT = 4,
		    SQL_WHERE = 5,
		    SQL_JOIN = 6,
		    SQL_RETURN = 7,
		    SQL_FROM = 8,
		    SQL_ORDER = 9,
		    SQL_LIMIT = 10,
		    SQL_AGGREGATE = 11,
		    SQL_DISTINCT = 12,
		    SQL_RENAME = 13,
		    FETCH_AS = 14,
    		MODEL_OBJ = 15,
    		MODEL_CLASS = 16,
    		NAME_LIST = 17
		};
		enum {
			QUOTE_SGL = '\'',
			QUOTE_DBL = '"'
		};

		static base_obj_mgr<ISql>  omg;

		static str_rc tableClass(str_ptr s);

		virtual str_rc quoteName(str_ptr name);

		virtual str_rc seqLastValue(str_ptr seq);

		virtual str_return truncate(Bindings& bind);

		str_return setSeqValue(int value, htab_ptr data);
		
		str_rc entityClass(str_ptr s);

		obj_return deleteSql(Bindings& bind);

		obj_return insert(Bindings& bind);

		obj_return select(Bindings& bind);

		

		obj_return update(Bindings& bind);

		

		str_return emit(val_ptr sp, Bindings* bind, str_ptr lalias, str_ptr ralias);

		str_rc getTruncateSql();

		str_rc valuesDefault();

	};

}//namespace wcd

#endif