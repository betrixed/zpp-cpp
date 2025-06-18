#ifndef WCD_MODEL_CPP
#define WCD_MODEL_CPP

#ifndef WCD_MODEL_H
#include "model.h"
#endif

#ifndef WCD_IDRIVER_H
#include "idriver.h"
#endif

#ifndef WCD_ISERVER_H
#include "iserver.h"
#endif

#ifndef WCD_IBUILD_H
#include "ibuild.h"
#endif

#ifndef SQL_ISQL_H
#include "sql_isql.h"
#endif

#ifndef DB_ARGINFO_H
#define DB_ARGINFO_H
extern "C" {
     #include "stub/db_arginfo.h"
}
#endif

#include <filesystem>

namespace wcd {

	zend_class_entry* zintf_ce_IfCrud;

	class Model_init : public state_init {
	public:
		Model_init() : state_init() {}

		zstr_intern find_first;
		zstr_intern find_all;
		zstr_intern by_str;
		zstr_intern and_str;
		zstr_intern eq_str;
		zstr_intern u_model;
		zstr_intern r_arg;
		zstr_intern escape_key;
		zstr_intern escape_str;

		void init() override {
			find_first = "findfirst";
			find_all = "findall";
			by_str = "by";
			and_str = "AND";
			eq_str = "=";
			u_model = "_model";
			r_arg = "r";
			escape_key = "escape";
			escape_str = "\\";

		}
	};

	Model_init MIS;

	zval_mgr 
	Model::createFromResult(zstr_user classname, htab_read results)
	{
		if (results.size()==0)
		{
			return htab_mgr::empty_array();
		}

		Model* m = model_instance(classname);

		zval_mgr rmgr;
		htab_write r(rmgr);

		htab_walk wk;

		auto result = wk.value();


		for(wk.start(results); wk.ok(); wk.next())
		{
			r.push_back(m->newRow(result.zarray(), true));
		}

		return rmgr;

	}

	zobj_mgr 
	Model::getConnect()
	{
		if (db_.ok())
		{
			return db_;
		}

		db_ = IServer::connect(zval_user());

		return db_;

	}


	zobj_mgr 
	Model::getBuilderForMe()
	{
		if (builder_me_.ok())
		{
			return builder_me_;
		}
		zobj_mgr db_mgr = getConnect();
		IDriver* db = zobj_toc<IDriver>(db_mgr);

		builder_me_ = db->newDmlBuild();
		if (builder_me_.ok())
		{
			IBuild* ib = zobj_toc<IBuild>(builder_me_);
			ib->setModel(this->vobj());
		}
		return builder_me_;
	}


	zobj_mgr  
	Model::byKeyValue(zval_user keynames, zval_user values)
	{
		zobj_mgr build = getBuilderForMe();
		IBuild* ib = zobj_toc<IBuild>(build);
		Bindings&  bind = ib->bindings();
		bind.limit(1);
		bind.whereKeyValue(keynames, values);
		ib->columns_.init();
		return bind.select(build);	
	}

	Model* //static
	Model::model_instance(zstr_user classname)
	{
		zobj_user model = Services::getOne(classname);
		Model* m = zobj_toc<Model>(model);

		return m;
	}

	zobj_mgr //static
	Model::keyValue(zstr_user static_class, zval_user keynames, zval_user values)
	{
		
		Model* m = Model::model_instance(static_class);
		return m->byKeyValue(keynames, values);
	}

	zobj_mgr 
	Model::withValues(zstr_user static_class, zval_user keyvalues)
	{
		Model* m = Model::model_instance(static_class);

		htab_mgr kv(keyvalues.zarray());

		htab_mgr keynames = htab_mgr::getKeys(kv);
		htab_mgr values = htab_mgr::getValues(kv);
		zval_mgr keynames_mgr(keynames);
		zval_mgr values_mgr(values);
		return m->byKeyValue(keynames_mgr, values_mgr);
	}

