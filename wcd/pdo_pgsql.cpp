#ifndef WCD_PDO_PGSQL_CPP
#define WCD_PDO_PGSQL_CPP

namespace wcd {
use namespace zpp;

class PGInit : state_init 
{
public:
	str_intern pgsql_s;
	str_intern tablenames_qry;

	void init() override;
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


str_rc Pdo_pgsql::getSqlType() 
{
	return PGIs.pgsql_s;
}



htab_return Pdo_pgsql::getTableNames()  
{
	//val_return
	htab_ptr    empty;
	htab_return result;

	val_return pstmt = this->prepareQuery(PGIs.tablenames_qry, empty, empty);

	if (pstmt.has_errors())
	{
		result = pstmt;
	}
	else { 
		result = IDriver::fetchAllRows(pstmt.value_, IDriver::PDO_FETCH_NUM);
	}
	return result;
}


val_rc Pdo_pgsql::lastSeqValue(str_ptr name) 
{
	return this->lastInsertId(name);
}

}


#endif