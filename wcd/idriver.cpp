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

namespace wcd {

base_obj_mgr<IDriver> IDriver::omg;

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

};//namespace

using namespace wcd;

PHP_MINIT_FUNCTION(Wcd_IDriver_reg)
{
	IDriver::omg.classEntry(register_class_Wcd_IDriver());

	return SUCCESS;
}
#endif