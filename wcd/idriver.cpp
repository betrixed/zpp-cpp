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

#ifndef SQL_ISQL_H
#include "sql_isql.h"
#endif

#ifndef WCD_MODEL_H
#include "model.h"
#endif

#ifndef WCD_ISQL_H
#include "isql.h"
#endif

#ifndef WCD_PDO_PGSQL_H
#include "pdo_pgsql.h"
#endif

#ifndef WCD_PDO_PGSQL_H
#include "pdo_mysql.h"
#endif

#ifndef PGSQLFN_WDC_H
#include "pgsqlfn.h"
#endif

#ifndef DIR_CACHE_H
#include "wcc/dircache.h"
#endif


namespace wcd {

base_obj_mgr<IDriver> IDriver::omg;

DBSInit DBS;

void DBSInit::init() {	
		query_fn = "query";
		fetch_fn = "fetch";
		close_cursor = "closecursor";
		pdo_prefix = "pdo_";

		errorcode_fn = "errorcode";
		pdo_class = "PDO";
		begin_trans = "begintransaction";
		bind_value = "bindvalue";
		commit_fn = "commit";

		quote_fn = "quote";
		regex_quoted = R"x(/^\'(.*)\'$/)x";
		rx_cap1 = "$1";
		execute_fn = "execute";
		fetchall_fn = "fetchall";
		rowcount_fn = "rowcount";

		
		mysql_str = "mysql";
		intransaction_fn = "intransaction";
		lastinsertid_fn = "lastinsertid";
		place_holder = "?";

		prepare_fn = "prepare";
		error_str = "error";
		readschema_fn = "readschema";
		rollback_fn = "rollback";
		
		
		cfg_name = "name";
		db_name = "db_name";
		tbl_models = "table_models";
		iconfig_key = "cfg";
		schema_def = "schemaDef";
		sqlgen_s = "sqlgen";

		handle_s = "handle";
		logging_s = "logging";
		lastsql_s = "lastsql";
	}


void //static
IDriver::throw_not_implemented(const char* msg)
{
	zend_throw_error(zend_ce_error, "IDriver: need override for %s", msg);
}

void //static 
IDriver::notImplementedMsg(str_buf& buf, const char* fn)
{
	buf << "IDriver: virtual " << '\'' << fn << '\'' << " needs override";
}


void 
IDriver::construct(obj_ptr icfgobj, str_ptr name)
{

	obj_ptr self(vobj());

	//zend_printf("This %s\n", typeid(*this).name());
	//showobj("Construct ", self);

	name_ptr_ = self.property_ptr(DBS.cfg_name);
	//showmem(DBS.cfg_name.data(), name_ptr_);
	//zend_printf("name_ptr_%s %lx\n", DBS.cfg_name.data(), (long unsigned int)((zval*) name_ptr_) );
	
	val_rc::try_decref(name_ptr_); 
	name_ptr_.bind_string(name);

	handle_ptr_ = self.property_ptr(DBS.handle_s);

	cfg_ptr_ = self.property_ptr(DBS.iconfig_key);
	//showobj("config obj", icfgobj);
	
	val_rc::try_decref(cfg_ptr_); 
	cfg_ptr_.bind_object(icfgobj);
	//showmem(DBS.iconfig_key.data(), cfg_ptr_);

	logging_ptr_ = self.property_ptr(DBS.logging_s);
	lastsql_ptr_ = self.property_ptr(DBS.lastsql_s);

	ifetch_ = PDO_FETCH_ASSOC;

	wkself_ = weak_ref::refObject(self);

	//showobj("wkself", wkself_);

	IConfig* cfg = icfg_c();
	db_name_ = cfg->getDatabase();
	isql_ = cfg->newSql();
	//showobj("isql_", isql_);

}

str_ptr IDriver::getName() const
{
	return name_ptr_.zstr();
}

void 
IDriver::debug_info(htab_rw di)
{
	base_d::debug_info(di); // properties.

	di.set(DBS.db_name, db_name_);
	di.set(SQSTR.db_ref, wkself_);
	
	di.set(DBS.tbl_models, table_models_);
	di.set(DBS.schema_def, schema_def_);
	di.set(DBS.fetch_fn, ifetch_);
	di.set(DBS.sqlgen_s, isql_);

}

obj_ptr  
IDriver::iconfig()
{
	return cfg_ptr_.zobject();
}

IConfig*    
IDriver::icfg_c()
{
	return zobj_toc<IConfig>(cfg_ptr_.zobject());
}

void 
IDriver::destruct()
{
	if (isConnected())
	{
		close();
	}

	wkself_.init(); 

	schema_def_.init();

	table_models_.init();

	isql_.init();

	val_rc::try_decref(cfg_ptr_);

}

str_return 
IDriver::getDSN()
{
	str_return result;
	notImplementedMsg(result.error(), __FUNCTION__);
	return result;
}

error_return 
IDriver::connect()
{
	error_return result;
	notImplementedMsg(result.error(), __FUNCTION__);
	return result;
}

void //virtual
IDriver::afterConnect()
{

}

obj_return
IDriver::handle()
{
	obj_return result;

	if (!handle_ptr_.ok())
	{
		error_return err = connect();
		if (err.has_errors())
		{
			result = err.move_error();
		}
	}
	result.value_ = handle_ptr_.zobject();
	return result;
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

htab_return 
IDriver::getTableNames()
{
	
	htab_return result;
	notImplementedMsg(result.error(), __FUNCTION__);

	return result;
}


void 
IDriver::close()
{
	val_rc::try_decref(handle_ptr_);
}

str_rc 
IDriver::getSqlType()
{
	str_rc result;
	return result;
}

bool
IDriver::begin()
{
	return false;
}

bool 
IDriver::commit()
{
	return false;
}

bool
IDriver::rollback()
{
	return false;
}

str_rc 
IDriver::quoteName(str_ptr name)
{
	return isql_c()->quoteName(name);
}	

error_return 
IDriver::closeStmt(obj_ptr stmt)
{
	error_return result;
	notImplementedMsg(result.error(), __FUNCTION__);
	return result;
}

str_rc 
IDriver::escape(str_ptr value)
{
	throw_not_implemented("escape");
	return value;
}

void IDriver::bind(obj_ptr stmt, htab_ptr params)
{
	throw_not_implemented("bind");
}

/* static */
error_return 
IDriver::check_results(val_ptr test)
{
	error_return result;

	int rtype = test.ztype();
	switch(rtype) {
	case IS_FALSE:
	case IS_TRUE:
	case IS_LONG:
	case IS_ARRAY:
		break;
	default:
		result.error() << "Bad PHP return type: " << rtype;
		break;
	}

	return result;
}

val_return
IDriver::execute(obj_ptr stmt, bool close, bool fetch)
{
	val_return result;
	notImplementedMsg(result.error(), __FUNCTION__);
	return result;
}

htab_return
IDriver::fetchAllRows(obj_ptr stmt, int mode)
{
	htab_return result;
	notImplementedMsg(result.error(), __FUNCTION__);
	return result;
}

val_return 
IDriver::fetchRow(obj_ptr stmt, int mode)
{
	val_return result;
	notImplementedMsg(result.error(), __FUNCTION__);
	return result;
}


obj_rc 
IDriver::newBindings()
{
	obj_rc result = Bindings::omg.new_zobj();

	Bindings& bind = *zobj_toc<Bindings>(result);

	bind.construct(isql_, wkself_);

	obj_rc plist = this->newParamList();

	bind.setParamList(plist);
	
	return result;
}

obj_rc 
IDriver::newParamList()
{
	obj_rc result(ParamList::omg.new_zobj());
	ParamList* plist = zobj_toc<ParamList>(result);
	plist->construct( wkself_ );
	return result;
}

obj_rc 
IDriver::newDmlBuild()
{
	IConfig* cfg = icfg_c();

	str_rc bclass =  cfg->getDmlBuildClass();

	htab_rc args_mgr;
	htab_rw args(args_mgr);
	args.push_back( wkself_ );
	return ReflectCache::staticInstanceArgs(bclass,args);
}

obj_rc 
IDriver::getSchema()
{
	
	ICache*  cache = nullptr;
	str_rc name;
	

	if (schema_def_.ok())
	{
		return schema_def_;
	}
	obj_ptr server_mgr = Services::getOne(IServer::omg.class_name());
	IServer* isv = zobj_toc<IServer>(server_mgr);

	obj_rc cacheobj = isv->getDataCache();
	if (cacheobj.ok())
	{

		cache = zobj_toc<ICache>(cacheobj);
		str_rc cdir = cache->getOption(SFDi.opt_cachedir);

		name = icfg_c()->getDatabase();

		val_rc nullval;
		schema_def_ = cache->get(name, nullval); 


	}
	if (!schema_def_.ok())
	{
		schema_def_ = readSchema();
		if (cache)
		{
			val_rc data(schema_def_);
			cache->set(name, data);
		}
	}
	return schema_def_;


}

obj_rc 
IDriver::readSchema()
{
	if (!schema_def_.ok())
	{
		str_rc sclass = getSchemaClass();

		obj_rc sdef = ReflectCache::staticInstance(sclass);
		val_rc self(vobj());

		sdef.call(DBS.readschema_fn, self);
		schema_def_ = sdef;
	}
	return schema_def_;
}

ISql* 
IDriver::isql_c()
{
	return zobj_toc<ISql>(isql_);
}

obj_ptr 
IDriver::isql()
{
	return isql_;
}

str_rc 
IDriver::modelClassName(str_ptr tableName)
{
	IConfig* cfg = icfg_c();

	str_rc ns = cfg->get(ICS.k_model_ns);

	str_rc cname = isql_c()->entityClass(tableName);

	str_buf buf;

	if (ns.size())
	{
		buf << ns << "\\";
	}

	buf << cname;

	return buf.zstr();
}

val_return 
IDriver::querySingle(str_ptr query)
{
	val_return result;
	notImplementedMsg(result.error(),__FUNCTION__);
	return result;
}

htab_return 
IDriver::getTableColumns(str_ptr tableName)
{
	obj_rc model_mgr = getTableModel(tableName);

	Model* m = zobj_toc<Model>(model_mgr);
	return m->getColDefs();
}

htab_return 
IDriver::getColumnNames(str_ptr tableName)
{
	htab_return cols = getTableColumns(tableName);
	if (!cols.has_errors())
	{
		cols.value_ = htab_rc::getKeys(cols.value_);
	}
	return cols;
}

obj_rc 
IDriver::getTableModel(str_ptr tableName)
{
	obj_rc result;

	if (table_models_.size())
	{
		val_ptr obj = table_models_.get(tableName);
		result = obj.zobject();
		if (result.ok())
		{
			return result;
		}
	}
	str_rc modelClass = modelClassName(tableName);
	zend_class_entry* ce = class_data::get_class(modelClass);

	if (ce) 
	{
		class_data cdata(ce);
		cdata.new_object(result);
	}
	else {
		//zend_printf("ModelClass not found %s\n", modelClass.data());
		result = Model::omg.new_zobj();
	}
	if (result.ok())
	{
		result.call(STAB.construct_key);
		
		Model* m = zobj_toc<Model>(result);
		
		m->setConnect(wkself_);
		m->setName(tableName);
		
		htab_rw hw(table_models_);
		hw.set(tableName, result);
	}
	return result;
}

str_rc 
IDriver::getDatabaseName()
{
	return db_name_;
}

int 
IDriver::getFetch()
{
	return ifetch_;
}

int 
IDriver::setFetch(int mode)
{
	int result = ifetch_;
	ifetch_ = mode;
	return result;
}

str_rc 
IDriver::getSchemaClass()
{
	IDriver* vp = this;

	str_rc stype = vp->getSqlType();

	stype = stype.ucfirst();
	str_buf buf;

	buf << "Wcd\\Schema\\" << stype << "\\Dump";

	return buf.zstr();
}

bool 
IDriver::inTransaction()
{
	return false;
}

bool 
IDriver::isAutoCommit()
{
	return false;
}

bool 
IDriver::isConnected()
{
	return false;
}

str_rc 
IDriver::lastSQL() const
{
	str_rc result;
	result = lastsql_ptr_.zstr();
	return result;
}

val_return
IDriver::lastInsertId(str_ptr name)
{
	val_return result;
	notImplementedMsg(result.error(), __FUNCTION__);
	return result;
}

val_return 
IDriver::lastSeqValue(str_ptr name)
{
	val_return result;
	notImplementedMsg(result.error(), __FUNCTION__);
	return result;
}

error_return  
IDriver::prepareExecute(str_ptr query, htab_ptr values, htab_ptr bindTypes)
{
	error_return result;
	notImplementedMsg(result.error(), __FUNCTION__);
	return result;
}

str_rc 
IDriver::param(unsigned pno)
{
	return DBS.place_holder;		
}

obj_return
IDriver::prepare(str_ptr query, htab_ptr options)
{
	//zend_printf("IDriver::prepare-- ");
	obj_return h;
	notImplementedMsg(h.error(), __FUNCTION__);
	return h;
}


obj_return
IDriver::prepareQuery(str_ptr query, htab_ptr values, htab_ptr bindTypes)
{
	obj_return result;
	notImplementedMsg(result.error(), __FUNCTION__);
	return result;
}

val_return
IDriver::query(str_ptr query, htab_ptr params)
{
	val_return result;
	notImplementedMsg(result.error(), __FUNCTION__);
	return result;
}


error_return 
IDriver::transaction()
{
	bool test = begin();
	error_return result;

	if (!test)
	{
		result.error() << "Begin transaction failed";
	}
	return result;
}


};//namespace

using namespace wcd;

//void construct(obj_ptr icfg, str_ptr name);
ZEND_METHOD(Wcd_IDriver, __construct)
{
	zval* config;
	zend_string*   name;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_OBJECT_OF_CLASS(config, IConfig::omg.class_entry_)
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
	zarg_rd args(execute_data);

	obj_ptr stmt;
	htab_ptr params;

	stmt = args.obj(args.need(0));
	params = args.htab(args.need(1));

	if (!args.throw_errors())
	{
		IDriver* db = zval_toc<IDriver>(ZEND_THIS);

		db->bind(stmt, params);
	}

	
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

	error_return result = db->closeStmt(stmt);
	result.throw_errors();
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

	error_return val = db->connect();
	val.throw_errors();
}


ZEND_METHOD(Wcd_IDriver, escape)
{
	zend_string* sval;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(sval)
	ZEND_PARSE_PARAMETERS_END();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	str_rc result = db->escape(sval);

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, execute)
{
	val_ptr stmt;

	bool  close = true;
	bool  fetch = false;

	zarg_rd args(execute_data);

	args.ztype(stmt, args.need(0), IS_OBJECT);
	args.zbool(close, args.option(1));
	args.zbool(fetch, args.option(2));

	if (!args.throw_errors())
	{
		IDriver* db = zval_toc<IDriver>(ZEND_THIS);	
		val_return result = db->execute(stmt, close, fetch);
		result.value_.move_zv(return_value);
	}
}

ZEND_METHOD(Wcd_IDriver, fetchAllRows)
{
	zarg_rd args(execute_data);

	obj_ptr stmt = args.obj(args.need(0));
	zend_long fmode = PDO_FETCH_ASSOC;

	args.zlong(fmode, args.option(1));

	htab_return result;

	if (!args.throw_errors())
	{
		IDriver* db = zval_toc<IDriver>(ZEND_THIS);	
		htab_return result = db->fetchAllRows(stmt, fmode);
		if (!result.throw_errors()) {
			result.value_.move_zv(return_value);	
		}
	}	
}

ZEND_METHOD(Wcd_IDriver, fetchRow)
{
	zarg_rd args(execute_data);

	obj_ptr stmt = args.obj(args.need(0));
	zend_long fmode = PDO_FETCH_ASSOC;

	args.zlong(fmode, args.option(1));
	if (!args.throw_errors())
	{
		IDriver* db = zval_toc<IDriver>(ZEND_THIS);	

		val_return result = db->fetchRow(stmt, fmode);
		if (!result.throw_errors())
		{
			result.value_.move_zv(return_value);
		}
	}
}

ZEND_METHOD(Wcd_IDriver, getColumnNames)
{
	zend_string*   table;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(table)
	ZEND_PARSE_PARAMETERS_END();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);	

