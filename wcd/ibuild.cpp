#ifndef WCD_IBUILD_CPP
#define WCD_IBUILD_CPP

#ifndef WCD_IBUILD_H
#include "ibuild.h"
#endif

#ifndef WCD_IDRIVER_H
#include "idriver.h"
#endif

#ifndef SQL_PART_H
#include "sql_ipart.h"
#endif

#ifndef WCD_IROW_H
#include "irow.h"
#endif

#ifndef WCD_SQL_RAW_H
#include "raw.h"
#endif

#ifndef DB_ARGINFO_H
#define DB_ARGINFO_H
extern "C" {
     #include "stub/db_arginfo.h"
}
#endif

namespace wcd {
using namespace zpp;

	base_obj_mgr<IBuild> IBuild::omg;

	void IBuild::construct(val_ptr driver)
	{
		obj_rc dbobj(driver.zobject());
		IDriver* db = zobj_toc<IDriver>(dbobj);

		driver_ = db->getName();
		bindings_ = db->newBindings();
		isql_ = db->isql_;

	}

	void throw_array_hole(unsigned int ix)
	{
		str_buf buf;

		buf << "Where list has missing index " << ix;

		zend_throw_error(zend_ce_error, buf.data());
	}

	void 
	IBuild::distinct(bool set)
	{
		Bindings& bind = bindings();
		val_rc value(set);
		bind.set((int) ISql::SQL_DISTINCT, value);
	}
	void IBuild::where_unpack(htab_ptr aw)
	{
		auto wlen = aw.size();
		if (wlen==0)
		{
			return;
		}
		val_ptr p0 = aw.get(int(0));
		if (p0.isNull())
		{
			throw_array_hole(0);
			return;
		}
		val_ptr p3;
		if (wlen > 3)
		{
			p3 = aw.get(int(3));
			if (p3.isNull())
			{
				throw_array_hole(3);
				return;
			}
		}
		val_ptr p2;
		if (wlen > 2)
		{
			p2 = aw.get(int(2));
			if (p2.isNull())
			{
				throw_array_hole(2);
				return;
			}
		}
		val_ptr p1 = aw.get(int(1));
		if (p1.isNull()) 
		{
			throw_array_hole(1);
			return;
		}

		switch(wlen)
		{
		case 4:
			where(p0,p1,p2,p3);
			break;
		case 3:
			where(p0,p1,p2, val_ptr());
			break;
		case 2:
			where(p0,p1, val_ptr(), val_ptr());
			break;
		}
	}

	void IBuild::where_list(htab_ptr aw)
	{
		auto wlen = aw.size();

		if (wlen==0)
		{
			return;
		}

		val_ptr p0 = aw.get(int(0));

		if (p0.isNull())
		{
			throw_array_hole(0);
			return;
		}
		if (p0.isArray())
		{
			htab_walk wk;
			auto item = wk.value();
			for(wk.start(aw); wk.ok(); wk.next())
			{
				where_list(item.zarray());
			}
		}
		else {
			where_unpack(aw);
		}
	}

	ISql& 
	IBuild::isql()
	{
		return *(zobj_toc<ISql>(isql_));
	}

	Bindings& 
	IBuild::bindings()
	{
		return *(zobj_toc<Bindings>(bindings_));
	}

	obj_rc
	IBuild::getDb()
	{
		return IServer::connect(driver_);
	}

	IDriver& 
	IBuild::idb()
	{
		obj_rc db = IServer::connect(driver_);
		return *(zobj_toc<IDriver>(db));
	}

	void IBuild::destruct()
	{
		isql_.init();
		bindings_.init();
		model_.init();
	}

	void 
	IBuild::setReturns(htab_ptr names)
	{
		Bindings& bind = bindings();
		bind.addarray(ISql::SQL_RETURN, names);
	}

