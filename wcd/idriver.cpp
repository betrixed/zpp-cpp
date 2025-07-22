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
	zstr_intern  execute_fn;
	zstr_intern  fetchall_fn;
	zstr_intern  rowcount_fn;


	zstr_intern  getattribute_fn;
	zstr_intern  mysql_str;
	zstr_intern  intransaction_fn;
	zstr_intern  lastinsertid_fn;
	zstr_intern  place_holder;

	zstr_intern  prepare_fn;
	zstr_intern  error_str;
	zstr_intern  readschema_fn;
	zstr_intern  rollback_fn;
	zstr_intern  setattribute_fn;

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
		execute_fn = "execute";
		fetchall_fn = "fetchall";
		rowcount_fn = "rowcount";

		getattribute_fn = "getattribute";
		mysql_str = "mysql";
		intransaction_fn = "intransaction";
		lastinsertid_fn = "lastinsertid";
		place_holder = "?";

		prepare_fn = "prepare";
		error_str = "error";
		readschema_fn = "readschema";
		rollback_fn = "rollback";
		setattribute_fn = "setattribute";
	}

DBSInit DBS;


void 
IDriver::construct(zobj_user icfgobj, zstr_user name)
{
	//showobj("cfg obj", icfgobj);
	//showstr("cfg name", name);
	ifetch_ = PDO_FETCH_ASSOC;

	icfg_ = icfgobj;
	cfg_name_ = name;
	IConfig* cfg = icfg_c();
	db_name_ = cfg->getDatabase();
	isql_ = cfg->newSql();
}

zobj_mgr  
IDriver::iconfig()
{
	return icfg_;
}

IConfig*    
IDriver::icfg_c()
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
	zstr_mgr dname = icfg_c()->getDriverName();
	dname = dname.to_lower();

	if (dname.starts_with(DBS.pdo_prefix)) {
		dname = dname.substr(4);
	}
	else {
		zend_throw_error(zend_ce_error,"PDO Driver names need to begin with 'pdo_'");
		return dname;
	}
	IConfig* cfg = icfg_c();
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

	IConfig* cfg = icfg_c();

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

