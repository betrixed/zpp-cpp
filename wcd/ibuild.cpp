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
		zval_mgr list(names);
		bind.add(ISql::SQL_RETURN, list);
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
				bind.add(ISql::SQL_INSERT, temp);
			}
		}
		else {
			// already stamped time.
			temp = irow->getData();
			bind.add(ISql::SQL_INSERT, temp);
		}

		zobj_mgr plist_mgr = isql().insert(bind);

		bind.wipe(ISql::SQL_INSERT);

		if (plist_mgr.ok())
		{
			ParamList* plist = zobj_toc<ParamList>(plist_mgr);
			const zstr_mgr& sql = plist->getSql();
			const htab_mgr& values = plist->getValues();
			const htab_mgr& rets = plist->getReturns();

			IDriver& db = idb();

			int fetch = db.setFetch(IDriver::FETCH_ASSOC);
			result = RunSql::op(driver_, sql, values, rets);
			db.setFetch(fetch);
		}
		return  result;
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


		columns_.init();
		IDriver& db = idb();
		int fetch = db.setFetch(IDriver::FETCH_OBJECT);
		zval_mgr result = bind.select(vobj());
		if (fetch != IDriver::FETCH_OBJECT)
			db.setFetch(fetch);
		return result;
	}

	zval_mgr 
	IBuild::update(zobj_user irow, htab_read dirty)
	{
		Bindings& bind = bindings();

		IRow* rowobj = zobj_toc<IRow>(irow);

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

			return RunSql::op(driver_, sql, params);
			
		}
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

		zval_mgr temp(table);
		bind.add(ISql::SQL_FROM, temp);
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
		bind.add(ISql::SQL_INSERT, columns);

		ISql* isql = zobj_toc<ISql>(isql_);

		return isql->insert(bind);

	}


	zval_mgr 
	IBuild::oneRow()
	{

	}

	zval_mgr 
	IBuild::allRows()
	{

	}

	zval_mgr 
	IBuild::first(htab_read columns)
	{

	}

	zval_mgr
	IBuild::get(htab_read columns)
	{
		columns_ = columns;
		Bindings& bind = bindings();
		return bind.select(vobj());
	}
}; // namespace wcd



using namespace wcd;

ZEND_METHOD(Wcd_IBuild, __construct)
{
	zval* dbobj;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT_OF_CLASS(zval, IDriver::omg.classEntry())
	ZEND_PARSE_PARAMETERS_END();

	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);

	cobj->construct(dbobj);
}

ZEND_METHOD(Wcd_IBuild, __destruct)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);
}

ZEND_METHOD(Wcd_IBuild, aggregate)
{
	zend_string* func;
	zval*        columns;

	ZEND_PARSE_PARAMETERS_START(1,2)
	Z_PARAM_STR(func)
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY_OR_NULL(columns)
	ZEND_PARSE_PARAMETERS_END();

	IBuild* cobj = zval_toc<IBuild>(ZEND_THIS);

	zval_mgr result = cobj->aggregate(func, columns);

	result.move_zv(return_value);
}

/*

;
ZEND_METHOD(Wcd_IBuild, aggregate);
ZEND_METHOD(Wcd_IBuild, allRows);
ZEND_METHOD(Wcd_IBuild, avg);
ZEND_METHOD(Wcd_IBuild, count);
ZEND_METHOD(Wcd_IBuild, deleteRow);
ZEND_METHOD(Wcd_IBuild, distinct);
ZEND_METHOD(Wcd_IBuild, first);
ZEND_METHOD(Wcd_IBuild, get);
ZEND_METHOD(Wcd_IBuild, getDriver);
ZEND_METHOD(Wcd_IBuild, getBindings);
ZEND_METHOD(Wcd_IBuild, getFrom);
ZEND_METHOD(Wcd_IBuild, getInsertSql);
ZEND_METHOD(Wcd_IBuild, getParamList);
ZEND_METHOD(Wcd_IBuild, getSql);
ZEND_METHOD(Wcd_IBuild, hasModel);
ZEND_METHOD(Wcd_IBuild, insert);
ZEND_METHOD(Wcd_IBuild, limit);
ZEND_METHOD(Wcd_IBuild, now);
ZEND_METHOD(Wcd_IBuild, offset);
ZEND_METHOD(Wcd_IBuild, oneRow);
ZEND_METHOD(Wcd_IBuild, orderBy);
ZEND_METHOD(Wcd_IBuild, seqLastValue);
ZEND_METHOD(Wcd_IBuild, set);
ZEND_METHOD(Wcd_IBuild, setFetch);
ZEND_METHOD(Wcd_IBuild, setInsert);
ZEND_METHOD(Wcd_IBuild, setModel);
ZEND_METHOD(Wcd_IBuild, setModelClass);
ZEND_METHOD(Wcd_IBuild, setReturns);
ZEND_METHOD(Wcd_IBuild, setSeqValue);
ZEND_METHOD(Wcd_IBuild, table);
ZEND_METHOD(Wcd_IBuild, update);
ZEND_METHOD(Wcd_IBuild, where);
ZEND_METHOD(Wcd_IBuild, wipe);
*/

PHP_MINIT_FUNCTION(Wcd_IBuild_reg)
{
	IBuild::omg.classEntry(register_class_Wcd_IBuild());
}

#endif