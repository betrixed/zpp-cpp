#ifndef HTML_PLATES_CPP
#define HTML_PLATES_CPP

#ifndef HTML_PLATES_H
#include "htmlplates.h"
#endif

#ifndef PLATE_ENGINE_H
#include "plate_engine.h"
#endif

#ifndef PLATE_WCP_H
#include "plate.h"
#endif

namespace wcc {

using namespace zpp;
	
base_obj_mgr<HtmlPlates>	HtmlPlates::omg;

class HP_init : public state_init 
{
public:
	HP_init() : state_init() {}

	zstr_intern  view_model;
	zstr_intern  model_svc;
	zstr_intern  model_key;
	zstr_intern  model_var;

	zstr_intern  values_key;
	zstr_intern  level_key;
	zstr_intern  views_key;
	zstr_intern  view_key;
	zstr_intern  final_key;
	zstr_intern  engine_key;
	zstr_intern  raw_key;

	void init() override {
		view_model = "view_model";
		model_svc = "model_svc";
		model_key = "model";
		model_var = "m";
		values_key = "values";
		level_key = "level";
		views_key = "views";
		view_key = "view";
		final_key = "final";
		engine_key = "engine";
		raw_key = "raw";
	}
};

HP_init HPit;

void HtmlPlates::construct(zstr_user model_id)
{
	if (!model_id.isNull()) {
		model_svc_ = model_id;
	}
	else {
		model_svc_ = HPit.view_model;
	}
}

void HtmlPlates::debug_info(htab_write di)
{
	di.set(HPit.values_key, values_);
	di.set(HPit.level_key, levels_);
	di.set(HPit.model_svc, model_svc_);
	di.set(HPit.model_key, model_);

}

void HtmlPlates::initValues()
{
	zval_mgr mtemp(getModel());
	htab_write hw(values_);

	hw.set(HPit.model_var, mtemp);
	hw.set(HPit.view_key, this->vobj());
}

/**
 * Push from inner to outer
 */
void HtmlPlates::pushLevel(zstr_user name)
{
	htab_write(levels_).push_back(name);
}

zobj_mgr 
HtmlPlates::getModel()
{
	if (model_.isNull()) {

		model_ = Services::service( model_svc_);

		if (zobj_user(model_).isNull())
		{
			model_ = Config::make(htab_read());
		}
	}
	return  model_;
}

void HtmlPlates::setModel(zobj_user model)
{
	model_ = model;
	htab_write(values_).set(HPit.model_var, model);
}

void HtmlPlates::mergeData(htab_read items)
{
	htab_write(values_).merge(items);
}

zstr_mgr
HtmlPlates::renderView(htab_write options) 
{
	options.set(HPit.final_key, false);
	return render(options);
}

zstr_mgr HtmlPlates::render(htab_read options) 
{
	initValues();
	zstr_mgr result;

	zval_user isfinal = options.get(HPit.final_key);

	if (isfinal.isTrue()) {
		Services::service(HPit.final_key);
	}

	zval_user views = options.get(HPit.views_key);

	if (!views.isNull())
	{
		if (views.isArray())
		{
			for_key_value fkv;
			htab_write hw(levels_);
			for(fkv.start(views.zarray()); fkv.ok(); fkv.next())
			{
				hw.push_back(fkv.value());
			}
		}
	}

	zobj_mgr engine = Services::service(HPit.engine_key);
	//showobj("render call", engine);
	
	PlateEngine* pe = nullptr;
	if (!engine.isNull()) {
		 pe = zobj_toc<PlateEngine>(engine);
		 pe->shareWithAll(values_);
	}
	else {
		zend_throw_error(zend_ce_exception, "PlateEngine service required\n");
		return result;
	}

	htab_walk pw;
	auto pname = pw.value();

	zval_user raw = options.get(HPit.raw_key);

	int  ct = 0;

	zobj_mgr   inner_obj; // ownership stored here
	zobj_mgr   pobj;

	Plate*  inner_ptr = (Plate*) nullptr;
	Plate*  prev_ptr = (Plate*) nullptr;

	for (pw.start(levels_); pw.ok(); pw.next(), ct++)
	{
		zstr_user s = pname.zstr();

		if (!ct) {
			inner_obj = pe->newPlate(s, false);
			inner_ptr = zobj_toc<Plate>(inner_obj);
			if (raw.size())
			{
				inner_ptr->setRaw(raw);
			}
			prev_ptr = inner_ptr;
		}
		else {
			prev_ptr->setLayout(s);
			pobj = pe->newPlate(s, true);
			prev_ptr = zobj_toc<Plate>(pobj);
		}

	}
	
	result = inner_ptr->render(htab_read());

	pe->clearPlates();

	// break this circular reference to self
	htab_write(values_).unset(HPit.view_key); 

	return result;
}

}; // namespace wcc;

ZEND_METHOD(Wcc_HtmlPlates, __construct)
{
	zend_string* model_svc = nullptr;
	ZEND_PARSE_PARAMETERS_START(0, 1)
	Z_PARAM_OPTIONAL
	Z_PARAM_STR_OR_NULL(model_svc)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlPlates>(ZEND_THIS);
	cobj->construct(model_svc);
}

ZEND_METHOD(Wcc_HtmlPlates, getModel)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlPlates>(ZEND_THIS);
	zobj_mgr result = cobj->getModel();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_HtmlPlates, pushLevel)
{
	zend_string* name;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlPlates>(ZEND_THIS);
	cobj->pushLevel(name);
}

ZEND_METHOD(Wcc_HtmlPlates, setModel)
{
	zval* object;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_OBJECT(object)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlPlates>(ZEND_THIS);

	cobj->setModel(zobj_user(object));
}

ZEND_METHOD(Wcc_HtmlPlates, mergeData)
{
	zval* data;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ARRAY(data)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlPlates>(ZEND_THIS);

	cobj->mergeData(zval_user(data).zarray());
}

ZEND_METHOD(Wcc_HtmlPlates, renderView)
{
	zval* options;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ARRAY(options)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlPlates>(ZEND_THIS);

	zval_mgr options_copy(options);

	zstr_mgr result = cobj->renderView(zval_user(options_copy));
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_HtmlPlates, render)
{
	zval* options;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ARRAY(options)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlPlates>(ZEND_THIS);

	//showmem("render options", options);
	zstr_mgr result = cobj->render(zval_user(options).zarray());
	result.move_zv(return_value);
	
}


PHP_MINIT_FUNCTION(Wcc_HtmlPlates_reg)
{
	auto ce = register_class_Wcc_HtmlPlates();

	HtmlPlates::omg.classEntry(ce);

	return SUCCESS;
}



//htmlplates.cpp
#endif