zstr_mgr 
IDriver::getSqlType()
{
	return DBS.mysql_str;
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

zstr_mgr 
IDriver::quoteName(zstr_user name)
{
	return isql_c()->quoteName(name);
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

zval_mgr 
IDriver::getCaseAttribute()
{
	return getAttribute(PDO_ATTR_CASE);
}

void 
IDriver::setCaseAttribute(int value)
{
	zval_mgr arg(value);

	setAttribute(PDO_ATTR_CASE, arg);
}

bool 
IDriver::begin()
{
	zobj_user pdo(handle());
	bool result = false;
	if (pdo.ok())
	{
		zval_mgr test = pdo.call(DBS.begin_trans);
		result = test.isTrue();
	}
	return result;
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

bool 
check_results(zval_user test)
{
	int rtype = test.ztype();
	switch(rtype) {
	case IS_FALSE:
	case IS_TRUE:
	case IS_LONG:
	case IS_ARRAY:
		return true;
	}
	zend_throw_error(zend_ce_error,"Error on fetch results");
	return false;
}
zval_mgr 
IDriver::execute(zval_user stmt, bool close, bool fetch)
{
	//zend_printf("execute: bool(%d)\n", fetch);
	zobj_user sobj(stmt);

	zval_mgr pdo_result = sobj.call(DBS.execute_fn);
	zval_mgr result;

	if (pdo_result.isTrue())
	{
		if (fetch) {
			zval_mgr farg(ifetch_);
			result = sobj.call(DBS.fetchall_fn, farg);
		}
		else {
			result = sobj.call(DBS.rowcount_fn);
			//showmem("RowCount", result);
		}
	}
	if (close || pdo_result.isFalse())
	{
		sobj.call(DBS.close_cursor);
		closeStmt(stmt);
		if (isAutoCommit() && inTransaction())
		{
			commit();
		}
	}

	if (pdo_result.isTrue()) {
		if (fetch)
		{
			check_results(result);
		}
	}
	else {
		result.set_bool(false);
	}
	return result;
}

htab_mgr 
IDriver::fetchAllRows(zval_user stmt, int mode)
{
	zobj_user sobj(stmt);
	htab_mgr result;

	if (sobj.ok())
	{
		zval_mgr farg(mode);
		result = sobj.call(DBS.fetchall_fn, farg);
	}
	if (!result.size())
	{
		result = htab_mgr::empty_array();
	}
	return result;
}

zval_mgr 
IDriver::fetchRow(zval_user stmt, int mode)
{
	zobj_user sobj(stmt);
	zval_mgr farg(mode);
	return sobj.call(DBS.fetch_str, farg);
}

zobj_mgr 
IDriver::newDmlBuild()
{
	IConfig* cfg = icfg_c();

	zstr_mgr bclass =  cfg->getDmlBuildClass();

	htab_mgr args_mgr;
	htab_write args(args_mgr);
	args.push_back(zobj_user(vobj()));
	return ReflectCache::staticInstanceArgs(bclass,args);
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
		name = icfg_c()->getDatabase();
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

zobj_mgr 
IDriver::readSchema()
{
	if (!schema_def_.ok())
	{
		zobj_mgr sdef = ReflectCache::staticInstance(getSchemaClass());
		zval_mgr self(vobj());

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

zobj_mgr 
IDriver::isql()
{
	return isql_;
}

zstr_mgr 
IDriver::modelClassName(zstr_user tableName)
{
	IConfig* cfg = icfg_c();

	zstr_mgr ns = cfg->get(ICS.k_model_ns);

	zstr_mgr cname = isql_c()->entityClass(tableName);

	zstr_buffer buf;

	if (ns.size())
	{
		buf << ns << "\\";
	}

	buf << cname;

	return buf.zstr();
}

zval_mgr 
IDriver::getAttribute(int key)
{
	zobj_mgr pdo(handle());

	zval_mgr arg(key);

	return pdo.call(DBS.getattribute_fn, arg);

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

htab_mgr 
IDriver::getTableColumns(zstr_user tableName)
{
	zobj_mgr model_mgr = getTableModel(tableName);
	Model* m = zobj_toc<Model>(model_mgr);
	return m->getColDefs();
}

htab_mgr 
IDriver::getColumnNames(zstr_user tableName)
{
	htab_mgr cols = getTableColumns(tableName);
	return htab_mgr::getKeys(cols);
}

zobj_mgr 
IDriver::getTableModel(zstr_user tableName)
{
	zobj_mgr result;

	if (table_models_.size())
	{
		zval_user obj = table_models_.get(tableName);
		result = obj.zobject();
		if (result.ok())
		{
			return result;
		}
	}
	zstr_mgr modelClass = modelClassName(tableName);
	class_data cdata(modelClass);
	if (cdata.ok()) 
	{
		cdata.new_object(result);
	}
	else {
		result = Model::omg.new_zobj();
		Model* m = zobj_toc<Model>(result);
		m->setName(tableName);	
	}
	if (result.ok())
	{
		htab_write hw(table_models_);
		
		hw.set(tableName, result);
	}
	return result;
}

zstr_mgr 
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

zstr_mgr 
IDriver::getSchemaClass()
{
	zstr_mgr stype = getSqlType();
	stype = stype.ucfirst();
	zstr_buffer buf;

	buf << "Wcd\\Schema\\" << stype << "\\Dump";

	return buf.zstr();
}

bool 
IDriver::inTransaction()
{
	zobj_mgr pdo(handle());

	zval_mgr result = pdo.call(DBS.intransaction_fn);
	return result.isTrue();
}

bool 
IDriver::isAutoCommit()
{
	return false;
}

bool 
IDriver::isConnected()
{
	return handle_.isObject();
}

zstr_mgr 
IDriver::lastSQL() const
{
	return lastsql_;
}

zval_mgr 
IDriver::lastInsertId()
{
	zobj_mgr pdo(handle());

	return pdo.call(DBS.lastinsertid_fn);
}

zval_mgr 
IDriver::lastSeqValue(zstr_user name)
{
	zstr_buffer buf;

	buf << "select lastval(" << name << ")";
	zstr_mgr  sql = buf.zstr();
	htab_read empty;

	zval_mgr srow = query(sql, empty);
	zval_mgr result;
	if (srow.isArray())
	{
		htab_read rows(srow);
		result = rows.get(int(0));
	}
	return result;
}

zstr_mgr 
IDriver::param(int pno)
{
	return DBS.place_holder;		
}

zval_mgr 
IDriver::prepare(zstr_user query)
{
	zobj_mgr pdo(handle());

	lastsql_ = query;
	zval_mgr sql(query);
	zval_mgr stmt = pdo.call(DBS.prepare_fn, sql);
	/** if (!stmt.ok())
	{
		zend_throw_error(zend_ce_error, "Prepare: %s", lastsql_.data());
	}
	*/
	return stmt;
}


zval_mgr
IDriver::prepareQuery(zstr_user query, htab_read values, htab_read bindTypes)
{
	zval_mgr stmt_mgr = prepare(query);

	zobj_mgr stmt(stmt_mgr);

	if (!stmt.ok())
	{
		return stmt_mgr;
	}
	zval_mgr test;
	zval_mgr temp;


	if (values.size())
	{
		if (values.has_index(0))
		{
			temp = values;
			test = stmt.call(DBS.execute_fn, temp);
		}
		else {
			htab_walk wk;
			auto key = wk.key();
			auto val = wk.value();
			for(wk.start(values); wk.ok(); wk.next())
			{
				if (bindTypes.size())
				{
					temp = bindTypes.get(key);
				}
				else {
					temp = (zend_long)pdo_type(val.ztype());
				}
				stmt.call(DBS.bind_value, val, temp);
			}
			test = stmt.call(DBS.execute_fn);
		}
	}
	else {
		test = stmt.call(DBS.execute_fn);
	}
	if (!test.ok())
	{
		zend_throw_error(zend_ce_error,"Statement execute failed %s", lastsql_.data());
	}
	return stmt_mgr;
}

zval_mgr 
IDriver::query(zstr_user query, htab_read params)
{
	htab_read btypes;

	zobj_mgr stmt = prepareQuery(query, params, btypes);

	zval_mgr result = stmt.call(DBS.fetchall_fn);

	stmt.call(DBS.close_cursor);

	if (!check_results(result)) 
	{
		result.set_bool(false);
	}

	return result;
}

bool 
IDriver::rollback()
{
	zobj_mgr pdo(handle());

	zval_mgr result = pdo.call(DBS.rollback_fn);

	return result.isTrue();
}

bool 
IDriver::transaction()
{
	bool result = begin();

	if (!result)
	{
		zend_throw_error(zend_ce_error, "Begin transaction failed");
	}

	return result;
}

bool 
IDriver::setAttribute(int key, zval_user value)
{
	zobj_mgr pdo(handle());

	zval_mgr arg1(key);
	zval_mgr result = pdo.call(DBS.setattribute_fn, arg1, value);

	return result.isTrue();
}

};//namespace

using namespace wcd;

//void construct(zobj_user icfg, zstr_user name);
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

ZEND_METHOD(Wcd_IDriver, execute)
{
	zval* stmt;
	bool  close = true;
	bool  fetch = false;

	ZEND_PARSE_PARAMETERS_START(1,3)
	Z_PARAM_OBJECT(stmt)
	Z_PARAM_OPTIONAL
	Z_PARAM_BOOL(close);
	Z_PARAM_BOOL(fetch);

	ZEND_PARSE_PARAMETERS_END();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);	

	zval_mgr result = db->execute(stmt, close, fetch);

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, fetchAllRows)
{
	zval* stmt;
	zend_long   mode = PDO_FETCH_ASSOC;

	ZEND_PARSE_PARAMETERS_START(1,2)
	Z_PARAM_OBJECT(stmt)
	Z_PARAM_OPTIONAL
	Z_PARAM_LONG(mode);
	ZEND_PARSE_PARAMETERS_END();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);	

	htab_mgr result = db->fetchAllRows(stmt, mode);

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, fetchRow)
{
	zval* stmt;
	zend_long   mode = PDO_FETCH_ASSOC;

	ZEND_PARSE_PARAMETERS_START(1,2)
	Z_PARAM_OBJECT(stmt)
	Z_PARAM_OPTIONAL
	Z_PARAM_LONG(mode);
	ZEND_PARSE_PARAMETERS_END();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);	

	htab_mgr result = db->fetchRow(stmt, mode);

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, getAttribute)
{
	zend_long   key;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_LONG(key)
	ZEND_PARSE_PARAMETERS_END();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);	

	zval_mgr result = db->getAttribute(key);

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, getCaseAttribute)
{
	ZEND_PARSE_PARAMETERS_NONE();
	IDriver* db = zval_toc<IDriver>(ZEND_THIS);	

	zval_mgr result = db->getCaseAttribute();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, setCaseAttribute)
{
	zend_long value;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_LONG(value)
	ZEND_PARSE_PARAMETERS_END();


	IDriver* db = zval_toc<IDriver>(ZEND_THIS);	

	db->setCaseAttribute(value);

}

ZEND_METHOD(Wcd_IDriver, getColumnNames)
{
	zend_string*   table;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(table)
	ZEND_PARSE_PARAMETERS_END();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);	

	htab_mgr result = db->getColumnNames(table);

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, getConnectOptions)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	htab_mgr result = db->getConnectOptions();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, getDSN)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	zstr_mgr result = db->getDSN();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, getDatabaseName)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	zstr_mgr result = db->getDatabaseName();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, getFetch)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	zend_long result = db->getFetch();
	RETURN_LONG(result);
}

