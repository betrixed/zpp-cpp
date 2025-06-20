#ifndef WCD_IDRIVER_CPP
#define WCD_IDRIVER_CPP

#ifndef WCD_IDRIVER_H
#include "idriver.h"
#endif

#ifndef DB_ARGINFO_H
#define DB_ARGINFO_H
extern "C" {
     #include "stub/db_arginfo.h"
}
#endif

#ifndef ICACHE_H
#include "wcc/icache.h"
#endif

#ifndef WCC_SERVICES_H
#include "wcc/services.h"
#endif

namespace wcd {

base_obj_mgr<IDriver> IDriver::omg;


class DBSInit : public state_init {
public:

	zstr_intern  query_str;
	zstr_intern  fetch_str;
	zstr_intern  close_cursor;


	DBSInit() : state_init() {}
		

	void init() override {	
		query_str = "query";
		fetch_str = "fetch";
		close_cursor = "closecursor";
	}
};

DBSInit DBS;

void 
IDriver::construct(zobj_user icfg, zstr_user name)
{
	icfg_ = icfg;
	cfg_name_ = name;
	IConfig* cfg = iconfig();
	db_name_ = cfg->getDatabase();
	isql_ = cfg->newSql();
}

IConfig*    
IDriver::iconfig()
{
	return zobj_toc<IConfig>(icfg_);
}

void 
IDriver::destruct()
{
	
}

zobj_mgr 
IDriver::newDmlBuild()
{
	IConfig* cfg = iconfig();
	return cfg->newDmlBuild(this->vobj());
}

zobj_mgr 
IDriver::getSchema()
{
	
	ICache*  cache = nullptr;
	zstr_mgr name;

	if (schema_def_.ok())
	{
		return schema_def_;
	}
	zobj_user server_mgr = Services::getOne(IServer::omg.class_name());
	IServer* isv = zobj_toc<IServer>(server_mgr);
	zobj_mgr cache_mgr = isv->getDataCache();
	if (cache_mgr.ok())
	{
		cache = zobj_toc<ICache>(cache_mgr);
		name = iconfig()->getDatabase();
		schema_def_ = cache->get(name); 
	}
	if (!schema_def_.ok())
	{
		schema_def_ = readSchema();
		if (cache)
		{
			zval_mgr data(schema_def_);
			cache->set(name, data);
		}
	}
	return schema_def_;


}

zval_mgr 
IDriver::querySingle(zstr_user query)
{
	zobj_mgr pdo(handle_);

	zval_mgr arg1(query);

	zobj_mgr stmt = pdo.call(DBS.query_str, arg1);

	arg1 = (zend_long) ifetch_;
	zval_mgr result = stmt.call(DBS.fetch_str, arg1);

	stmt.call(DBS.close_cursor);

	return result;
}

};//namespace

using namespace wcd;




PHP_MINIT_FUNCTION(Wcd_IDriver_reg)
{
	IDriver::omg.classEntry(register_class_Wcd_IDriver());

	return SUCCESS;
}
#endif