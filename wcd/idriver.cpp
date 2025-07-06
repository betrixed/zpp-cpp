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

#ifndef WCD_RUNSQL_H
#include "runsql.h"
#endif

#ifndef PHP_PDO_DRIVER_H
#include <ext/pdo/php_pdo_driver.h>
#endif

namespace wcd {

base_obj_mgr<IDriver> IDriver::omg;


class DBSInit : public state_init {
public:

	zstr_intern  query_str;
	zstr_intern  fetch_str;
	zstr_intern  close_cursor;
	zstr_intern  pdo_prefix;
	zstr_intern  pdo_class;
	zstr_intern  begin_trans;
	zstr_intern  bind_value;
	zstr_intern  commit_fn;

	zstr_intern  quote_fn;
	zstr_intern  regex_quoted;
	zstr_intern  rx_cap1;


	DBSInit() : state_init() {}
		

	void init() override;
};


void DBSInit::init() {	
		query_str = "query";
		fetch_str = "fetch";
		close_cursor = "closecursor";
		pdo_prefix = "pdo_";
		pdo_class = "PDO";
		begin_trans = "begintransaction";
		bind_value = "bindvalue";
		commit_fn = "commit";

		quote_fn = "quote";
		regex_quoted = R"x(/^\'(.*)\'$/)x";
		rx_cap1 = "$1";
	}

DBSInit DBS;

void 
IDriver::construct(zobj_user icfgobj, zstr_user name)
{
	icfg_ = icfgobj;
	cfg_name_ = name;
	IConfig* cfg = icfg();
	db_name_ = cfg->getDatabase();
	isql_ = cfg->newSql();
}

zobj_mgr  
IDriver::iconfig()
{
	return icfg_;
}

IConfig*    
IDriver::icfg()
{
	return zobj_toc<IConfig>(icfg_);
}

void 
IDriver::destruct()
{
	if (isConnected())
	{
		close();
	}

	schema_def_.init();
	table_models_.init();
	isql_.init();
	icfg_.init();
}

zstr_mgr 
IDriver::getDSN()
{
	zstr_mgr dname = icfg()->getDriverName();
	dname = dname.to_lower();

	if (dname.starts_with(DBS.pdo_prefix)) {
		dname = dname.substr(4);
	}
	else {
		zend_throw_error(zend_ce_error,"PDO Driver names need to begin with 'pdo_'");
		return dname;
	}
	IConfig* cfg = icfg();
	zstr_mgr host = cfg->getHost();
	zstr_mgr dbname = cfg->getDatabase();

	zstr_buffer buf;

	buf << dname << ':' << "host=" << host
	    << ";dbname=" << dbname;

	return buf.zstr();
}

htab_mgr 
IDriver::getConnectOptions()
{
	htab_mgr result;

	htab_write options(result);

	options.push_back((int)PDO_ATTR_ERRMODE);
	options.push_back((int)PDO_ERRMODE_EXCEPTION);
	return result;
}

void 
IDriver::connect()
{
	if (handle_.ok())
	{
		return;
	}

	zstr_mgr dsn = getDSN();

	htab_mgr options = getConnectOptions();

	IConfig* cfg = icfg();

	zstr_mgr user = cfg->getUsername();
	zstr_mgr pw = cfg->getPassword();

	htab_mgr  args_mgr;
	htab_write args(args_mgr);

	args.push_back(dsn);
	args.push_back(user);
	args.push_back(pw);
	args.push_back(options);

	handle_ = ReflectCache::staticInstanceArgs(DBS.pdo_class, args_mgr);

	afterConnect();
}

void //virtual
IDriver::afterConnect()
{

}

zval_mgr 
IDriver::handle()
{
	if (handle_.ok())
	{
		return handle_;
	}
	connect();

	return handle_;
}

int IDriver::pdo_type(unsigned int ztype)
{
	switch(ztype)
	{
	case IS_LONG:
		return PDO_PARAM_INT;
	case IS_TRUE:
	case IS_FALSE:
		return PDO_PARAM_BOOL;
	case IS_STRING:
	default:
		return PDO_PARAM_STR;
	}
}

void 
IDriver::close()
{
	handle_.set_null();
}

bool 
IDriver::commit()
{
	zobj_mgr pdo = handle();

	if (pdo.ok())
	{
		zval_mgr result = pdo.call(DBS.commit_fn);
		return result.isTrue();
	}
	return false;
}

void 
IDriver::closeStmt(zval_user stmt)
{
	zobj_user obj(stmt);
	obj.call(DBS.close_cursor);
}


void IDriver::bind(zval_user stmt, htab_read params)
{
	zobj_user spdo(stmt);

	if (params.size())
	{
		htab_walk wk;
		auto val = wk.value();

		fn_call_args<3> bvcall;

		bvcall.set_fci(spdo, DBS.bind_value);
		zval* args = bvcall.argsptr();

		int ix = 0;

		for(wk.start(params); wk.ok(); wk.next(), ix++)
		{
			if (val.isArray())
			{
				htab_walk wk2;
				auto bname = wk2.key();
				auto bval = wk2.value();

				for(wk2.start(val.zarray()); wk2.ok(); wk2.next())
				{
	
					ZVAL_COPY_VALUE(args, bname);
					ZVAL_COPY_VALUE(args+1, bval);
					ZVAL_LONG(args+2, pdo_type(bval.ztype()));
					bvcall.call_fn();
				}
			}
			else {
				ZVAL_LONG(args, ix+1);
				ZVAL_COPY_VALUE(args+1, val);
				ZVAL_LONG(args+2, pdo_type(val.ztype()));
				bvcall.call_fn();
			}
		}
	}
}


bool 
IDriver::begin()
{
	zobj_user pdo(handle());
	if (pdo.ok())
	{
		zval_mgr result = pdo.call(DBS.begin_trans);
		return result.isTrue();
	}
	return false;
}

zstr_mgr 
IDriver::escape(zstr_user value)
{
	zobj_user pdo(handle());
	zval_mgr  arg(value);
	zstr_mgr result = pdo.call(DBS.quote_fn, arg);
	result = preg_replace(DBS.regex_quoted, DBS.rx_cap1, result);
	return result;
}


zobj_mgr 
IDriver::newDmlBuild()
{
	IConfig* cfg = icfg();
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
		name = icfg()->getDatabase();
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

//void construct(zobj_user icfg, zstr_user name);
ZEND_METHOD(Wcd_IDriver, __construct)
{
	zval* config;
	zend_string*   name;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_OBJECT_OF_CLASS(config, IConfig::omg.classEntry())
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	db->construct(config, name);

}

ZEND_METHOD(Wcd_IDriver, __destruct)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	db->destruct();
}



ZEND_METHOD(Wcd_IDriver, begin)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	RETURN_BOOL(db->begin());


}