	htab_return result = db->getColumnNames(table);
	result.throw_errors();
	result.value_.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, getDSN)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	str_return result = db->getDSN();
	result.throw_errors();

	result.value_.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, getDatabaseName)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	str_rc result = db->getDatabaseName();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, getFetch)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	zend_long result = db->getFetch();
	RETURN_LONG(result);
}

//obj_rc getSchema()
ZEND_METHOD(Wcd_IDriver, getSchema)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	obj_rc result = db->getSchema();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, getSchemaClass)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	str_rc result = db->getSchemaClass();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, getSqlType)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	str_rc result = db->getSqlType();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, getTableColumns)
{
	zend_string*   table;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(table)
	ZEND_PARSE_PARAMETERS_END();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);	

	htab_return result = db->getTableColumns(table);
	result.throw_errors();
	result.value_.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, getTableModel)
{
	zend_string*   table;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(table)
	ZEND_PARSE_PARAMETERS_END();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);	

	obj_rc result = db->getTableModel(table);

	result.move_zv(return_value);
}


ZEND_METHOD(Wcd_IDriver, getTableNames)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	htab_return result = db->getTableNames();
	result.throw_errors();
	result.value_.move_zv(return_value);
}


ZEND_METHOD(Wcd_IDriver, handle)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	obj_return result = db->handle();
	if (!result.throw_errors())
	{
		result.value_.move_zv(return_value);
	}

	
}

