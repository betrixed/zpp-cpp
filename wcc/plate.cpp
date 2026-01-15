#ifndef PLATE_WCC_CPP
#define PLATE_WCC_CPP

#ifndef PLATE_WCC_H
#include "plate.h"
#endif

#ifndef PLATE_ENGINE_H
#include "plate_engine.h"
#endif

/*#ifndef VIEW_OUTPUT_H
#include "view_output.h"
#endif
*/

extern "C" {
	#include "ext/standard/html.h"
};

namespace wcc 
{

base_obj_mgr<Plate> Plate::omg;

class OBtable {
public:
	fn_call obstart;
	fn_call obgetclean;
	fn_call obgetlevel;
	fn_call obendclean;
	fn_call obgetcontents;

	void init();
};

OBtable   OBfn;

class PlateInit : public state_init {
public:

	str_intern engine_key;
	str_intern name_key;
	str_intern data_key;
	str_intern raw_key;
	str_intern plate_key;

	str_intern layout_key;
	str_intern layout_data;
	str_intern style_level;
	str_intern sections;
	str_intern get_html;

	str_intern content_key;
	str_intern obgetclean_fn;
	str_intern obstart_fn;
	str_intern obgetlevel_fn;

	str_intern obendclean_fn;
	str_intern obgetcontents_fn;

	str_intern pushed_key;
	str_intern addstyle_fn;
	str_intern assets_key;

