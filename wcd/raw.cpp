#ifndef WCD_SQL_RAW_CPP
#define WCD_SQL_RAW_CPP

#ifndef WCD_SQL_RAW_H
#include "raw.h"
#endif

#ifndef WCD_IDRIVER_H
#include "driver.h"
#endif

#ifndef SQL_ARGINFO_H
#define SQL_ARGINFO_H
extern "C" {
	#include "stub/sqlipart_arginfo.h"
};
#endif

namespace wcd {
	using namespace zpp;

base_obj_mgr<Raw> Raw::omg;	

void 
Raw::construct(str_ptr sql, obj_ptr driver)
{
	sql_ = sql;
	db_ = driver;
}

val_rc  
Raw::execute()
{
	val_rc result;

	if (!db_.ok())
	{
		zend_throw_error(zend_ce_error,"DB Connection not set");
		return result;
	}

	IDriver* db = zobj_toc<IDriver>(db_);
	return db->querySingle(sql_);
}


obj_rc  
Raw::getConnection()
{
	return db_;
}


str_rc  
Raw::getSql()
{
	return sql_;
}


void  
Raw::setConnection(obj_ptr driver)
{
	db_ = driver;
}

}; // namespaced wcd


using namespace wcd;

ZEND_METHOD(Wcd_Sql_Raw, __construct)
{
	zend_string* sql;
	zval*        driver;

	ZEND_PARSE_PARAMETERS_START(1,2)
	Z_PARAM_STR(sql)
	Z_PARAM_OBJECT_OF_CLASS_OR_NULL(driver, IDriver::omg.classEntry())
	ZEND_PARSE_PARAMETERS_END();

	Raw* cobj = zval_toc<Raw>(ZEND_THIS);
	cobj->construct(sql, driver);

}

ZEND_METHOD(Wcd_Sql_Raw, setConnection)
{
	zval*        driver;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT_OF_CLASS(driver, IDriver::omg.classEntry())
	ZEND_PARSE_PARAMETERS_END();

	Raw* cobj = zval_toc<Raw>(ZEND_THIS);
	cobj->setConnection(driver);
}

ZEND_METHOD(Wcd_Sql_Raw, getConnection)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Raw* cobj = zval_toc<Raw>(ZEND_THIS);
	obj_rc db = cobj->getConnection();
	db.move_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Raw, getSql)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Raw* cobj = zval_toc<Raw>(ZEND_THIS);
	str_rc db = cobj->getSql();
	db.move_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Raw, execute)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Raw* cobj = zval_toc<Raw>(ZEND_THIS);
	val_rc data = cobj->execute();
	data.move_zv(return_value);
}


PHP_MINIT_FUNCTION(Sql_Raw_reg)
{
	Raw::omg.classEntry(register_class_Wcd_Sql_Raw());

	return SUCCESS;
}

#endif