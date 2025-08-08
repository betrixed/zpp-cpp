#ifndef WCD_RUNSQL_H
#define WCD_RUNSQL_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

namespace wcd {
	using namespace zpp;

	class RunSql : public base_d {
	protected:
		obj_rc  db_;
		str_rc  sql_;
		htab_rc  bind_;
		bool      retval_;

	public:

		static base_obj_mgr<RunSql> omg;

		void construct(obj_ptr db, str_ptr sql, 
			 htab_rd bind, bool rval = false);

		val_rc operation();
		val_rc run();
		
		static val_rc op(obj_ptr db, str_ptr sql, 
			htab_rd bind = htab_rd(), bool rval = false );

	};
};


#endif