ZEND_METHOD(Wcd_IDriver, iConfig)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	obj_ptr result = db->iconfig();

	result.copy_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, iSql)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	obj_ptr result = db->isql();

	result.copy_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, inTransaction)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	bool result = db->inTransaction();

	RETURN_BOOL(result);
}

ZEND_METHOD(Wcd_IDriver, isConnected)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	bool result = db->isConnected();

	RETURN_BOOL(result);
}

ZEND_METHOD(Wcd_IDriver, isAutoCommit)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	bool result = db->isAutoCommit();

	RETURN_BOOL(result);
}

ZEND_METHOD(Wcd_IDriver, lastInsertId)
{
	zarg_rd args(execute_data);
	str_ptr name;

	name = args.str_or_null(args.option(0));

	if (!args.throw_errors())
	{
		IDriver* db = zval_toc<IDriver>(ZEND_THIS);

		val_return result = db->lastInsertId(name);

		if (!result.throw_errors())
		{
			result.value_.move_zv(return_value);
		}
	}
	
}

ZEND_METHOD(Wcd_IDriver, lastSeqValue)
{
	str_ptr   seqname;

	zarg_rd args(execute_data);

	args.zstring(seqname, args.need(0));

	if (!args.throw_errors())
	{
		IDriver* db = zval_toc<IDriver>(ZEND_THIS);

		val_return result = db->lastSeqValue(seqname);
		result.value_.move_zv(return_value);
	}
}

