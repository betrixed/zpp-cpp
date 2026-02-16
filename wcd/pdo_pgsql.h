#ifndef WCD_PDO_PGSQL_H
#define WCD_PDO_PGSQL_H

#ifndef WCD_IDRIVER_H
#include "idriver.h"
#endif

namespace wcd {

	using namespace zpp;
	using namespace wcc;

	class Pdo_pgsql : public IDriver {
	public:

		static base_obj_mgr<Pdo_pgsql> omg;
		static zend_class_entry* register_class(zend_class_entry* pclass);
		
		str_rc getSqlType() override;
		htab_return getTableNames() override;
		val_return  lastSeqValue(str_ptr name) override;
	};

	class PGInit : public state_init 
	{
	public:
		str_intern pgsql_s;
		str_intern tablenames_qry;

		void init() override;
	};

	extern PGInit PGIs;
} // wcd namespace

#endif