	val_rc 
	IBuild::insert(val_ptr rdata)
	{
		//zend_printf("IBuild::insert\n");

		Bindings& bind = bindings();
		bind.wipe(ISql::SQL_INSERT);

		val_rc result;
		obj_rc row_mgr;
		htab_rc timeStamps;
		htab_ptr rows;

		bool     is_multiple = false;

		if (rdata.isArray())
		{
			rows = rdata.zarray();

			int rowct = rows.size();
			if (rowct == 0)
			{
				result.set_bool(false);
				return result;

			}
			else if (rowct > 0){
				row_mgr = rows.get(int(0));
			}
			is_multiple = (rowct > 1);

		}
		else if (rdata.isObject())
		{
			row_mgr = rdata.zobject();
		}

		IRow* irow = zobj_toc<IRow>(row_mgr);
		obj_ptr model_mgr = irow->getModel();

		Model* model = zobj_toc<Model>(model_mgr);

		if (model->hasTimeStamps()) {
			timeStamps = irow->stampTime(now());
		}
		val_rc temp;

		if (is_multiple) {
			htab_walk wk;
			auto row = wk.value();

			for(wk.start(rows); wk.ok(); wk.next())
			{
				irow = zval_toc<IRow>(row);

				if (timeStamps.size()) {
					temp = timeStamps;
					irow->mergeData(temp);
				}
				temp = irow->getData();
				bind.addarray(ISql::SQL_INSERT, temp);
			}
		}
		else {
			// already stamped time.
			temp = irow->getData();
			bind.addarray(ISql::SQL_INSERT, temp);
		}

		
		obj_rc plist_mgr = isql().insert(bind);
		//showobj("plist_mgr", plist_mgr);

		bind.wipe(ISql::SQL_INSERT);

		if (plist_mgr.ok())
		{
			ParamList* plist = zobj_toc<ParamList>(plist_mgr);
			str_ptr sql(plist->getSql());
			htab_ptr values(plist->getValues());
			htab_ptr rets(plist->getReturns());

			IDriver& db = idb();

			int fetch = db.setFetch(IDriver::FETCH_ASSOC);

			result = RunSql::op(getDb(), sql, values, (rets.size() > 0));

			//showmem("result", result);
			db.setFetch(fetch);
		}
		return  result;
	}

	void 
	IBuild::debug_info(htab_rw di)
	{
		di.set(SQSTR.driver, driver_);
		di.set(SQSTR.isql, isql_);

		di.set(SQSTR.bind_key, bindings_);

		di.set(SQSTR.model, model_);
		di.set(SQSTR.modelclass, modelClass_);
		di.set(SQSTR.columns, columns_);
	}

	val_rc
	IBuild::aggregate(str_ptr agfn, htab_ptr columns)
	{
		//model_.init();

		htab_rc args_mgr;
		htab_rw args(args_mgr);

		args.set(SQSTR.function, agfn);
		args.set(SQSTR.columns, columns);

		Bindings& bind = bindings();
		val_rc temp(args_mgr);
		bind.set(ISql::SQL_AGGREGATE, temp);
		bind.set(ISql::FETCH_AS, IDriver::FETCH_OBJECT);
		bind.unset(ISql::MODEL_OBJ);
		
		val_rc result = bind.select();

		if (result.isArray())
		{
			htab_ptr rows(result);
			if (rows.size())
			{
				obj_rc robj = rows.get(int(0));
				result = robj.property(agfn);
			}
		}
		//showmem("aggregate result", result);
		return result;
	}

	val_rc 
	IBuild::update(obj_ptr irow, htab_ptr dirty)
	{
		Bindings& bind = bindings();

		IRow* rowobj = zobj_toc<IRow>(irow);

		val_rc result;

		if (dirty.size())
		{
			htab_walk wk;

			auto cvalue = wk.value();

			for(wk.start(dirty); wk.ok(); wk.next())
			{
				bind.update(cvalue, rowobj->get(cvalue));
			}

			htab_rc ts = rowobj->stampTime(now(), Model::UPDATE_TS);

			if (ts.size())
			{
				auto column = wk.key();

				for(wk.start(ts); wk.ok(); wk.next())
				{
					bind.update(column, cvalue);
				}
			}

			obj_rc plist_mgr = isql().update(bind);
			ParamList* plist = zobj_toc<ParamList>(plist_mgr);

			str_ptr sql(plist->getSql());
			htab_ptr params(plist->getValues());

			result = RunSql::op(getDb(), sql, params);
		}

		return result;
	}

