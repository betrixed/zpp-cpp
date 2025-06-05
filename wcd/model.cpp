#ifndef WCD_MODEL_CPP
#define WCD_MODEL_CPP

#ifndef WCD_MODEL_H
#include "model.h"
#endif


namespace wcd {

	zval_mgr Model::createFromResult(zstr_user classname, htab_read results)
	{
		if (results.size()==0)
		{
			return htab_mgr::empty_array();
		}

		zobj_mgr model = Services::getOne(classname);
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

		

	}


	zobj_mgr 
	Model::getBuilderForMe()
	{
		if (builder_me_.ok())
		{
			return builder_me_;
		}

		builder_me_ = 
	}

	zobj_mgr 
	Model::byKeyValue(htab_read keynames, htab_read values)
	{

	}

	zobj_mgr 
	Model::keyValue(zstr_user static_class, htab_read keynames, htab_read values)
	{
		zobj_mgr model = Services::getOne(static_class);
		Model* m = zobj_toc<Model>(model);
		return m->byKeyValue(keynames, values);
	}

}; // namespace wcd

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

#endif