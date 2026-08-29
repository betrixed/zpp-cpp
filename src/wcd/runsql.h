#ifndef WCD_RUNSQL_H
#define WCD_RUNSQL_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

namespace wcd {
	using namespace zpp;

	class RunSql : public base_d {
	protected:
		obj_rc    db_;
		str_rc    sql_;
		htab_rc   bind_;
		bool      retval_;

	public:

		static base_obj_mgr<RunSql> omg;

		void construct(obj_ptr db, str_ptr sql, 
			 htab_ptr bind, bool rval = false);

		val_return operation();
		val_return run();
		
		static val_return op(obj_ptr db, str_ptr sql, 
			htab_ptr bind = htab_ptr(), bool rval = false );

	};
};


#endif