	void 
	IBuild::table(str_ptr table, bool wipe)
	{
		Bindings& bind = bindings();

		if (wipe)
		{
			bind.wipe();
		}

		bind.addstr(ISql::SQL_FROM, table);
	}

	void 
	IBuild::limit(int lim, int offset)
	{
		Bindings& bind = bindings();
		val_rc a1(lim);
		val_rc a2(offset);
		bind.limit(a1, a2);
	}

	int 
	IBuild::count(val_ptr columns)
	{
		htab_rc names_mgr;
		htab_rw names(names_mgr);

		if (columns.isString())
		{
			htab_rw hw(names);
			names.push_back(columns.zstr());
		}
		else if (columns.isArray()) {
			names_mgr = columns.zarray();
		}
		else {
			names.push_back(SQSTR.asterisk);
		}
		val_rc result = aggregate(SQSTR.count_str, names_mgr);
		return result.zlong();
	}

	val_rc 
	IBuild::deleteRow(obj_ptr rowobj)
	{
		Model* model = zobj_toc<Model>(model_);
		IRow*  irow = zobj_toc<IRow>(rowobj);

		val_rc null_result;


		htab_ptr pkey = model->getPKey();
		//zend_printf("deleteRow - ");
		//showdata("pkey", pkey);

	    int pkeyct = pkey.size();
	    Bindings& bind = bindings();


	    if (pkeyct)
	    {
	    	htab_rc values = irow->getDataValues(pkey);
	    	//showdata("values", values);
	    	for( int ix = 0; ix < pkeyct; ix++)
	    	{
	    		val_ptr key = pkey.get(ix);
	    		val_ptr value = values.get(key);
	    		//showmem("key value", value);
	    		bind.where(key, SQSTR.cmp_equal, value, SQSTR.and_str);
	    	}
	    }
	    else {
	    	//WHERE alread set?
	    	val_ptr wcond = bind.get(ISql::SQL_WHERE);
	    	if (wcond.isNull())
	    	{
	    		zend_throw_error(zend_ce_error,"deleteRow() without primary key or where condition set");
	    		return null_result;
	    	}
	    }
	    ISql& sp = isql();
	    obj_rc params_mgr =  sp.deleteSql(bind);
	    ParamList* plist = zobj_toc<ParamList>(params_mgr);
	    str_ptr sql(plist->getSql());
	    //showstr("delete sql", sql);
	    htab_ptr params(plist->getValues());
	    //showdata("delete params", params);

	    return RunSql::op(getDb(), sql, params);


	}

	void 
	IBuild::whereKeyValue(val_ptr key, val_ptr value)
	{
		Bindings& bind = bindings();
		bind.whereKeyValue(key, value);
	}

	void
	IBuild::where(val_ptr column, val_ptr opcmp, val_ptr value, val_ptr bval)
	{
		val_rc arg2;
		val_rc arg3;
		val_rc arg4;

		if (!value.ok())
		{
			arg2 = SQSTR.cmp_equal;;
			arg3 = opcmp;
		}
		else {
			arg2 = opcmp;
			arg3 = value;
		}

		if (!bval.ok())
		{
			arg4 = SQSTR.and_str;
		}
		else {
			arg4 = bval;
		}
		where(column, arg2.zstr(), arg3, arg4.zstr());

	}

