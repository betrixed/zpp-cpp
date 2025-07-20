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

	void IBuild::construct(zval_user driver)
	{
		driver_ = driver;
		params_ = ParamList::omg.new_zobj();
		ParamList* plist = zobj_toc<ParamList>(params_);

		plist->construct(driver);

		IDriver* db = zobj_toc<IDriver>(driver_);
		isql_ = db->isql_;

		bindings_ = Bindings::omg.new_zobj();
		Bindings& bind = *zobj_toc<Bindings>(bindings_);

		zval_mgr sqlmgr(isql_);

		bind.construct(sqlmgr, driver);
		bind.setParamList(params_);

	}

	void throw_array_hole(unsigned int ix)
	{
		zstr_buffer buf;

		buf << "Where list has missing index " << ix;

		zend_throw_error(zend_ce_error, buf.data());
	}

	void 
	IBuild::distinct(bool set)
	{
		Bindings& bind = bindings();
		zval_mgr value(set);
		bind.set((int) ISql::SQL_DISTINCT, value);
	}
	void IBuild::where_unpack(htab_read aw)
	{
		auto wlen = aw.size();
		if (wlen==0)
		{
			return;
		}
		zval_user p0 = aw.get(int(0));
		if (p0.isNull())
		{
			throw_array_hole(0);
			return;
		}
		zval_user p3;
		if (wlen > 3)
		{
			p3 = aw.get(int(3));
			if (p3.isNull())
			{
				throw_array_hole(3);
				return;
			}
		}
		zval_user p2;
		if (wlen > 2)
		{
			p2 = aw.get(int(2));
			if (p2.isNull())
			{
				throw_array_hole(2);
				return;
			}
		}
		zval_user p1 = aw.get(int(1));
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
			where(p0,p1,p2, zval_user());
			break;
		case 2:
			where(p0,p1, zval_user(), zval_user());
			break;
		}
	}

	void IBuild::where_list(htab_read aw)
	{
		auto wlen = aw.size();

		if (wlen==0)
		{
			return;
		}

		zval_user p0 = aw.get(int(0));

		if (p0.isNull())
		{
			throw_array_hole(0);
			return;
		}
		if (p0.isArray())
		{
			htab_walk wk;
			auto item = wk.value();
			for(wk.start(p0.zarray()); wk.ok(); wk.next())
			{
				//Only process arrays
				if (item.isArray())
				{
					where_list(item.zarray());
				}
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

	IDriver& 
	IBuild::idb()
	{
		return *(zobj_toc<IDriver>(driver_));
	}

	void IBuild::destruct()
	{
		isql_.init();
		driver_.init();
		params_.init();
		bindings_.init();
		model_.init();
	}

	void 
	IBuild::setReturns(htab_read names)
	{
		Bindings& bind = bindings();
		bind.addarray(ISql::SQL_RETURN, names);
	}

	zval_mgr 
	IBuild::insert(zval_user rdata)
	{
		Bindings& bind = bindings();
		bind.wipe(ISql::SQL_INSERT);
		ParamList* plist = zobj_toc<ParamList>(params_);

		plist->wipe();

		zval_mgr result;
		zobj_mgr row_mgr;
		htab_mgr timeStamps;
		htab_read rows;

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
		zobj_user model_mgr = irow->getModel();

		Model* model = zobj_toc<Model>(model_mgr);

		if (model->hasTimeStamps()) {
			timeStamps = irow->stampTime(now());
		}
		zval_mgr temp;

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

		zobj_mgr plist_mgr = isql().insert(bind);
		//showobj("plist_mgr", plist_mgr);

		bind.wipe(ISql::SQL_INSERT);

		if (plist_mgr.ok())
		{
			ParamList* plist = zobj_toc<ParamList>(plist_mgr);
			zstr_mgr& sql = plist->getSql();
			htab_mgr& values = plist->getValues();
			htab_mgr& rets = plist->getReturns();

			IDriver& db = idb();

			int fetch = db.setFetch(IDriver::FETCH_ASSOC);

			result = RunSql::op(driver_, sql, values, (rets.size() > 0));

			//showmem("result", result);
			db.setFetch(fetch);
		}
		return  result;
	}

	void 
	IBuild::debug_info(htab_write di)
	{
		di.set(SQSTR.driver, driver_);
		di.set(SQSTR.isql, isql_);
		di.set(SQSTR.params, params_);

		di.set(SQSTR.bind_key, bindings_);

		di.set(SQSTR.model, model_);
		di.set(SQSTR.modelclass, modelClass_);
		di.set(SQSTR.columns, columns_);
	}

	zval_mgr
	IBuild::aggregate(zstr_user agfn, htab_read columns)
	{
		model_.init();

		htab_mgr args_mgr;
		htab_write args(args_mgr);

		args.set(SQSTR.function, agfn);
		args.set(SQSTR.columns, columns);

		Bindings& bind = bindings();
		zval_mgr temp(args_mgr);
		bind.set(ISql::SQL_AGGREGATE, temp);
		bind.set(ISql::FETCH_AS, IDriver::FETCH_OBJECT);


		zval_mgr result = bind.select();

		if (result.isArray())
		{
			htab_read rows(result);
			if (rows.size())
			{
				zobj_mgr robj = rows.get(int(0));
				result = robj.property(agfn);
			}
		}
		return result;
	}

	zval_mgr 
	IBuild::update(zobj_user irow, htab_read dirty)
	{
		Bindings& bind = bindings();

		IRow* rowobj = zobj_toc<IRow>(irow);

		zval_mgr result;

		if (dirty.size())
		{
			htab_walk wk;

			auto cvalue = wk.value();

			for(wk.start(dirty); wk.ok(); wk.next())
			{
				bind.update(cvalue, rowobj->get(cvalue));
			}

			htab_mgr ts = rowobj->stampTime(now(), Model::UPDATE_TS);

			if (ts.size())
			{
				auto column = wk.key();

				for(wk.start(ts); wk.ok(); wk.next())
				{
					bind.update(column, cvalue);
				}
			}

			zobj_mgr plist_mgr = isql().update(bind);
			ParamList* plist = zobj_toc<ParamList>(plist_mgr);

			zstr_mgr sql = plist->getSql();
			htab_mgr params = plist->getValues();

			result = RunSql::op(driver_, sql, params);
			
		}

		return result;
	}

	void 
	IBuild::table(zstr_user table, bool wipe)
	{
		Bindings& bind = bindings();

		if (wipe)
		{
			bind.wipe();
			ParamList* plist = zobj_toc<ParamList>(params_);
			plist->wipe();
		}

		bind.addstr(ISql::SQL_FROM, table);
	}

	void 
	IBuild::limit(int lim, int offset)
	{
		Bindings& bind = bindings();

		bind.limit(lim, offset);
	}

	int 
	IBuild::count(zval_user columns)
	{
		htab_mgr names_mgr;
		htab_write names(names_mgr);

		if (columns.isString())
		{
			htab_write hw(names);
			names.push_back(columns.zstr());
		}
		else if (columns.isArray()) {
			names_mgr = columns.zarray();
		}
		else {
			names.push_back(SQSTR.asterisk);
		}
		zval_mgr result = aggregate(SQSTR.count_str, names_mgr);
		return result.zlong();
	}

	zval_mgr 
	IBuild::deleteRow(zobj_user rowobj)
	{
		Model* model = zobj_toc<Model>(model_);
		IRow*  irow = zobj_toc<IRow>(rowobj);

		zval_mgr null_result;

		Bindings& bind = bindings();

	    htab_read pkey = model->getPKey();

	    int pkeyct = pkey.size();
	    if (pkeyct)
	    {
	    	htab_mgr values = irow->getDataValues(pkey);
	    	for( int ix = 0; ix < pkeyct; ix++)
	    	{
	    		zval_user key = pkey.get(ix);
	    		zval_user value = values.get(ix);
	    		bind.where(key, SQSTR.cmp_equal, value, SQSTR.and_str);
	    	}
	    }
	    else {
	    	//WHERE alread set?
	    	zval_user wcond = bind.get(ISql::SQL_WHERE);
	    	if (wcond.isNull())
	    	{
	    		zend_throw_error(zend_ce_error,"deleteRow() without primary key or where condition set");
	    		return null_result;
	    	}
	    }
	    ISql& sp = isql();
	    zobj_mgr params_mgr =  sp.deleteSql(bind);
	    ParamList* plist = zobj_toc<ParamList>(params_mgr);
	    zstr_mgr sql = plist->getSql();
	    htab_mgr params = plist->getValues();

	    return RunSql::op(driver_, sql, params);


	}

	void 
	IBuild::whereKeyValue(zval_user key, zval_user value)
	{
		Bindings& bind = bindings();
		bind.whereKeyValue(key, value);
	}

	void
	IBuild::where(zval_user column, zval_user opcmp, zval_user value, zval_user bval)
	{
		zval_mgr arg2;
		zval_mgr arg3;
		zval_mgr arg4;

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
	IBuild::where(zval_user column, zstr_user opcmp, zval_user value, zstr_user bval)
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
			zobj_mgr raw = column.zobject();
			if (raw.instanceof(Raw::omg.classEntry())) {
				whereRaw(raw, value, bval);
				return;
			}
		}
		zstr_buffer buf;
		buf << "column type '" << zend_zval_type_name(column) << "' not supported";

		zend_throw_error(zend_ce_error, buf.data());
		return;
		
	}

	zobj_mgr 
	IBuild::getInsertSql(htab_read columns)
	{
		Bindings& bind = bindings();
		bind.wipe(ISql::SQL_INSERT);

		ParamList* params = zobj_toc<ParamList>(params_);
		params->wipe();
		bind.addarray(ISql::SQL_INSERT, columns);

		ISql* isql = zobj_toc<ISql>(isql_);

		return isql->insert(bind);

	}

	zval_mgr
	IBuild::get_first()
	{
		Bindings& bind = bindings();
		bind.limit(1, 0);
		zval_mgr result = bind.select();

		if (result.isArray())
		{
			htab_read rdata(result);
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

	zval_mgr 
	IBuild::oneRow()
	{
		columns_.init();
		return get_first();
	}


	zval_mgr 
	IBuild::allRows()
	{
		return bindings().select();
	}

	zval_mgr 
	IBuild::first(htab_read columns)
	{
		if (columns.size())
		{
			Bindings& bind = bindings();
			bind.set(ISql::NAME_LIST, columns);
		}
		return get_first();
	}

	zval_mgr
	IBuild::get(htab_read columns)
	{
		Bindings& bind = bindings();
		if (columns.size())
		{
			bind.set(ISql::NAME_LIST, columns);
		}
		
		return bind.select();
	}

	zstr_mgr 
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
	IBuild::orderBy(zstr_user colname, bool descend)
	{
		Bindings& bind = bindings();

		bind.orderBy(colname, descend);
	}

	zval_mgr 
	IBuild::seqLastValue(zstr_user seqname)
	{

		zstr_user sql = isql().seqLastValue(seqname);

		zval_mgr result = RunSql::op(driver_, sql);

		if (result.ok()) {
			htab_read rows(result);
			htab_read r1(rows.get(int(0)));
			htab_walk wk;
			wk.start(r1);
			return wk.value();
		}
		return result;
	}

	void 
	IBuild::set(zstr_user cname, zval_user value)
	{
		Bindings& bind = bindings();
		bind.update(cname, value);	
	}

	int
	IBuild::setFetch(int mode)
	{
		IDriver& db = idb();

		return db.setFetch(mode);
	}

	void 
	IBuild::setInsert(htab_read data)
	{
		Bindings& bind = bindings();
		bind.addarray(ISql::SQL_INSERT, data);
	}

	void 
	IBuild::setModel(zobj_user model, bool bind)
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
			Model* m = zobj_toc<Model>(model);
			zstr_mgr name = m->getName();
			table(name);
		}
	}

	zval_mgr 
	IBuild::setSeqValue(int value, htab_read data)
	{
		zstr_mgr sql = isql().setSeqValue(value, data);
		zval_mgr result = RunSql::op(driver_, sql);

		if (result.isArray()) {
			htab_read rows(result);
			htab_read r1(rows.get(int(0)));
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

	zval_mgr result = cobj->aggregate(func, columns);

	result.move_zv(return_value);
}

//public function allRows() : mixed
ZEND_METHOD(Wcd_IBuild, allRows)
{
	ZEND_PARSE_PARAMETERS_NONE();
	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);
	zval_mgr result = cobj->allRows();
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

	zval_mgr columns;

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

//	zval_mgr deleteRow(zobj_user rowobj)
ZEND_METHOD(Wcd_IBuild, deleteRow)
{
	zval* irow;

	ZEND_PARSE_PARAMETERS_START(1,1);
	Z_PARAM_OBJECT_OF_CLASS(irow, IRow::omg.class_entry_)
	ZEND_PARSE_PARAMETERS_END();

	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);
	zval_mgr result = cobj->deleteRow(irow);
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
	zval_mgr result = cobj->first(htab_read(ht));
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
	zval_mgr result = cobj->get(htab_read(ht));
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IBuild, getDriver)
{
	ZEND_PARSE_PARAMETERS_NONE();
	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);
	zobj_mgr result = cobj->driver_;
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IBuild, getBindings)
{
	ZEND_PARSE_PARAMETERS_NONE();
	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);
	zobj_mgr result = cobj->bindings_;
	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_IBuild, getSql)
{
	ZEND_PARSE_PARAMETERS_NONE();
	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);
	zobj_mgr result = cobj->isql_;
	result.move_zv(return_value);	
}