	zval_mgr 
	Model::callStatic(zstr_user static_name, zstr_user method, zval_user params)
	{
		htab_read  parray(params.zarray());

		Model* m = Model::model_instance(static_name);
		zobj_mgr build = m->getBuilderForMe();
		IBuild* ib = zobj_toc<IBuild>(build);

		zstr_mgr mlower = method.to_lower();
		zstr_user nullstr;
		zval_mgr  nullval;
		if (mlower.starts_with(MIS.find_first)) 
		{
			if (mlower.size()==9) 
			{
				
				

				ib->where(params, nullstr, nullval, MIS.and_str);
				return ib->oneRow();
			}
			else {
				zstr_mgr bystr = mlower.substr(9);
				if (bystr.starts_with(MIS.by_str))
				{
					zstr_mgr column = bystr.substr(2);

					zval_mgr value = parray.get((int)0);
					zval_mgr col_mgr(column);
					ib->where(col_mgr, MIS.eq_str, value, nullstr);
					return ib->oneRow();

				}
			}
		}
		else if (zs_cmp(mlower, MIS.find_all)==0)
		{
			if (parray.size())
			{
				ib->where(params, nullstr, nullval, MIS.and_str);
				return ib->allRows();
			}
			zval_mgr   result = ib->allRows();

			if (zval_user(result).isObject())
			{
				htab_mgr rows_mgr;
				htab_write rows(rows_mgr);

				rows.push_back(result);
				result = rows_mgr;
			}
			return result;
		}
		zval_mgr obj_method;
		htab_write arg1(obj_method);

		arg1.push_back(build);
		arg1.push_back(mlower);
		
		return FTAB.call_user_func_array.call(obj_method, params);
	}

	zstr_mgr //static
	Model::getTableName(zstr_user cname)
	{
		return Model::classToTableName(cname);
	}

	zstr_mgr 
	Model::classToTableName(zstr_user class_name)
	{
		zstr_mgr result(class_name);
		int ix = result.rfind('\\');

		if (ix >= 0) {
			result = result.substr(ix+1);
		}

		result = result.uncamel();

		zstr_mgr rlower = result.to_lower();

		ix = rlower.strpos(MIS.u_model);

		if (ix >= 0)
		{
			result = result.substr(0, ix);
		}
		return result;
	}

	zobj_mgr //static 
	Model::find(zstr_user static_name, zval_user id)
	{
		
		zobj_mgr result;

		Model* m = model_instance(static_name);
		htab_mgr pkey = m->getPKey();
		zval_mgr pkey_mgr(pkey);

		if (!pkey.size())
		{
			zend_throw_error(zend_ce_error,"Model without Primary key columns");
			return result;
		}

		if (id.isArray())
		{
			htab_read vlist(id.zarray());
			zval_user test = vlist.get((int)0);
			if (test.isNull()) {
				htab_mgr v2 = htab_mgr::sublist(pkey, vlist);
				zval_mgr arg2(v2);
				return m->byKeyValue(pkey_mgr, arg2);
			}
			return m->byKeyValue(pkey_mgr,id);
		}


		zval_mgr vlist_tab;
		htab_write vlist(vlist_tab);
		vlist.push_back(id);

		return m->byKeyValue(pkey_mgr, vlist_tab);
	}


	htab_mgr Model::getColDefs()
	{
		if (class_cdefs_.ok())
		{
			return class_cdefs_;
		}
	}

	zstr_mgr Model::getName()
	{
		if (name_.ok())
		{
			return name_;
		}

		name_ = Model::classToTableName(vobj()->ce->name);
		return name_;
	}

	htab_mgr 
	Model::getPKey()
	{
		if (class_pkey_.ok())
		{
			return class_pkey_;
		}
		class_pkey_ = htab_mgr::empty_array();
		return class_pkey_;
	}

