#ifndef WCD_SQL_POSTGRES_H
#define WCD_SQL_POSTGRES_H

#ifndef WCD_IDRIVER_H
#include "idriver.h"
#endif

#ifndef ZPP_PREG_H
#include "zpp/preg.h"
#endif
 
namespace wcd {

	using namespace zpp;
	using namespace wcc;

	class PGSInit : public state_init {

	public:
		str_intern  rex_pgname;
		

		void init() override;

	};


	class Postgres : public ISql {
	protected:
		preg	pgname;
	public:

		Postgres();
		virtual ~Postgres() = default;

		static base_obj_mgr<Postgres> omg;
		static zend_class_entry* register_class(zend_class_entry* isql_class);

		str_rc quoteName(str_ptr name) override;
		str_rc seqLastValue(str_ptr seqname) override;
		str_return truncate(Bindings& bind)  override;

		
	};

};
#endif