	void init() override
	{
		engine_key = "engine";
		name_key = "name";
		data_key = "data";
		raw_key = "raw";
		plate_key = "plate";

		layout_key = "layout";
		layout_data = "layout_data";
		style_level = "style_level";
		sections = "sections";
		get_html = "gethtml";

		content_key = "content";
		obgetclean_fn = "ob_get_clean";
		obstart_fn = "ob_start";
		obgetlevel_fn = "ob_get_level";
		obendclean_fn = "ob_end_clean";
		obgetcontents_fn = "ob_get_contents";

		pushed_key = "pushed";
		addstyle_fn = "addstyle";
		assets_key = "assets";

		OBfn.init();
	}

};

PlateInit PLD;


void
OBtable::init()
{
	obstart.set_fname(PLD.obstart_fn);
	obgetclean.set_fname(PLD.obgetclean_fn);
	obgetlevel.set_fname(PLD.obgetlevel_fn);
	obendclean.set_fname(PLD.obendclean_fn);
	obgetcontents.set_fname(PLD.obgetcontents_fn);
}


void Plate::debug_info(htab_rw d)
{

	//zend_printf("debug info Plate\n");

	d.set(PLD.engine_key, engine_);
	//zend_printf("debug info name_\n");
	//showobj("name_", name_);
	//showstr("namekey", PLD.namekey);

	d.set(PLD.name_key, name_);
	//zend_printf("debug info data_\n");
	
	d.set(PLD.data_key, data_);
	//zend_printf("debug info sections_\n");
	d.set(PLD.sections, sections_);
	//zend_printf("debug info layoutName_\n");
	d.set(PLD.layout_key, layout_);
	//zend_printf("debug info layoutData_\n");
	d.set(PLD.layout_data, layoutData_);
	//zend_printf("debug info style_level_\n");
	d.set(PLD.style_level, style_level_);
	//zend_printf("debug info pushedSection_\n");
	d.set(PLD.pushed_key, isPushed_);
	//zend_printf("debug info raw_\n");
	d.set(PLD.raw_key, raw_);



}

void Plate::construct(str_ptr name, obj_ptr engine)
{	

	//ServiceAccess::construct(Wcc_Services::instance());
	
	//zend_printf("Plate::construct\n");
	
	engine_ = engine;
	
	
	name_ = name;
	//showstr("leaf_ name", leaf_);
	//showobj("this", this->zobj());

	isPushed_ = false;
}

/*
obj_rc
Plate::make(obj_ptr engine, str_ptr name)
{
	obj_rc result = Plate::omg.new_zobj();
	Plate* cobj = zobj_toc<Plate>(result);
	cobj->construct(engine, name);

	return result;
}
*/

void Plate::setData(htab_ptr data)
{
	//showmem("setData arg", data);
	data_ = data;
	//showarray("setData data", data_);
}

htab_ptr 
Plate::getData()
{
	return data_;
}


bool Plate::pathExists() {

	return !path_.isNull();
}


str_rc 
Plate::escape(str_ptr s, str_ptr func) 
{
	str_rc result;

	if (!s.size())
	{
		return result;
	}

	int flags = ENT_QUOTES|ENT_SUBSTITUTE| ENT_HTML401;

	zend_string* data = s;

	zend_string* ns = php_escape_html_entities_ex(
		(unsigned char*)ZSTR_VAL(data), ZSTR_LEN(data), /*all*/ 0, 
		flags, /*hint charset*/ nullptr, 
		/*double-encode*/ true, /*quiet*/ 0);
	result.adopt(ns);
	return result;
}

//! return value is anchored by path_ member.
str_ptr Plate::getPath()
{
	str_ptr result(path_);

	if (result.isNull())
	{
		PlateEngine *pe = zobj_toc<PlateEngine>(engine_);

		path_ = pe->find(name_);
		result = path_;
	}
	return result;
}


str_ptr
Plate::getName() 
{
	return name_;
}

htab_rc
Plate::getPublish()
{

	//zend_printf("getPublish\n");
	htab_rc result;
	htab_rw publish(result);

	//showarrayy("publish 1", publish_);
	PlateEngine* pe = zobj_toc<PlateEngine>(engine_);

	htab_rc plate_data = pe->getData(name_);
	publish.merge(plate_data);

	htab_ptr owndata(data_);
	if (owndata.size() > 0) {
		publish.merge(owndata);
	}
	publish.set(PLD.plate_key, this->vobj());
	return result;
}

void Plate::addSection(str_ptr name, str_ptr val) 
{
	htab_rw(sections_).set(name,val);
}

str_ptr Plate::getSection(str_ptr name, str_ptr defaultval)
{
	str_ptr result = htab_ptr(sections_).get(name);
	if (result.isNull())
	{
		result = defaultval;
	}
	return result;
}


// sub-routine for internal
str_rc
Plate::full_render(htab_ptr rdata)
{	
	str_rc result;

	str_ptr name = getName();

	PlateEngine* engine = zobj_toc<PlateEngine>(engine_);
	
	htab_rw wdata(data_);

	if (rdata.size())
	{
		wdata.merge(rdata);
	}
	str_ptr mypath = getPath();
	
	if (mypath.isNull())
	{
		str_buf buf;
		buf << "<p>The template '" << name << "' could not be found</p>\n";
		buf << engine->dumpPaths();

		return buf.zstr();
	}

	obj_ptr loadHtml = engine->getLoadHtml();

	if (loadHtml.isNull()) {
		zend_throw_error(zend_ce_error, "PlateEngine has no IfLoadHtml object");
		return result;
	}
	val_rc arg1 (this->getPath());
	val_rc arg2 (this->getPublish());
	
	result = loadHtml.call(PLD.get_html, arg1, arg2);

	return result;
}

void Plate::clean()
{
	// break pointer cycle
	engine_.init(); 
}

str_rc Plate::render(htab_ptr data)
{
	//showstr("plate render for ", leaf_);

	str_rc result;
	// showstr("raw_", raw_);

	if (raw_.size()) {
		result = raw_;
	}
	else {
		result = full_render(data);
	}

	str_ptr wrap(layout_);

	if (wrap.size()) {
		PlateEngine* pe = zobj_toc<PlateEngine>(engine_);

		obj_rc temp = pe->getPlate(wrap);
		obj_ptr layout_obj(temp);
		
		if (layout_obj.ok())
		{
			Plate* layout = zobj_toc<Plate>(layout_obj);

			layout->addSection(PLD.content_key, result);

			result = layout->render(layoutData_);
		}
	}
	return result;
}

str_rc 
Plate::insert(str_ptr name, htab_ptr data)
{
	PlateEngine* pe = zobj_toc<PlateEngine>(engine_);
	if (data.isNull())
	{
		data = data_;
	}
	return pe->render(name, data);
}

str_rc Plate::fetch(str_ptr name, htab_ptr data)
{
	return insert(name, data);
}

str_rc Plate::getContent()
{
	val_ptr content = htab_ptr(sections_).get(PLD.content_key);
	if (content.isNull())
	{
		return str_temp("<pre>\n-- Missing Content--\n</pre>\n");
	}
	return content;
}


void Plate::stop()
{
	if (sectionName_.isNull()) 
	{
		zend_throw_error(zend_ce_error,"stop() section without start()");
		return;
	}

	str_rc newContent = OBfn.obgetclean.call_fn();

	htab_rw slabs(sections_);

	str_buf buf;

	if (isPushed_) {

		str_ptr prevContent = slabs.get(sectionName_);

		if (prevContent.size())
		{
			buf << prevContent;
		}
		isPushed_ = false;
	}
	buf << newContent;

	slabs.set(sectionName_, buf.zstr());
}

void Plate::push(str_ptr name)
{
	isPushed_ = true;
	start(name);
}

void Plate::start(str_ptr name)
{
	if (name == PLD.content_key) {
		zend_throw_error(zend_ce_error,"Section name 'content' is reserved");
		return;
		
	}
	if (sectionName_.size()) {
		zend_throw_error(zend_ce_error,"Sections cannot be nested");
		return;
	}
	
	sectionName_ = name;
	OBfn.obstart.call_fn();
}

int Plate::getObLevel() {
	val_rc level = OBfn.obgetlevel.call_fn();
	return val_ptr(level).zlong();
}

void Plate::styleBegin()
{
	OBfn.obstart.call_fn();
	style_level_ = getObLevel();
}

void Plate::styleEnd()
{
	int levelDiff = getObLevel() - style_level_;
	if (levelDiff != 0)
	{
		zend_throw_error(zend_ce_error,"StyleEnd ob_level() error");
		return;
	}
	val_rc styles = OBfn.obgetcontents.call_fn();
	OBfn.obendclean.call_fn();

	auto services = Services::cpp_global();

	val_rc assets_z = services->get(PLD.assets_key);
	obj_ptr assets(assets_z);
	if (assets.ok()) {
		obj_ptr(assets).call(PLD.addstyle_fn,  styles);
	}
}

void Plate::layout(str_ptr leaf, htab_ptr data)
{
	layout_ = leaf;

	layoutData_ = data;

}

void Plate::setLayout(str_ptr leaf)
{
	layout_ = leaf;
}

void Plate::setLayoutData(htab_ptr data)
{
	layoutData_ = data;
}
};//namespace %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