	void
	IBuild::where(val_ptr column, str_ptr opcmp, val_ptr value, str_ptr bval)
	{
		Bindings& bind = bindings();

		if (column.isString())
		{
			bind.where(column, opcmp, value, bval);
			return;
		}
		if (column.isArray())
		{
			where_list(column.zarray());
			return;
		}
		if (column.isObject())
		{
			obj_rc raw = column.zobject();
			if (raw.instanceof(Raw::omg.classEntry())) {
				whereRaw(raw, value, bval);
				return;
			}
		}
		str_buf buf;
		buf << "column type '" << zend_zval_type_name(column) << "' not supported";

		zend_throw_error(zend_ce_error, buf.data());
		return;
		
	}

	obj_rc 
	IBuild::getInsertSql(htab_ptr columns)
	{
		Bindings& bind = bindings();
		bind.wipe(ISql::SQL_INSERT);

		bind.addarray(ISql::SQL_INSERT, columns);

		ISql* isql = zobj_toc<ISql>(isql_);

		return isql->insert(bind);

	}

	val_rc
	IBuild::get_first()
	{
		Bindings& bind = bindings();
		val_rc a1(1);
		val_rc a2;

		bind.limit(a1, a2);
		val_rc result = bind.select();

		if (result.isArray())
		{
			htab_ptr rdata(result);
			if (rdata.size())
			{
				result = rdata.get(int(0));
			}
			else {
				result.set_null();
			}
		}
		return result;
	}

	val_rc 
	IBuild::oneRow()
	{
		columns_.init();
		return get_first();
	}


	val_rc 
	IBuild::allRows()
	{
		return bindings().select();
	}

	val_rc 
	IBuild::first(htab_ptr columns)
	{
		if (columns.size())
		{
			Bindings& bind = bindings();
			bind.set(ISql::NAME_LIST, columns);
		}
		return get_first();
	}

	val_rc
	IBuild::get(htab_ptr columns)
	{
		Bindings& bind = bindings();
		if (columns.size())
		{
			bind.set(ISql::NAME_LIST, columns);
		}
		
		return bind.select();
	}

	str_rc 
	IBuild::now()
	{
		datetime_obj dtime;

		//zend_printf("IBuild::now() ");
		//showstr("now_format", DTData.now_format);
		
		return dtime.format(DTData.now_format);
	}

	void 
	IBuild::offset(int value)
	{
		Bindings& bind = bindings();

		bind.offset(value);
	}

	void 
	IBuild::orderBy(val_ptr colname, bool descend)
	{
		Bindings& bind = bindings();

		bind.orderBy(colname, descend);
	}

	val_rc 
	IBuild::seqLastValue(str_ptr seqname)
	{

		str_ptr sql = isql().seqLastValue(seqname);

		val_rc result = RunSql::op(getDb(), sql);

		if (result.ok()) {
			htab_ptr rows(result);
			htab_ptr r1(rows.get(int(0)));
			htab_walk wk;
			wk.start(r1);
			return wk.value();
		}
		return result;
	}

	void 
	IBuild::set(str_ptr cname, val_ptr value)
	{
		Bindings& bind = bindings();
		bind.update(cname, value);	
	}

	void 
	IBuild::setInsert(htab_ptr data)
	{
		Bindings& bind = bindings();
		bind.addarray(ISql::SQL_INSERT, data);
	}

	void 
	IBuild::setModel(obj_ptr model, bool bind)
	{
		if (model.ok())
		{
			if (!model.instanceof(Model::omg.class_entry_))
			{
				zend_throw_error(zend_ce_error, "Object not Model class");
				return;
			}
		}
		model_ = model;

		if (model.ok() && bind)
		{
			//showobj("set model", model);
			Model* m = zobj_toc<Model>(model);
			str_rc name = m->getName();

			//zend_printf("table %s\n", name.data());
			table(name);

			Bindings& bind = bindings();
			val_rc temp(model);
			bind.set(ISql::MODEL_OBJ, temp);
			bind.set(ISql::FETCH_AS, IDriver::FETCH_ASSOC);
		}
	}

