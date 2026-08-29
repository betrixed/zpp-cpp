#ifndef WCD_PDO_PGSQL_H
#define WCD_PDO_PGSQL_H

#ifndef WCD_PDO_DRIVER_H
#include "pdodriver.h"
#endif

namespace wcd {

	using namespace zpp;

	class Pdo_pgsql : public PdoDriver {
	public:

		static base_obj_mgr<Pdo_pgsql> omg;
		static zend_class_entry* register_class(zend_class_entry* pclass);
		
		str_rc getSqlType() override;
		str_rc getSchemaClass() override;
		htab_return getTableNames() override;
		htab_return getSequenceNames() override;
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