#ifndef WCD_IBUILD_CPP
#define WCD_IBUILD_CPP

#ifndef WCD_IBUILD_H
#include "ibuild.h"
#endif

#ifndef SQL_PART_H
#include "sql_ipart.h"
#endif

namespace wcd {
using namespace zpp;


	void IBuild::construct(zval_user driver)
	{
		idriver_ = driver;
		params_ = ParamList::omg.new_zobj();
		ParamList* plist = zobj_toc<ParamList>(params_);

		plist->construct(driver);

		IDriver* db = zobj_toc<IDriver>(idriver_);
		isql_ = db->isql_;

		bindings_ = Bindings::omg.new_zobj();
		Bindings& bind = zobj_toc<Bindings>(bindings_);

		zval_mgr sqlmgr(isql_);

		bind->construct(sqlmgr, driver);
		bind->setParamList(params_);

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
		bind.add(ISql::SQL_RETURN, names);
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

		bool     is_multiple = false;

		if (rdata.isArray())
		{
			htab_read rows(rdata.zarray());

			int rowct = rows.size();
			if (rowct > 0)
			{
				row_mgr = rows.get(int(0));
			}
			else {
				result.set_bool(false);
				return result;
			}
			is_multiple = (rowct > 1);

		}
		else if (rdata.isObject())
		{
			row_mgr = rdata.zobject();
		}
		IRow* irow = zobj_toc<IRow>(row_mgr);
		zobj_mgr model_mgr = irow->getModel();

		Model* model = zobj_toc<Model>(model_mgr);

		if (model->hasTimeStamps()) {

		}
	}

	zval_mgr
	IBuild::aggregate(zstr_user agfn, htab_read columns)
	{
		model_.init();

		htab_mgr args_mgr;
		htab_write args(args_mgr);

		args.set(IBS.function_key, afgfn);
		args.set(IBS.columns_key, columns);

		Bindings* bind = zobj_toc<Bindings>(bindings_);
		bind->set(ISql::SQL_AGGREGATE, args_mgr);

		//ifetch_ = 

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

			htab_mgr ts = rowobj->stampTime(, UPDATE_TS);

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

			return RunSql::op(db_, sql, params);
			
		}
	}

	void 
	IBuild::table(zstr_user table, bool wipe=true)
	{
		Bindings& bind = bindings();

		if (wipe)
		{
			bind.wipe();
			ParamList* plist = zobj_toc<ParamList>(params_);
			plist.wipe();
		}

		bind.add(ISql::SQL_FROM, table);
	}

	int 
	IBuild::count(zval_user columns)
	{
		htab_mgr names;

		if (columns.isString())
		{
			htab_write hw(names);
			names.push_back(columns.zstr());
		}
		else columns.isArray() {
			names = columns.zarray();
			
		}
		zval_mgr result = aggregate(SQSTR.count_str, names);
		return result.zlong();
	}

	zval_mgr 
	IBuild::deleteRow(zobj_mgr rowobj)
	{
		Model* model = zobj_toc<Model>(model_);
		IRow*  row = zobj_toc<IRow>(rowobj);

		Bindings& bind = bindings();

	    htab_mgr pkey = model->getPKey();
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
	    		return;
	    	}
	    }
	    zobj_mgr params_mgr =  isql().delete(bind);
	    ParamList* plist = zobj_toc<ParamList>(params_mgr);
	    zstr_mgr sql = plist->getSql();
	    htab_mgr params = plist->getValues();

	    return RunSql::op(db_, sql, params);


	}

	void 
	IBuild::whereKeyValue(zval_user key, zval_user value)
	{
		Bindings& bind = bindings();
		bind.whereKeyValue(key, value);
	}



	void
	IBuild::where(zval_user column, zstr_user operator, zval_user value, zstr_user bval)
	{
		Bindings& bind = bindings();

		if (column.isString())
		{
			bind.where(column, operator, value, bval);
		}
		if (columns.isObject())
		{

		}
		zend_throw_error(zend_ce_error, "Where column type not supported");
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

	}
}; // namespace wcd

#endif