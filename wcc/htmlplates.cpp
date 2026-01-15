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

	str_intern  view_model;
	str_intern  model_svc;
	str_intern  model_key;
	str_intern  model_var;

	str_intern  values_key;
	str_intern  level_key;
	str_intern  views_key;
	str_intern  view_key;
	str_intern  final_key;
	str_intern  engine_key;
	str_intern  raw_key;

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

void HtmlPlates::construct(str_ptr model_id)
{
	if (!model_id.isNull()) {
		model_svc_ = model_id;
	}
	else {
		model_svc_ = HPit.view_model;
	}
}

void HtmlPlates::debug_info(htab_rw di)
{
	di.set(HPit.values_key, values_);
	di.set(HPit.level_key, levels_);
	di.set(HPit.model_svc, model_svc_);
	di.set(HPit.model_key, model_);

}

void HtmlPlates::initValues()
{
	val_rc mtemp(getModel());
	htab_rw hw(values_);

	hw.set(HPit.model_var, mtemp);
	hw.set(HPit.view_key, this->vobj());
}

/**
 * Push from inner to outer
 */
void HtmlPlates::pushLevel(str_ptr name)
{
	htab_rw(levels_).push_back(name);
}

obj_rc 
HtmlPlates::getModel()
{
	if (model_.isNull()) {

		model_ = Services::service( model_svc_);

		if (obj_ptr(model_).isNull())
		{
			model_ = Config::make(htab_ptr());
		}
	}
	return  model_;
}

void HtmlPlates::setModel(obj_ptr model)
{
	model_ = model;
	htab_rw(values_).set(HPit.model_var, model);
}

void HtmlPlates::mergeData(htab_ptr items)
{
	htab_rw(values_).merge(items);
}

str_rc
HtmlPlates::renderView(htab_rw options) 
{
	options.set(HPit.final_key, false);
	return render(options);
}

str_rc HtmlPlates::render(htab_ptr options) 
{
	initValues();
	str_rc result;

	val_ptr isfinal = options.get(HPit.final_key);

	if (isfinal.isTrue()) {
		Services::service(HPit.final_key);
	}

	val_ptr views = options.get(HPit.views_key);

	if (!views.isNull())
	{
		if (views.isArray())
		{
			for_key_value fkv;
			htab_rw hw(levels_);
			for(fkv.start(views.zarray()); fkv.ok(); fkv.next())
			{
				hw.push_back(fkv.value());
			}
		}
	}

	obj_rc engine = Services::service(HPit.engine_key);
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

	val_ptr raw = options.get(HPit.raw_key);

	int  ct = 0;

	obj_rc   inner_obj; // ownership stored here
	obj_rc   pobj;

	Plate*  inner_ptr = (Plate*) nullptr;
	Plate*  prev_ptr = (Plate*) nullptr;

	for (pw.start(levels_); pw.ok(); pw.next(), ct++)
	{
		str_ptr s = pname.zstr();

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
	
	result = inner_ptr->render(htab_ptr());

	pe->clearPlates();

	// break this circular reference to self
	htab_rw(values_).unset(HPit.view_key); 

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
	obj_rc result = cobj->getModel();
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

	cobj->setModel(obj_ptr(object));
}

ZEND_METHOD(Wcc_HtmlPlates, mergeData)
{
	zval* data;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ARRAY(data)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlPlates>(ZEND_THIS);

	cobj->mergeData(val_ptr(data).zarray());
}

ZEND_METHOD(Wcc_HtmlPlates, renderView)
{
	zval* options;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ARRAY(options)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlPlates>(ZEND_THIS);

	val_rc options_copy(options);

	str_rc result = cobj->renderView(val_ptr(options_copy));
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
	str_rc result = cobj->render(val_ptr(options).zarray());
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