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

#ifndef DIR_CACHE_H
#include "wcc/dircache.h"
#endif

namespace wcd {

base_obj_mgr<IDriver> IDriver::omg;

DBSInit DBS;

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

		cfg_name = "name";
		db_name = "db_name";
		tbl_models = "table_models";
		iconfig_key = "cfg";
		schema_def = "schemaDef";
		sqlgen_s = "sqlgen";


	}




void 
IDriver::construct(obj_ptr icfgobj, str_ptr name)
{

	ifetch_ = PDO_FETCH_ASSOC;

	icfg_ = icfgobj;
	name_ = name;

	obj_ptr self(vobj());

	wkself_ = weak_ref::refObject(self);

	val_rc parg(name);
	self.property(DBS.cfg_name, parg);

	IConfig* cfg = icfg_c();
	db_name_ = cfg->getDatabase();
	isql_ = cfg->newSql();


}

void 
IDriver::debug_info(htab_rw di)
{


	di.set(DBS.cfg_name, name_);
	di.set(DBS.db_name, db_name_);
	di.set(SQSTR.db_ref, wkself_);
	
	di.set(DBS.tbl_models, table_models_);
	di.set(DBS.iconfig_key, icfg_);
	di.set(DBS.schema_def, schema_def_);
	di.set(DBS.fetch_str, ifetch_);
	di.set(DBS.sqlgen_s, isql_);

}

obj_ptr  
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

	wkself_.init(); 

	schema_def_.init();

	table_models_.init();

	isql_.init();

	icfg_.init();
}

str_return 
IDriver::getDSN()
{
	str_return result;
	str_rc dname = icfg_c()->getDriverName();
	
	dname.lowercase();

	if (dname.starts_with(DBS.pdo_prefix)) {
		dname = dname.substr(4);
	}
	else {
		result.error() << "PDO Driver names need to begin with 'pdo_' : " << dname;
		return result;
	}

	IConfig* cfg = icfg_c();
	str_rc host = cfg->getHost();
	str_rc dbname = cfg->getDatabase();

	str_buf buf;

	buf << dname << ':' << "host=" << host
	    << ";dbname=" << dbname;
	result.value_ = buf.zstr();

	return result;
}

htab_rc 
IDriver::getConnectOptions()
{
	htab_rc result;

	htab_rw options(result);

	options.push_back((int)PDO_ATTR_ERRMODE);
	options.push_back((int)PDO_ERRMODE_EXCEPTION);
	return result;
}

error_return 
IDriver::connect()
{
	error_return result;

	if (handle_.ok())
	{
		return result;
	}

	str_return test = getDSN();

	if (test.has_errors())
	{
		result = std::move(test);
		return result;
	}

	htab_rc options = getConnectOptions();

	IConfig* cfg = icfg_c();

	str_rc user = cfg->getUsername();
	str_rc pw = cfg->getPassword();

	htab_rc  args_mgr;
	htab_rw args(args_mgr);

	args.push_back(test.value_);
	args.push_back(user);
	args.push_back(pw);
	args.push_back(options);

	handle_ = ReflectCache::staticInstanceArgs(DBS.pdo_class, args_mgr);

	afterConnect();

	return result;
}

void //virtual
IDriver::afterConnect()
{

}

val_rc 
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

htab_return 
IDriver::getTableNames()
{
	htab_return result;

	result.error() << "Not implemented in IDriver";

	return result;
}


void 
IDriver::close()
{
	handle_.set_null();
}

str_rc 
IDriver::getSqlType()
{
	return DBS.mysql_str;
}

bool 
IDriver::commit()
{
	obj_rc pdo = handle();

	if (pdo.ok())
	{
		val_rc result = pdo.call(DBS.commit_fn);
		return result.isTrue();
	}
	return false;
}

str_rc 
IDriver::quoteName(str_ptr name)
{
	return isql_c()->quoteName(name);
}	

bool 
IDriver::closeStmt(val_ptr stmt)
{
	obj_ptr sobj(stmt);
	val_rc result = sobj.call(DBS.close_cursor);
	return result.isTrue();
}


void IDriver::bind(val_ptr stmt, htab_ptr params)
{

	obj_ptr spdo(stmt);

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
					val_rc check = bvcall.call_fn();
				}
			}
			else {
				ZVAL_LONG(args, ix+1);
				ZVAL_COPY_VALUE(args+1, val);

				int ptype = pdo_type(val.ztype());
				ZVAL_LONG(args+2, ptype);
				val_rc check = bvcall.call_fn();
				
			}
		}
	}
}