	int // static
	Model::importFromCSV(zstr_user static_name, zstr_user filename)
	{
		if (! std::filesystem::exists(filename.vstr()) )
		{
			return 0;
		}

		int datarowct = -1;

		Model* m = model_instance(static_name);

		zstr_mgr tableName = m->getName();

		zobj_mgr db = m->getConnect();
		IDriver* driver = zobj_toc<IDriver>(db);

		htab_mgr columns = m->getColDefs();
		htab_mgr fieldNames;

		bool init = false;
		unsigned int  colcount = 0;

		zobj_mgr builder = m->getBuilderForMe();

		zval_mgr import_mgr = FTAB.fopen.call(filename, MIS.r_arg);

		zval_user import(import_mgr);

		if (import.isResource())
		{
			htab_mgr csv_args;
			htab_write csv(csv_args);
			csv.set(MIS.escape_key, MIS.escape_str);

			fn_fgetcsv fgetcsv;
			fgetcsv.set_named_args(csv_args);

			fn_stripslashes stripslashes;
			zval_mgr stmt;

			while(true)
			{
				zval_mgr line_mgr = fgetcsv.call(import);
				zval_user line(line_mgr); 
				zstr_mgr  cellstr;

				if (line.isFalse())
				{
					break;
				}

				htab_mgr values_mgr;

				htab_write values(line.zarray());
				htab_walk sw;

				auto item = sw.value();
				auto ix = sw.key();

				for(sw.start(line.zarray()); sw.ok(); sw.next())
				{
					cellstr = stripslashes.call(item);
					values.push_back(cellstr);
				}

				if (!init)
				{
					driver->begin();
					auto values_count = values.size();
					if (values_count) {
						colcount = 0;
						htab_walk wk;
						auto vname = wk.value();
						for(wk.start(values); wk.ok(); wk.next())
						{
							zval_user col = columns.get(vname);
							if (!col.isNull()) {
								colcount += 1;
							}
						}
						if (colcount == values_count) {
							fieldNames = values;
						}
					}
					if (! fieldNames.size())
					{
						fieldNames = columns.slice(0, values_count);
						datarowct = 0;

					}
					init = true;
				}
				if (datarowct == 0) {
					//  prepaire for multi inserts
					htab_mgr columns_mgr;
					htab_write columns(columns_mgr);

					for(sw.start(values); sw.ok(); sw.next())
					{
						zstr_mgr colname = fieldNames.get(ix);
						columns.set(colname, item);
					}
					IBuild* ib = zobj_toc<IBuild>(builder);

					zobj_mgr plist_mgr = ib->getInsertSql(columns_mgr);
					ParamList* plist = zobj_toc<ParamList>(plist_mgr);
					zstr_mgr sql = plist->getSql();
					htab_mgr record = plist->getValues();

					zval_mgr stmt = driver->prepare(sql);

					driver->bind(stmt, record);
				}
				else if (datarowct > 0) {
					driver->bind(stmt, values);
				}
				driver->execute(stmt, false, false);
				datarowct += 1;
			}

			driver->closeStmt(stmt);

			FTAB.fclose.call(import);

			htab_mgr seq_defs = m->getSeqDefs();

			if (seq_defs.size())
			{
				m->sequenceMax();
			}

			driver->commit();

		}
		return datarowct;
	}

	zobj_mgr 
	Model::getTableDef()
	{
		if (class_tdef_.ok())
		{
			return class_tdef_;
		}

		zobj_mgr driver = getConnect();

		IDriver* db = zobj_toc<IDriver>(driver);

		zobj_mgr schema = db->getSchema();
	}

}; // namespace wcd

using namespace wcd;

ZEND_METHOD(Wcd_Model, KeyValue)
{
	zval* keynames;
	zval* values;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_ARRAY(keynames)
	Z_PARAM_ARRAY(values)
	ZEND_PARSE_PARAMETERS_END();

	zend_class_entry* static_class = zend_get_called_scope(execute_data);

	zobj_mgr result = Model::keyValue(static_class->name, keynames, values);

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Model, WithValues)
{
	zval* values;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(values)
	ZEND_PARSE_PARAMETERS_END();

	zend_class_entry* static_class = zend_get_called_scope(execute_data);

	zobj_mgr result = Model::withValues(static_class->name, values);

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Model, __callStatic)
{
	zend_string* method;
	zval*         params;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_STR(method)
	Z_PARAM_ARRAY(params)
	ZEND_PARSE_PARAMETERS_END();

	zend_class_entry* static_class = zend_get_called_scope(execute_data);
	zval_mgr result = Model::callStatic(static_class->name, method, params);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcd_Model, classToTableName)
{
	zend_string* cname;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(cname)
	ZEND_PARSE_PARAMETERS_END();

	zstr_mgr result = Model::classToTableName(cname);

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Model, createFromResult)
{
	zend_string* cname;
	zval* results;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_STR(cname)
	Z_PARAM_ARRAY(results)
	ZEND_PARSE_PARAMETERS_END();

	zval_mgr result = Model::createFromResult(cname, results);

	result.move_zv(return_value);	
}

ZEND_METHOD(Wcd_Model, find)
{
	zval* values;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(values)
	ZEND_PARSE_PARAMETERS_END();

	zend_class_entry* static_class = zend_get_called_scope(execute_data);

	zobj_mgr result = Model::find(static_class->name, values);

	result.move_zv(return_value);	

}

ZEND_METHOD(Wcd_Model, getTableName)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_class_entry* static_class = zend_get_called_scope(execute_data);

	zstr_mgr result = Model::getTableName(static_class->name);
	result.move_zv(return_value);	
}

PHP_MINIT_FUNCTION(Wcd_Model_reg)
{
	zintf_ce_IfCrud = register_class_Wcd_IfCrud();

	Model::omg.classEntry(register_class_Wcd_Model(zintf_ce_IfCrud));

	return SUCCESS;
}
#endif