ZEND_METHOD(Wcd_IBuild, getFrom)
{
	ZEND_PARSE_PARAMETERS_NONE();
	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);
	Bindings& bind = *zobj_toc<Bindings>(cobj->bindings_);


	zobj_user result = bind.get(ISql::SQL_FROM);

	result.return_zv(return_value);
}

ZEND_METHOD(Wcd_IBuild, getInsertSql)
{
	HashTable* columns = nullptr;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY_HT(columns)
	ZEND_PARSE_PARAMETERS_END();

	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);
	zobj_mgr result = cobj->getInsertSql(columns);
	result.move_zv(return_value);
}

//public function getParamList() : ?ParamList
ZEND_METHOD(Wcd_IBuild, getParamList)
{
	ZEND_PARSE_PARAMETERS_NONE();
	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);
	zobj_mgr result = cobj->params_;
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
	
	zval_user test(data);
	if (test.isObject())
	{
		zobj_mgr irow(test.zobject());
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
	zval_mgr result = cobj->insert(test);
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

	zstr_mgr result = cobj->now();

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

	zval_mgr result = cobj->oneRow();

	result.move_zv(return_value);
}

//public function orderBy(string $column, bool $descend = false) : void
ZEND_METHOD(Wcd_IBuild, orderBy)
{
	zend_string* colname;
	bool         descend = false;

	ZEND_PARSE_PARAMETERS_START(1,2)
	Z_PARAM_STR(colname)
	Z_PARAM_OPTIONAL
	Z_PARAM_BOOL(descend)
	ZEND_PARSE_PARAMETERS_END();

	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);
	cobj->orderBy(colname, descend);
}

//public function seqLastValue(string $seqname): ?int
ZEND_METHOD(Wcd_IBuild, seqLastValue)
{
	zend_string* sname;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(sname)
	ZEND_PARSE_PARAMETERS_END();

	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);
	
	zval_mgr result = cobj->seqLastValue(sname);
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

	zval_mgr result = cobj->setSeqValue(value, data);

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

	zval_mgr result = cobj->update(row, dirty);

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