	val_rc 
	IBuild::setSeqValue(int value, htab_ptr data)
	{
		str_rc sql = isql().setSeqValue(value, data);
		val_rc result = RunSql::op(getDb(), sql);

		if (result.isArray()) {
			htab_ptr rows(result);
			htab_ptr r1(rows.get(int(0)));
			result = htab_walk::first(r1);
		}
		return result;
	}

	void IBuild::wipe()
	{
		bindings().wipe();
	}

}; // namespace wcd



using namespace wcd;

ZEND_METHOD(Wcd_IBuild, __construct)
{
	zval* dbobj;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT_OF_CLASS(dbobj, IDriver::omg.class_entry_)
	ZEND_PARSE_PARAMETERS_END();

	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);

	cobj->construct(dbobj);
}

ZEND_METHOD(Wcd_IBuild, __destruct)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);

	cobj->destruct();
}

ZEND_METHOD(Wcd_IBuild, aggregate)
{
	zend_string* func;
	zval*        columns = nullptr;

	ZEND_PARSE_PARAMETERS_START(1,2)
	Z_PARAM_STR(func)
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY_OR_NULL(columns)
	ZEND_PARSE_PARAMETERS_END();

	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);

	val_rc result = cobj->aggregate(func, columns);

	result.move_zv(return_value);
}

//public function allRows() : mixed
ZEND_METHOD(Wcd_IBuild, allRows)
{
	ZEND_PARSE_PARAMETERS_NONE();
	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);
	val_rc result = cobj->allRows();
	result.move_zv(return_value);
}

//public function count(string|array $columns = "*") : int
ZEND_METHOD(Wcd_IBuild, count)
{
	HashTable* list = nullptr;
	zend_string* column = nullptr;

	ZEND_PARSE_PARAMETERS_START(0,1);
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY_HT_OR_STR_OR_NULL(list, column)
	ZEND_PARSE_PARAMETERS_END();

	val_rc columns;

	if (column)
	{
		columns = column;
	}
	else if (list)
	{
		columns = list;
	}

	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);
	RETURN_LONG(cobj->count(columns));
}

//	val_rc deleteRow(obj_ptr rowobj)
ZEND_METHOD(Wcd_IBuild, deleteRow)
{
	zval* irow;

	ZEND_PARSE_PARAMETERS_START(1,1);
	Z_PARAM_OBJECT_OF_CLASS(irow, IRow::omg.class_entry_)
	ZEND_PARSE_PARAMETERS_END();

	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);
	val_rc result = cobj->deleteRow(irow);
	result.move_zv(return_value);
}

//public function distinct(bool $set = true): void
ZEND_METHOD(Wcd_IBuild, distinct)
{
	bool set = true;
	ZEND_PARSE_PARAMETERS_START(0,1)
	Z_PARAM_OPTIONAL
	Z_PARAM_BOOL(set)
	ZEND_PARSE_PARAMETERS_END();

	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);

	cobj->distinct(set);
}

//public function first(?array $columns = null) : mixed
ZEND_METHOD(Wcd_IBuild, first)
{
	HashTable* ht = nullptr;

	ZEND_PARSE_PARAMETERS_START(0,1)
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY_HT_OR_NULL(ht)
	ZEND_PARSE_PARAMETERS_END();

	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);
	val_rc result = cobj->first(htab_ptr(ht));
	result.move_zv(return_value);
}

//public function get(?array $columns = null) : mixed
ZEND_METHOD(Wcd_IBuild, get)
{
	HashTable* ht = nullptr;

	ZEND_PARSE_PARAMETERS_START(0,1)
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY_HT_OR_NULL(ht)
	ZEND_PARSE_PARAMETERS_END();

	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);
	val_rc result = cobj->get(htab_ptr(ht));
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IBuild, getDriver)
{
	ZEND_PARSE_PARAMETERS_NONE();
	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);
	obj_rc result = cobj->getDb();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IBuild, getBindings)
{
	ZEND_PARSE_PARAMETERS_NONE();
	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);
	obj_rc result = cobj->bindings_;
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IBuild, getSql)
{
	ZEND_PARSE_PARAMETERS_NONE();
	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);
	obj_rc result = cobj->isql_;
	result.move_zv(return_value);	
}

