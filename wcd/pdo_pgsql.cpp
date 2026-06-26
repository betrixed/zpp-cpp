#ifndef WCD_PDO_PGSQL_CPP
#define WCD_PDO_PGSQL_CPP

#ifndef WCD_PDO_PGSQL_H
#include "pdo_pgsql.h"
#endif

#ifndef PDO_PGSQL_ARGINFO
#define PDO_PGSQL_ARGINFO
extern "C" {
	#include "stub/pdo_drivers_arginfo.h"
}
#endif

namespace wcd {
using namespace zpp;

base_obj_mgr<Pdo_pgsql> Pdo_pgsql::omg;

PGInit PGIs;

void PGInit::init()
{
	pgsql_s = "pgsql";
	tablenames_qry =  
	    "select tablename"
		" from pg_tables" 
			" where schemaname"
			" not in ('information_schema','pg_catalog')"
			" order by tablename";
}

str_rc 
Pdo_pgsql::getSqlType() 
{
	str_rc result;
	result = PGIs.pgsql_s;
	return result;
}

str_rc 
Pdo_pgsql::getSchemaClass()
{
	str_rc stype = getSqlType();
	//showstr("Pdo_pgsql sqltype is ", stype);

	stype = stype.ucfirst();
	str_buf buf;

	buf << "Wcd\\Schema\\" << stype << "\\Dump";

	return buf.zstr();
}

htab_return 
Pdo_pgsql::getTableNames()  
{
	//val_return
	htab_ptr    empty;
	htab_return result;

	obj_return test = this->prepareQuery(PGIs.tablenames_qry, empty, empty);
	
	if (test.has_errors())
	{
		result = test.move_error();
	}
	else { 
		result = fetchAllRows(test.value_, IDriver::FETCH_COLUMN);
	}
	return result;
}


val_return 
Pdo_pgsql::lastSeqValue(str_ptr name) 
{
	val_return result;

	// PDO can use this as name of sequence
	result =  this->lastInsertId(name);

	if (result.value_.isNull())
	{
		result.error() << " Last insert id was NULL";
	}
	return result;
}

//static 
zend_class_entry* 
Pdo_pgsql::register_class(zend_class_entry* pclass)
{
	zend_class_entry* me = register_class_Wcd_Ext_PdoPgsql(pclass);

	Pdo_pgsql::omg.classEntry(me);
	
	STATE_INIT_ADD(PGIs)
	
	return me;
}

}//namespace wcd


#endif