val_rc 
IDriver::getCaseAttribute()
{
	return getAttribute(PDO_ATTR_CASE);
}

void 
IDriver::setCaseAttribute(int value)
{
	val_rc arg(value);

	setAttribute(PDO_ATTR_CASE, arg);
}

bool 
IDriver::begin()
{
	obj_ptr pdo(handle());
	bool result = false;
	if (pdo.ok())
	{
		val_rc test = pdo.call(DBS.begin_trans);
		result = test.isTrue();
	}
	return result;
}

str_rc 
IDriver::escape(str_ptr value)
{
	obj_ptr pdo(handle());
	val_rc  arg(value);
	str_rc result = pdo.call(DBS.quote_fn, arg);
	result = preg_replace(DBS.regex_quoted, DBS.rx_cap1, result);
	return result;
}

error_return 
check_results(val_ptr test)
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
IDriver::execute(val_ptr stmt, bool close, bool fetch)
{

	obj_ptr sobj(stmt);

	val_rc pdo_result = sobj.call(DBS.execute_fn);
	val_return result;

	bool good_result = pdo_result.isTrue();

	if (good_result)
	{
		if (fetch) {
			val_rc farg(ifetch_);
			result.value_ = sobj.call(DBS.fetchall_fn, farg);
		}
		else {
			result.value_ = sobj.call(DBS.rowcount_fn);
		}
	}
	if (close || !good_result)
	{
		closeStmt(stmt);

		if (isAutoCommit() && inTransaction())
		{
			commit();
		}
	}

	if (good_result) {
		if (fetch)
		{
			error_return derr = check_results(result.value_);
			if (derr.has_errors())
			{
				result = std::move(derr);
			}
		}
	}
	else {
		result.value_.set_bool(false);
	}

	return result;
}

htab_rc 
IDriver::fetchAllRows(val_ptr stmt, int mode)
{
	obj_ptr sobj(stmt);
	htab_rc result;

	if (sobj.ok())
	{
		val_rc farg(mode);
		result = sobj.call(DBS.fetchall_fn, farg);
	}
	if (!result.size())
	{
		result = htab_rc::empty_array();
	}
	return result;
}

