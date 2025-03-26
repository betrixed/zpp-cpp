#ifndef HTML_PLATES_CPP
#define HTML_PLATES_CPP

#ifndef HTML_PLATES_H
#include "htmlplates.h"
#endif

#ifndef PLATE_ENGINE_H
#include "plate_engine.h"
#endif

#ifndef PLATE_WCP_H
#include "plate_wcp.h"
#endif

namespace wcc {

	HtmlPlates_Mgr htmlplates_mgr;

	const char* HtmlPlates::class_name = "Wcc\\HtmlPlates";
};


using namespace wcc;


void HtmlPlates::construct(zstr_ptr model_id)
{
	if (!model_id.isNull()) {
		model_svc_ = model_id;
	}
	else {
		model_svc_ = "view_model";
	}
	//showobj("htmlplates construct", this->zobj());
	services_ = Wcc_Services::instance();
}

void HtmlPlates::debug_info(HashTable *ht)
{
	htab_ptr di(ht);

	di.set(wis->valuekey, values_);
	di.set(wis->level_key, levels_);
	di.set("model_svc", model_svc_);
	di.set("model", model_);

}

void HtmlPlates::initValues()
{
	zval_own mtemp(getModel());
	values_.set("m", mtemp);
	values_.set(wis->view_key, this->zobj());
}

/**
 * Push from inner to outer
 */
void HtmlPlates::pushLevel(zstr_ptr name)
{
	levels_.push_back(name);
}

zobj_own HtmlPlates::getModel()
{
	if (model_.isNull()) {
		//showstr("getModel ", model_svc_);
		Wcc_Services* sv = svc_ptr();

		//showobj("Wcc_Services*", sv->zobj());

		model_ = sv->get( model_svc_);

		//showobj("model_", model_);

		if (model_.isNull())
		{
			model_ = zobj_own("Wcc\\Config");
		}
	}
	return  model_;
}

void HtmlPlates::setModel(zobj_ptr model)
{
	model_ = model;
	values_.set("m", model);
}

void HtmlPlates::mergeData(htab_ptr items)
{
	values_.merge(items);
}

zstr_own HtmlPlates::renderView(htab_ptr options) 
{
	options.set(wis->final_key, false);
	return render(options);
}

zstr_own HtmlPlates::render(htab_ptr options) 
{
	initValues();

	zval_ptr isfinal = options[wis->final_key];

	if (isfinal.isTrue()) {
		svc_ptr()->get(wis->final_key);
	}

	zval_ptr views = options["views"];

	if (!views.isNull())
	{
		if (views.isArray())
		{
			htab_walk hw;
			auto& val = hw.value();
			for(hw.start(views.zarray()); hw.ok(); hw.next())
			{
				levels_.push_back(val);
			}
		}
	}

	zobj_own engine = svc_ptr()->get(wis->engine_key);
	//showobj("render call", engine);
	Wcc_PlateEngine* pe = nullptr;
	if (!engine.isNull()) {
		 pe = zobj_toc<Wcc_PlateEngine>(engine.ptr());
		 pe->shareWithAll(zval_own(values_));
	}
	else {
		zend_throw_error(zend_ce_exception, "Plate engine not assigned\n");
	}

	htab_walk pw;
	auto& pname = pw.value();


	zval_ptr raw = options["raw"];
	zend_string* raw_val;
	bool hasData = raw.getStringData(&raw_val);
	if (!hasData)
	{
		raw_val = (zend_string*) nullptr;
	}
	int  ct = 0;

	zobj_own   inner_obj; // ownership stored here
	zobj_own   pobj;

	Wcc_Plate*  inner_ptr = (Wcc_Plate*) nullptr;
	Wcc_Plate*  prev_ptr = (Wcc_Plate*) nullptr;

	for (pw.start(levels_); pw.ok(); pw.next(), ct++)
	{
		zstr_ptr s = pname.zstr();

		//showstr("pname walk", s);
		if (!ct) {
			inner_obj = pe->make(s, false, raw_val);
			inner_ptr = zobj_toc<Wcc_Plate>(inner_obj);

			raw_val = (zend_string*) nullptr;
			prev_ptr = inner_ptr;
		}
		else {
			prev_ptr->setLayoutLeaf(s);
			// ownship stored in engine.
			//pobj.lose();
			pobj = pe->make(s, true, raw_val);
			prev_ptr = zobj_toc<Wcc_Plate>(pobj);
		}
		//showobj("inner", inner);
		//showobj("previous", previous);
	}
	
	zstr_own result = inner_ptr->render(htab_ptr());
	inner_ptr->clean();

	pobj.lose();
	inner_obj.lose();

	pe->clearPlates();
	

	// break services reference
	// svc_ptr()->unset(wis->engine_key);

	// inner was not stored in plateEngine
	

	// break circular reference to self
	values_.unset(wis->view_key); 

	//showobj("end render", this->zobj());
	//showobj("inner", inner);
	//showobj("previous", previous);
	return result;
}



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
	zobj_own result = cobj->getModel();
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

	cobj->setModel(zobj_ptr(object));
}

ZEND_METHOD(Wcc_HtmlPlates, mergeData)
{
	zval* data;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ARRAY(data)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlPlates>(ZEND_THIS);

	cobj->mergeData(htab_ptr(data));
}

ZEND_METHOD(Wcc_HtmlPlates, renderView)
{
	zval* options;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ARRAY(options)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<HtmlPlates>(ZEND_THIS);

	zstr_own result = cobj->renderView(htab_ptr(options));
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
	zstr_own result = cobj->render(htab_ptr(options));
	result.move_zv(return_value);
	
}


PHP_MINIT_FUNCTION(Wcc_HtmlPlates_reg)
{
	auto ce = register_class_Wcc_HtmlPlates();

	htmlplates_mgr.classEntry(ce);

	return SUCCESS;
}



//htmlplates.cpp
#endif