ZEND_METHOD(Wcd_IDriver, lastSQL)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	str_rc result = db->lastSQL();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, log)
{

}

ZEND_METHOD(Wcd_IDriver, modelClassName)
{
	zend_string*   table;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(table)
	ZEND_PARSE_PARAMETERS_END();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);	

	str_rc result = db->modelClassName(table);

	result.move_zv(return_value);
}


ZEND_METHOD(Wcd_IDriver, newParamList)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	obj_rc result = db->newParamList();

	result.move_zv(return_value);
}


ZEND_METHOD(Wcd_IDriver, newBindings)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	obj_rc result = db->newBindings();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, newDmlBuild)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	obj_rc result = db->newDmlBuild();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, param)
{
	zend_long   pno;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_LONG(pno)
	ZEND_PARSE_PARAMETERS_END();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);	

	str_rc result = db->param(pno);

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, prepare)
{
	str_ptr   sql;
	htab_ptr  options;

	zarg_rd args(execute_data);

	sql = args.str(args.need(0));
	
	//zval* test = args.option(1);
	//showmem("prepare teset", test);

	options = args.htab(args.option(1));

	if (!args.throw_errors())
	{
		IDriver* db = zval_toc<IDriver>(ZEND_THIS);	
		obj_return result = db->prepare(sql, options);

		result.throw_errors();
		result.value_.move_zv(return_value);	
	}
}