//zobj_mgr getSchema()
ZEND_METHOD(Wcd_IDriver, getSchema)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	zobj_mgr result = db->getSchema();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, getSchemaClass)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	zstr_mgr result = db->getSchemaClass();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, getSqlType)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	zstr_mgr result = db->getSqlType();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, getTableColumns)
{
	zend_string*   table;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(table)
	ZEND_PARSE_PARAMETERS_END();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);	

	htab_mgr result = db->getTableColumns(table);

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, getTableModel)
{
	zend_string*   table;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(table)
	ZEND_PARSE_PARAMETERS_END();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);	

	zobj_mgr result = db->getTableModel(table);

	result.move_zv(return_value);
}


ZEND_METHOD(Wcd_IDriver, getTableNames)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	htab_mgr result = db->getTableNames();
	result.move_zv(return_value);
}


ZEND_METHOD(Wcd_IDriver, handle)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	zval_mgr result = db->handle();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, iConfig)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	zobj_mgr result = db->iconfig();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, iSql)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	zobj_mgr result = db->isql();

	result.move_zv(return_value);
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
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	zval_mgr result = db->lastInsertId();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, lastSeqValue)
{
	zend_string*   seqname;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(seqname)
	ZEND_PARSE_PARAMETERS_END();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	zval_mgr result = db->lastSeqValue(seqname);

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, lastSQL)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	zstr_mgr result = db->lastSQL();

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

	zstr_mgr result = db->modelClassName(table);

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, newDmlBuild)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	zobj_mgr result = db->newDmlBuild();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, param)
{
	zend_long   pno;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_LONG(pno)
	ZEND_PARSE_PARAMETERS_END();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);	

	zstr_mgr result = db->param(pno);

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, prepare)
{
	zend_string*   sql;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(sql)
	ZEND_PARSE_PARAMETERS_END();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);	

	zval_mgr result = db->prepare(sql);

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, prepareQuery)
{
	zend_string* sql;
	zval*        pvalues = nullptr;
	zval*        ptypes = nullptr;
	ZEND_PARSE_PARAMETERS_START(1,3)
	Z_PARAM_STR(sql)
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY(pvalues)
	Z_PARAM_ARRAY(ptypes)
	ZEND_PARSE_PARAMETERS_END();

	htab_mgr values(pvalues);
	htab_mgr vtypes(ptypes);


	IDriver* db = zval_toc<IDriver>(ZEND_THIS);	

	zval_mgr result = db->prepareQuery( sql, values, vtypes);
	result.move_zv(return_value);
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

	htab_mgr values(params);

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);	

	zval_mgr result = db->query(sql, values);
	result.move_zv(return_value);	
}

