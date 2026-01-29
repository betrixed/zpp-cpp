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

		str_rc getSqlType() override;
		htab_return getTableNames() override;
		val_rc  lastSeqValue(str_ptr name) override;
	};

};
#endif