ZEND_METHOD(Wcc_Plate, __construct)
{
	zval* engine = nullptr;
	zend_string* name = nullptr;
	zend_class_entry* ece = PlateEngine::omg.classEntry();

	ZEND_PARSE_PARAMETERS_START(2, 2)
	Z_PARAM_STR(name)
	Z_PARAM_OBJECT_OF_CLASS(engine, ece)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Plate>(ZEND_THIS);

	cobj->construct(name, engine);
}

ZEND_METHOD(Wcc_Plate, getPublish)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Plate>(ZEND_THIS);
	//zend_printf("getPublish zend\n");

	htab_rc data = cobj->getPublish();
	//showarray("moved", data);
	data.move_zv(return_value);

	//showmem("getPublish", return_value);
}

ZEND_METHOD(Wcc_Plate, getData)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Plate>(ZEND_THIS);
	htab_ptr data = cobj->getData();
	//showarray("Plate getData", data);
	data.copy_zv(return_value);
}

ZEND_METHOD(Wcc_Plate, setData)
{
	zval* data;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ARRAY(data)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Plate>(ZEND_THIS);
	cobj->setData(data);
}


ZEND_METHOD(Wcc_Plate, escape)
{
	zend_string* s;
	zend_string* func = nullptr;

	ZEND_PARSE_PARAMETERS_START(1, 2)
	Z_PARAM_STR(s)
	Z_PARAM_OPTIONAL
	Z_PARAM_STR(func)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Plate>(ZEND_THIS);
	str_rc val = cobj->escape(s, func);

	val.move_zv(return_value);
}