ZEND_METHOD(Wcd_IBuild, getFrom)
{
	ZEND_PARSE_PARAMETERS_NONE();
	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);
	Bindings& bind = *zobj_toc<Bindings>(cobj->bindings_);


	obj_ptr result = bind.get(ISql::SQL_FROM);

	result.return_zv(return_value);
}

ZEND_METHOD(Wcd_IBuild, getInsertSql)
{
	HashTable* columns = nullptr;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY_HT(columns)
	ZEND_PARSE_PARAMETERS_END();

	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);
	obj_rc result = cobj->getInsertSql(columns);
	result.move_zv(return_value);
}


ZEND_METHOD(Wcd_IBuild, hasModel)
{
	ZEND_PARSE_PARAMETERS_NONE();
	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);
	bool result = cobj->hasModel();
	RETURN_BOOL(result);
}

//public function insert(array|IRow $rows) : mixed
ZEND_METHOD(Wcd_IBuild, insert)
{
	zval* data;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ZVAL(data)
	ZEND_PARSE_PARAMETERS_END();
	
	val_ptr test(data);
	if (test.isObject())
	{
		obj_rc irow(test.zobject());
		if (! irow.instanceof(IRow::omg.class_entry_))
		{
			zend_throw_error(zend_ce_error,"Not IRow object");
			RETURN_NULL();
		}	
	} 
	else if(!test.isArray())
	{
		zend_throw_error(zend_ce_error,"Not an Array");
		RETURN_NULL();
	}
	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);
	val_rc result = cobj->insert(test);
	result.move_zv(return_value);
}

//public function limit(int $limit, int $offset = 0): void
ZEND_METHOD(Wcd_IBuild, limit)
{
	zend_long limit;
	zend_long offset=0;

	ZEND_PARSE_PARAMETERS_START(1,2)
	Z_PARAM_LONG(limit)
	Z_PARAM_OPTIONAL
	Z_PARAM_LONG(offset)
	ZEND_PARSE_PARAMETERS_END();

	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);
	cobj->limit(limit,offset);
}

ZEND_METHOD(Wcd_IBuild, now)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);

	str_rc result = cobj->now();

	result.move_zv(return_value);
}

//public function offset(int $offset): void
ZEND_METHOD(Wcd_IBuild, offset)
{
	zend_long offset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_LONG(offset)
	ZEND_PARSE_PARAMETERS_END();

	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);


	cobj->offset(offset);

}

ZEND_METHOD(Wcd_IBuild, oneRow)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);

	val_rc result = cobj->oneRow();

	result.move_zv(return_value);
}

//public function orderBy(string $column, bool $descend = false) : void
ZEND_METHOD(Wcd_IBuild, orderBy)
{
	zarg_rd args(execute_data);

	bool         descend = false;
	val_ptr    colspec(args.need(0));

	args.zbool(descend, args.option(1));

	if (!args.throw_errors())
	{
		IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);
		cobj->orderBy(colspec, descend);
	}
}

//public function seqLastValue(string $seqname): ?int
ZEND_METHOD(Wcd_IBuild, seqLastValue)
{
	zend_string* sname;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(sname)
	ZEND_PARSE_PARAMETERS_END();

	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);
	
	val_rc result = cobj->seqLastValue(sname);
	result.move_zv(return_value);
}

//public function set(string $column, mixed $value): void
ZEND_METHOD(Wcd_IBuild, set)
{
	zend_string* cname;
	zval*        value;
	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_STR(cname)
	Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);

	cobj->set(cname, value);
}

//public function setFetch(int $mode) : int
ZEND_METHOD(Wcd_IBuild, setFetch)
{
	zend_long mode;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_LONG(mode)
	ZEND_PARSE_PARAMETERS_END();

	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);

	mode = cobj->setFetch(mode);

	RETURN_LONG(mode);
}

