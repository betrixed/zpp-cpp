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

	zval_mgr Model::createFromResult(zstr_user classname, htab_read results)
	{
		if (results.size()==0)
		{
			return htab_mgr::empty_array();
		}

		zobj_user model = Services::getOne(classname);
		Model* m = zobj_toc<Model>(model);

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

	zobj_mgr 
	Model::keyValue(zstr_user static_class, zval_user keynames, zval_user values)
	{
		zobj_user model = Services::getOne(static_class);
		Model* m = zobj_toc<Model>(model);
		return m->byKeyValue(keynames, values);
	}

}; // namespace wcd

using namespace wcd;

ZEND_METHOD(Wcd_Model, KeyValue)
{
	zval* keynames;
	zval* values;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_ARRAY(keynames);
	Z_PARAM_ARRAY(values);
	ZEND_PARSE_PARAMETERS_END();

	zend_class_entry* static_class = zend_get_called_scope(execute_data);

	zobj_mgr result = Model::keyValue(static_class->name, keynames, values);

	result.move_zv(return_value);
}

PHP_MINIT_FUNCTION(Wcd_Model_reg)
{
	zintf_ce_IfCrud = register_class_Wcd_IfCrud();

	Model::omg.classEntry(register_class_Wcd_Model(zintf_ce_IfCrud));

	return SUCCESS;
}
#endif