val_rc 
IDriver::fetchRow(val_ptr stmt, int mode)
{
	obj_ptr sobj(stmt);
	val_rc farg(mode);
	return sobj.call(DBS.fetch_str, farg);
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
		zend_printf("readSchema\n");
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

val_rc 
IDriver::getAttribute(int key)
{
	obj_rc pdo(handle());

	val_rc arg(key);

	return pdo.call(DBS.getattribute_fn, arg);

}

val_return 
IDriver::querySingle(str_ptr query)
{
	val_return result;

	obj_ptr pdo(handle());

	val_rc arg1(query);
	obj_rc stmt = pdo.call(DBS.query_str, arg1);

	if (!stmt.ok())
	{
		result.error() << "pdo call " << query;
		return result;
	}

	arg1 = (zend_long) ifetch_;
	result.value_ = stmt.call(DBS.fetch_str, arg1);

	stmt.call(DBS.close_cursor);

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
		zend_printf("ModelClass not found %s\n", modelClass.data());
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
	str_rc stype = getSqlType();
	stype = stype.ucfirst();
	str_buf buf;

	buf << "Wcd\\Schema\\" << stype << "\\Dump";

	return buf.zstr();
}

bool 
IDriver::inTransaction()
{
	obj_rc pdo(handle());

	val_rc result = pdo.call(DBS.intransaction_fn);
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

str_rc 
IDriver::lastSQL() const
{
	return lastsql_;
}

val_rc 
IDriver::lastInsertId()
{
	obj_rc pdo(handle());

	return pdo.call(DBS.lastinsertid_fn);
}

val_return 
IDriver::lastSeqValue(str_ptr name)
{
	str_buf buf;

	buf << "select lastval(" << name << ")";
	str_rc  sql = buf.zstr();
	htab_ptr empty;

	val_return result = query(sql, empty);

	if (!result.has_errors())
	{	
		val_rc& test = result.value_;
		if (test.isArray())
		{
			htab_ptr rows(test);
			result.value_ = rows.get(int(0));
		}
	}
	return result;
}

str_rc 
IDriver::param(int pno)
{
	return DBS.place_holder;		
}

val_return
IDriver::prepare(str_ptr query)
{
	//zend_printf("IDriver::prepare-- ");
	val_return result;

	obj_rc pdo(handle());

	//showstr("Last SQL", lastsql_);
	// Duplicate, to try and resolve 
	// mystery interaction with PDO that can occur 
	// with reference count error for sql string
	lastsql_ = query.duplicate();
	val_rc sql(query);
	result.value_ = pdo.call(DBS.prepare_fn, sql);

	if (!result.value_.ok())
	{
		result.error() << "Prepare: " << query;
	}

	return result;
}


val_return
IDriver::prepareQuery(str_ptr query, htab_ptr values, htab_ptr bindTypes)
{
	val_return result;

	result = prepare(query);

	if (result.has_errors())
	{
		return result;
	}

	obj_ptr stmt(result.value_);

	val_rc test;
	val_rc temp;


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
		result.error() << "Statement execute failed: " << query;
	}
	return result;
}

val_return
IDriver::query(str_ptr query, htab_ptr params)
{
	htab_ptr btypes;

	val_return result = prepareQuery(query, params, btypes);

	if (result.has_errors())
	{
		return result;
	}

	obj_rc stmt = result.value_.zobject();

	val_rc data = stmt.call(DBS.fetchall_fn);

	stmt.call(DBS.close_cursor);

	error_return check = check_results(data);

	if (check.has_errors()) 
	{
		result.value_.set_bool(false);
		result = std::move(check);
	}
	else {
		result.value_ = data;
	}

	return result;
}

bool 
IDriver::rollback()
{
	obj_rc pdo(handle());

	val_rc result = pdo.call(DBS.rollback_fn);

	return result.isTrue();
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

bool 
IDriver::setAttribute(int key, val_ptr value)
{
	obj_rc pdo(handle());

	val_rc arg1(key);
	val_rc result = pdo.call(DBS.setattribute_fn, arg1, value);

	return result.isTrue();
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
	zval* stmt;
	zend_long   mode = PDO_FETCH_ASSOC;

	ZEND_PARSE_PARAMETERS_START(1,2)
	Z_PARAM_OBJECT(stmt)
	Z_PARAM_OPTIONAL
	Z_PARAM_LONG(mode);
	ZEND_PARSE_PARAMETERS_END();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);	

	htab_rc result = db->fetchAllRows(stmt, mode);

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

	htab_rc result = db->fetchRow(stmt, mode);

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, getAttribute)
{
	zend_long   key;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_LONG(key)
	ZEND_PARSE_PARAMETERS_END();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);	

	val_rc result = db->getAttribute(key);

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, getCaseAttribute)
{
	ZEND_PARSE_PARAMETERS_NONE();
	IDriver* db = zval_toc<IDriver>(ZEND_THIS);	

	val_rc result = db->getCaseAttribute();
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

	htab_return result = db->getColumnNames(table);
	result.throw_errors();
	result.value_.move_zv(return_value);
}

ZEND_METHOD(Wcd_IDriver, getConnectOptions)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	htab_rc result = db->getConnectOptions();
	result.move_zv(return_value);
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

	val_rc result = db->handle();

	result.move_zv(return_value);
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
	ZEND_PARSE_PARAMETERS_NONE();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);

	val_rc result = db->lastInsertId();

	result.move_zv(return_value);
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

	zarg_rd args(execute_data);

	args.zstring(sql, args.need(0));

	if (!args.throw_errors())
	{
		IDriver* db = zval_toc<IDriver>(ZEND_THIS);	
		val_return result = db->prepare(sql);
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
		val_return result = db->prepareQuery( sql, values, types);
		result.throw_errors();
		result.value_.move_zv(return_value);
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
	zend_string* sql;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(sql)
	ZEND_PARSE_PARAMETERS_END();

	IDriver* db = zval_toc<IDriver>(ZEND_THIS);	

	val_return result = db->querySingle( sql );
	result.value_.move_zv(return_value);	
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

	zend_class_entry* dclass = register_class_Wcd_IDriver();

	IDriver::omg.classEntry(dclass);
	Pdo_pgsql::register_class(dclass);


	class_data cval(IDriver::omg.class_entry_);

	cval.add_constant("FETCH_OBJECT", PDO_FETCH_OBJ);
	cval.add_constant("FETCH_ASSOC", PDO_FETCH_ASSOC);
	cval.add_constant("FETCH_NUM", PDO_FETCH_NUM);

	STATE_INIT_ADD(DBS)

	return SUCCESS;
}
#endif