//public function setInsert(array $data) : void
ZEND_METHOD(Wcd_IBuild, setInsert)
{
	HashTable* data;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY_HT(data)
	ZEND_PARSE_PARAMETERS_END();

	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);

	cobj->setInsert(data);
}
//public function setModel(?Model $model = null, bool $bind = true): void
ZEND_METHOD(Wcd_IBuild, setModel)
{
	zval* model = nullptr;

	bool  bind = true;

	ZEND_PARSE_PARAMETERS_START(0,2)	
	Z_PARAM_OPTIONAL
	Z_PARAM_OBJECT_OR_NULL(model);
	Z_PARAM_BOOL(bind)
	ZEND_PARSE_PARAMETERS_END();

	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);

	cobj->setModel(model, bind); 	
}

//public function setModelClass(string $cname) : void
ZEND_METHOD(Wcd_IBuild, setModelClass)
{
	zend_string* cname;

	ZEND_PARSE_PARAMETERS_START(1,1)	
	Z_PARAM_STR(cname)
	ZEND_PARSE_PARAMETERS_END();

	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);

	cobj->modelClass_ = cname;
}

//public function setReturns(array $names) : void
ZEND_METHOD(Wcd_IBuild, setReturns)
{
	HashTable* data;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY_HT(data)
	ZEND_PARSE_PARAMETERS_END();

	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);

	cobj->setReturns(data);
}

//public function setSeqValue(int $value, array $data): ?int
ZEND_METHOD(Wcd_IBuild, setSeqValue)
{
	zend_long value;

	HashTable* data;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_LONG(value)
	Z_PARAM_ARRAY_HT(data)
	ZEND_PARSE_PARAMETERS_END();

	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);

	val_rc result = cobj->setSeqValue(value, data);

	result.move_zv(return_value);
}

//public function table(string $table, bool $wipe = true) : void
ZEND_METHOD(Wcd_IBuild, table)
{
	zend_string* tname;
	bool wipe = true;

	ZEND_PARSE_PARAMETERS_START(1,2)
	Z_PARAM_STR(tname)
	Z_PARAM_OPTIONAL
	Z_PARAM_BOOL(wipe)
	ZEND_PARSE_PARAMETERS_END();

	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);

	cobj->table(tname, wipe);
}

//public function update(IRow $row, array $dirty = []) : mixed
ZEND_METHOD(Wcd_IBuild, update)
{
	zval* row;

	HashTable* dirty = (HashTable*) &zend_empty_array;

	ZEND_PARSE_PARAMETERS_START(1,2)
	Z_PARAM_OBJECT_OF_CLASS(row, IRow::omg.class_entry_);
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY_HT(dirty)
	ZEND_PARSE_PARAMETERS_END();

	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);

	val_rc result = cobj->update(row, dirty);

	result.move_zv(return_value);
}

/* public function where(mixed $column, ?string $operator = null, 
                        mixed $value = null, string $bval = "AND") : void */
ZEND_METHOD(Wcd_IBuild, where)
{
	zval* column;
	zend_string* opstr = nullptr;
	zval* value = nullptr;
	zend_string* bval = nullptr;

	ZEND_PARSE_PARAMETERS_START(1,4)
	Z_PARAM_ZVAL(column)
	Z_PARAM_OPTIONAL
	Z_PARAM_STR_OR_NULL(opstr)
	Z_PARAM_ZVAL(value)
	Z_PARAM_STR_OR_NULL(bval)
	ZEND_PARSE_PARAMETERS_END();

	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);

	cobj->where(column, opstr, value, bval);
}

ZEND_METHOD(Wcd_IBuild, wipe)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);

	cobj->wipe();

}

PHP_MINIT_FUNCTION(Wcd_IBuild_reg)
{
	IBuild::omg.classEntry(register_class_Wcd_IBuild());

	return SUCCESS;
}

#endif