ZEND_METHOD(Wcd_IDriver, prepareQuery)
{
	str_ptr sql;
	htab_ptr        values;
	htab_ptr        types;

	zarg_rd args(execute_data);

	args.zstring(sql, args.need(0));
	args.zarray_null(values, args.option(1));
	args.zarray_null(types, args.option(2));

	if (!args.throw_errors())
	{
		IDriver* db = zval_toc<IDriver>(ZEND_THIS);	
		obj_return result = db->prepareQuery( sql, values, types);
		result.throw_errors();
		result.value_.move_zv(return_value);
	}
}

ZEND_METHOD(Wcd_IDriver, prepareExecute)
{
	str_ptr sql;
	htab_ptr        values;
	htab_ptr        types;

	zarg_rd args(execute_data);

	args.zstring(sql, args.need(0));
	args.zarray_null(values, args.option(1));
	args.zarray_null(types, args.option(2));

	if (!args.throw_errors())
	{
		IDriver* db = zval_toc<IDriver>(ZEND_THIS);	
		error_return result = db->prepareExecute( sql, values, types);
		result.throw_errors();
	}
}

ZEND_METHOD(Wcd_IDriver, query)
{
	zend_string* sql;
	zval*        params = nullptr;
	ZEND_PARSE_PARAMETERS_START(1,2)
	Z_PARAM_STR(sql)
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY(params)
	ZEND_PARSE_PARAMETERS_END();

	htab_rc values(params);

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);	

	val_return result = db->query(sql, values);
	result.throw_errors();
	result.value_.move_zv(return_value);	
}