ZEND_METHOD(Wcc_Plate, pathExists)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Plate>(ZEND_THIS);
	bool value = cobj->pathExists();
	RETURN_BOOL(value);
}


ZEND_METHOD(Wcc_Plate, fetch)
{

	zend_string* name = nullptr;
	zval* data = nullptr;

	ZEND_PARSE_PARAMETERS_START(1, 2)
	Z_PARAM_STR(name)
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY_OR_NULL(data);
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Plate>(ZEND_THIS);
	str_rc ret = cobj->fetch(name, data);
	ret.move_zv(return_value);
}


ZEND_METHOD(Wcc_Plate, getContent)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Plate>(ZEND_THIS);
	str_rc ret = cobj->getContent();
	ret.move_zv(return_value);
}


ZEND_METHOD(Wcc_Plate, getPath)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Plate>(ZEND_THIS);
	str_ptr ret = cobj->getPath();
	ret.copy_zv(return_value);
}


ZEND_METHOD(Wcc_Plate, insert)
{
	zend_string* name = nullptr;
	zval* data = nullptr;

	ZEND_PARSE_PARAMETERS_START(1, 2)
	Z_PARAM_STR(name)
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY_OR_NULL(data);
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Plate>(ZEND_THIS);

	str_rc ret = cobj->insert(name, data);
	ret.move_zv(return_value);
}


ZEND_METHOD(Wcc_Plate, layout)
{
	zend_string* name = nullptr;
	zval* data = nullptr;

	val_rc nodata;

	ZEND_PARSE_PARAMETERS_START(1, 2)
	Z_PARAM_STR(name)
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY(data);
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Plate>(ZEND_THIS);

	if (!data)
	{
		data = nodata;
	}
	cobj->layout(name, data);
}


ZEND_METHOD(Wcc_Plate, push)
{
	zend_string* s;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(s)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Plate>(ZEND_THIS);
	cobj->push(s);
}


ZEND_METHOD(Wcc_Plate, stop)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Plate>(ZEND_THIS);
	cobj->stop();
}


ZEND_METHOD(Wcc_Plate, render)
{
	zval* data = nullptr;

	ZEND_PARSE_PARAMETERS_START(0, 1)
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY_OR_NULL(data)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Plate>(ZEND_THIS);

	str_rc ret = cobj->render(data);

	ret.move_zv(return_value);
}


ZEND_METHOD(Wcc_Plate, section)
{
	zend_string* name = nullptr;
	zend_string* ifnot = nullptr;
	
	ZEND_PARSE_PARAMETERS_START(1, 2)
	Z_PARAM_STR(name)
	Z_PARAM_OPTIONAL
	Z_PARAM_STR_OR_NULL(ifnot);
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Plate>(ZEND_THIS);

	str_ptr ret = cobj->getSection(name, ifnot);

	ret.copy_zv(return_value);
}


ZEND_METHOD(Wcc_Plate, setRaw)
{
	zend_string* s;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(s)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Plate>(ZEND_THIS);
	cobj->setRaw(s);
}


ZEND_METHOD(Wcc_Plate, start)
{
	zend_string* s;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(s)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Plate>(ZEND_THIS);
	cobj->start(s);
}


ZEND_METHOD(Wcc_Plate, styleBegin)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Plate>(ZEND_THIS);
	cobj->styleBegin();
}


ZEND_METHOD(Wcc_Plate, styleEnd)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Plate>(ZEND_THIS);
	cobj->styleEnd();
}

PHP_MINIT_FUNCTION(Wcc_Plate_reg)
{
	zend_class_entry* ce = register_class_Wcc_Plate();

	Plate::omg.classEntry(ce);

	return SUCCESS;
}


//plate_wpp.cpp
#endif