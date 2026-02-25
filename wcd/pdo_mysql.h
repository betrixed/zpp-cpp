#ifndef PDO_MYSQL_H
#define PDO_MYSQL_H

#ifndef WCD_PDO_DRIVER_H
#include "pdodriver.h"
#endif


namespace wcd {

	using namespace zpp;
	using namespace wcc;

	class Pdo_mysql : public PdoDriver {
	public:

		static base_obj_mgr<Pdo_mysql> omg;

		static zend_class_entry* register_class(zend_class_entry* pclass);
		
		str_rc getSqlType() override;
		str_rc getSchemaClass() override;
		htab_return getTableNames() override;
	};

	class MYS_Init : public state_init 
	{
	public:
		str_intern mysql_s;
		str_intern tablenames_qry;
		str_intern schema_class;

		void init() override;
	};

	extern MYS_Init MYS;
} // wcd namespace


#endif