ZEND_METHOD(Wcd_IDriver, bind)
{
	zval* stmt;
	zval* params;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_OBJECT(stmt)
	Z_PARAM_ARRAY(params)
	ZEND_PARSE_PARAMETERS_END();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	db->bind(stmt, params);
}

ZEND_METHOD(Wcd_IDriver, close)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	db->close();
}

ZEND_METHOD(Wcd_IDriver, closeStmt)
{
	zval* stmt;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT(stmt)
	ZEND_PARSE_PARAMETERS_END();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	db->closeStmt(stmt);
}

ZEND_METHOD(Wcd_IDriver, commit)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	bool result = db->commit();
	RETURN_BOOL(result);
}

ZEND_METHOD(Wcd_IDriver, connect)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	db->connect();
}


ZEND_METHOD(Wcd_IDriver, escape)
{
	zend_string* sval;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(sval)
	ZEND_PARSE_PARAMETERS_END();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	zstr_mgr result = db->escape(sval);

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, execute){}
ZEND_METHOD(Wcd_IDriver, fetchAllRows){}
ZEND_METHOD(Wcd_IDriver, fetchRow){}
ZEND_METHOD(Wcd_IDriver, getAttribute){}
ZEND_METHOD(Wcd_IDriver, getColumnNames){}
ZEND_METHOD(Wcd_IDriver, getConnectOptions){}
ZEND_METHOD(Wcd_IDriver, getDSN){}
ZEND_METHOD(Wcd_IDriver, getDatabaseName){}
ZEND_METHOD(Wcd_IDriver, getFetch){}
ZEND_METHOD(Wcd_IDriver, getSchema){}
ZEND_METHOD(Wcd_IDriver, getSchemaClass){}
ZEND_METHOD(Wcd_IDriver, getTableColumns){}
ZEND_METHOD(Wcd_IDriver, getTableModel){}
ZEND_METHOD(Wcd_IDriver, getTableNames){}
ZEND_METHOD(Wcd_IDriver, handle){}
ZEND_METHOD(Wcd_IDriver, iConfig){}
ZEND_METHOD(Wcd_IDriver, iSql){}
ZEND_METHOD(Wcd_IDriver, inTransaction){}
ZEND_METHOD(Wcd_IDriver, isAutoCommit){}
ZEND_METHOD(Wcd_IDriver, isConnected){}
ZEND_METHOD(Wcd_IDriver, lastInsertId){}
ZEND_METHOD(Wcd_IDriver, lastSeqValue){}
ZEND_METHOD(Wcd_IDriver, log){}
ZEND_METHOD(Wcd_IDriver, modelClassName){}
ZEND_METHOD(Wcd_IDriver, newDmlBuild){}
ZEND_METHOD(Wcd_IDriver, param){}
ZEND_METHOD(Wcd_IDriver, prepare){}
ZEND_METHOD(Wcd_IDriver, prepareQuery){}
ZEND_METHOD(Wcd_IDriver, query){}
ZEND_METHOD(Wcd_IDriver, querySingle){}
ZEND_METHOD(Wcd_IDriver, quoteName){}
ZEND_METHOD(Wcd_IDriver, readSchema){}
ZEND_METHOD(Wcd_IDriver, rollback){}
ZEND_METHOD(Wcd_IDriver, setAttribute){}
ZEND_METHOD(Wcd_IDriver, setFetch){}
ZEND_METHOD(Wcd_IDriver, transaction){}


PHP_MINIT_FUNCTION(Wcd_IDriver_reg)
{
	IDriver::omg.classEntry(register_class_Wcd_IDriver());
	

	return SUCCESS;
}
#endif