ZEND_METHOD(Wcd_IDriver, querySingle)
{
	zend_string* sql;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(sql)
	ZEND_PARSE_PARAMETERS_END();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);	

	zval_mgr result = db->querySingle( sql );
	result.move_zv(return_value);	
}

ZEND_METHOD(Wcd_IDriver, quoteName)
{
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);	

	zstr_mgr result = db->quoteName( name );
	result.move_zv(return_value);		
}

//zobj_mgr readSchema();
ZEND_METHOD(Wcd_IDriver, readSchema)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	zobj_mgr result = db->readSchema();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, rollback)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	bool result = db->rollback();

	RETURN_BOOL(result);
}

//public function setAttribute(int $attkey, mixed $value)
ZEND_METHOD(Wcd_IDriver, setAttribute)
{
	zend_long attkey;
	zval*     value;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_LONG(attkey)
	Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	bool result = db->setAttribute(attkey, value);

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

	bool result = db->transaction();

	RETURN_BOOL(result);
}

PHP_MINIT_FUNCTION(Wcd_IDriver_reg)
{
	IDriver::omg.classEntry(register_class_Wcd_IDriver());
	
	class_data cval(IDriver::omg.class_entry_);

	cval.add_constant("FETCH_OBJECT", PDO_FETCH_OBJ);
	cval.add_constant("FETCH_ASSOC", PDO_FETCH_ASSOC);
	cval.add_constant("FETCH_NUM", PDO_FETCH_NUM);


	return SUCCESS;
}
#endif