#ifndef WCD_PDO_PGSQL_CPP
#define WCD_PDO_PGSQL_CPP

#ifndef PDO_PGSQL_ARGINFO
#define PDO_PGSQL_ARGINFO
extern "C" {
	#include "stub/pdo_pgsql_arginfo.h"
}
#endif

namespace wcd {
using namespace zpp;

base_obj_mgr<Pdo_pgsql> Pdo_pgsql::omg;

class PGInit : state_init 
{
public:
	str_intern pgsql_s;
	str_intern tablenames_qry;

	void init() override
	{
		pgsql_s = "pgsql";
		tablenames_qry =  
		    "select tablename "
			" from pg_tables" 
  			" where schemaname not in ('information_schema','pg_catalog')"
  			" order by tablename";
	}
};

PGInit PGIs;

str_rc 
Pdo_pgsql::getSqlType() 
{
	return PGIs.pgsql_s;
}

htab_return 
Pdo_pgsql::getTableNames()  
{
	//val_return
	htab_ptr    empty;
	htab_return result;

	val_return pstmt = this->prepareQuery(PGIs.tablenames_qry, empty, empty);

	if (pstmt.has_errors())
	{
		result = std::move(pstmt);
	}
	else { 
		result = IDriver::fetchAllRows(pstmt.value_, IDriver::FETCH_NUM);
	}
	return result;
}


val_return 
Pdo_pgsql::lastSeqValue(str_ptr name) 
{
	val_return result;

	// weak, because no use of name
	result.value_ =  this->lastInsertId();

	if (result.value_.isNull())
	{
		result.error() << "last insert id was NULL";
	}
	return result;
}

//static 
zend_class_entry* 
Pdo_pgsql::register_class(zend_class_entry* pclass)
{
	zend_class_entry* me = register_class_Wcd_PdoPgsql(pclass);
	Pdo_pgsql::omg.classEntry(me);
	return me;
}

}//namespace wcd


#endif