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

		void init() override {
			find_first = "findfirst";
			find_all = "findall";
			by_str = "by";
			and_str = "AND";
			eq_str = "=";
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
		zval_mgr keys_mgr(keynames);
		zval_mgr vals_mgr(values);

		bind.whereKeyValue(keys_mgr, vals_mgr);
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
		return 
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

PHP_MINIT_FUNCTION(Wcd_Model_reg)
{
	zintf_ce_IfCrud = register_class_Wcd_IfCrud();

	Model::omg.classEntry(register_class_Wcd_Model(zintf_ce_IfCrud));

	return SUCCESS;
}
#endif