ZEND_METHOD(Wcd_IDriver, querySingle)
{
	zarg_rd args(execute_data);
	str_ptr sql;

	sql = args.str(args.need(0));

	if (!args.throw_errors())
	{
		IDriver* db = zval_toc<IDriver>(ZEND_THIS);
		val_return result = db->querySingle( sql );
		if (!result.throw_errors())
		{
			result.value_.move_zv(return_value);	
		}
	}
}

ZEND_METHOD(Wcd_IDriver, quoteName)
{
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);	

	str_rc result = db->quoteName( name );
	result.move_zv(return_value);		
}

//obj_rc readSchema();
ZEND_METHOD(Wcd_IDriver, readSchema)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	obj_rc result = db->readSchema();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, rollback)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	bool result = db->rollback();

	RETURN_BOOL(result);
}

//public function setFetch(int $value): int 
ZEND_METHOD(Wcd_IDriver, setFetch)
{
	zend_long mode;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_LONG(mode)
	ZEND_PARSE_PARAMETERS_END();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	zend_long result = db->setFetch(mode);

	RETURN_LONG(result);
}

ZEND_METHOD(Wcd_IDriver, transaction)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	error_return result = db->transaction();

	bool check = !result.has_errors();
	RETURN_BOOL(check);
	result.throw_errors();
}

ZEND_METHOD(Wcd_IDriver, getWeakRef)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	weak_ref result = db->selfRef();

	result.move_zv(return_value);
}

PHP_MINIT_FUNCTION(Wcd_IDriver_reg)
{
	STATE_INIT_ADD(DBS)


	zend_class_entry* dclass = register_class_Wcd_IDriver();

	IDriver::omg.classEntry(dclass);

	class_data cval(IDriver::omg.class_entry_);

	cval.add_constant("FETCH_OBJECT", PDO_FETCH_OBJ);
	cval.add_constant("FETCH_ASSOC", PDO_FETCH_ASSOC);
	cval.add_constant("FETCH_NUM", PDO_FETCH_NUM);
	cval.add_constant("FETCH_COLUMN", PDO_FETCH_COLUMN);


	zend_class_entry* pdo = PdoDriver::register_class(dclass);

	Pdo_pgsql::register_class(pdo);
	Pdo_mysql::register_class(pdo);
	
	Pgsqlfn::register_class(dclass);

	return SUCCESS;
}
#endif