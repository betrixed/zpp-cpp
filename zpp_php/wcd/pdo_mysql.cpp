#ifndef PDO_MYSQL_CPP
#define PDO_MYSQL_CPP

#ifndef PDO_MYSQL_H
#include "pdo_mysql.h"
#endif

#ifndef PDO_PGSQL_ARGINFO
#define PDO_PGSQL_ARGINFO
extern "C" {
	#include "stub/pdo_drivers_arginfo.h"
}
#endif

namespace wcd {
using namespace zpp;

base_obj_mgr<Pdo_mysql> Pdo_mysql::omg;

MYS_Init MYS;

void 
MYS_Init::init()
{
	mysql_s = "mysql";
	tablenames_qry = "SHOW TABLES";
	schema_class = "Wcd\\Schema\\Mysql\\Dump";
}

str_rc 
Pdo_mysql::getSqlType()
{
	str_rc result(MYS.mysql_s);
	return result;
}

str_rc 
Pdo_mysql::getSchemaClass()
{
	str_rc result(MYS.schema_class);
	return result;
}


htab_return 
Pdo_mysql::getTableNames()
{
	htab_return result = this->query_lcase(MYS.tablenames_qry, IDriver::FETCH_COLUMN);
	return result;
}

zend_class_entry* Pdo_mysql::register_class(zend_class_entry* pclass)
{
	zend_class_entry* me = register_class_Wcd_Ext_PdoMysql(pclass);
	Pdo_mysql::omg.classEntry(me);
	
	STATE_INIT_ADD(MYS)
	
	return me;
}


}//namespace wcd
#endif