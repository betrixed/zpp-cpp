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
	PlateInit() : state_init(){}

	zstr_intern engine_key;
	zstr_intern name_key;
	zstr_intern data_key;
	zstr_intern raw_key;
	zstr_intern plate_key;

	zstr_intern layout_key;
	zstr_intern layout_data;
	zstr_intern style_level;
	zstr_intern sections;
	zstr_intern get_html;

	zstr_intern content_key;
	zstr_intern obgetclean_fn;
	zstr_intern obstart_fn;
	zstr_intern obgetlevel_fn;

	zstr_intern obendclean_fn;
	zstr_intern obgetcontents_fn;

	zstr_intern pushed_key;
	zstr_intern addstyle_fn;
	zstr_intern assets_key;

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


	}

	void init_req() override
	{
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


void Plate::debug_info(htab_write d)
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

void Plate::construct(zstr_user name, zobj_user engine)
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
zobj_mgr
Plate::make(zobj_user engine, zstr_user name)
{
	zobj_mgr result = Plate::omg.new_zobj();
	Plate* cobj = zobj_toc<Plate>(result);
	cobj->construct(engine, name);

	return result;
}
*/

void Plate::setData(htab_read data)
{
	//showmem("setData arg", data);
	data_ = data;
	//showarray("setData data", data_);
}

htab_read 
Plate::getData()
{
	return data_;
}


bool Plate::pathExists() {

	return !path_.isNull();
}


zstr_mgr 
Plate::escape(zstr_user s, zstr_user func) 
{
	zstr_mgr result;

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
zstr_user Plate::getPath()
{
	zstr_user result(path_);

	if (result.isNull())
	{
		PlateEngine *pe = zobj_toc<PlateEngine>(engine_);

		path_ = pe->find(name_);
		result = path_;
	}
	return result;
}


zstr_user
Plate::getName() 
{
	return name_;
}

htab_mgr
Plate::getPublish()
{

	//zend_printf("getPublish\n");
	htab_mgr result;
	htab_write publish(result);

	//showarrayy("publish 1", publish_);
	PlateEngine* pe = zobj_toc<PlateEngine>(engine_);

	htab_mgr plate_data = pe->getData(name_);
	publish.merge(plate_data);

	htab_read owndata(data_);
	if (owndata.size() > 0) {
		publish.merge(owndata);
	}
	publish.set(PLD.plate_key, this->vobj());
	return result;
}

void Plate::addSection(zstr_user name, zstr_user val) 
{
	htab_write(sections_).set(name,val);
}

zstr_user Plate::getSection(zstr_user name, zstr_user defaultval)
{
	zstr_user result = htab_read(sections_).get(name);
	if (result.isNull())
	{
		result = defaultval;
	}
	return result;
}


// sub-routine for internal
zstr_mgr
Plate::full_render(htab_read rdata)
{	
	zstr_mgr result;

	zstr_user name = getName();

	PlateEngine* engine = zobj_toc<PlateEngine>(engine_);
	
	htab_write wdata(data_);

	if (rdata.size())
	{
		wdata.merge(rdata);
	}
	zstr_user mypath = getPath();
	
	if (mypath.isNull())
	{
		zstr_buffer buf;
		buf << "<p>The template '" << name << "' could not be found</p>\n";
		buf << engine->dumpPaths();

		return buf.zstr();
	}

	zobj_user loadHtml = engine->getLoadHtml();

	if (loadHtml.isNull()) {
		zend_throw_error(zend_ce_error, "PlateEngine has no IfLoadHtml object");
		return result;
	}
	zval_mgr arg1 (this->getPath());
	zval_mgr arg2 (this->getPublish());
	
	result = loadHtml.call(PLD.get_html, arg1, arg2);

	return result;
}

void Plate::clean()
{
	// break pointer cycle
	engine_.init(); 
}

zstr_mgr Plate::render(htab_read data)
{
	//showstr("plate render for ", leaf_);

	zstr_mgr result;
	// showstr("raw_", raw_);

	if (raw_.size()) {
		result = raw_;
	}
	else {
		result = full_render(data);
	}

	zstr_user wrap(layout_);

	if (wrap.size()) {
		PlateEngine* pe = zobj_toc<PlateEngine>(engine_);

		zobj_mgr temp = pe->getPlate(wrap);
		zobj_user layout_obj(temp);
		
		if (layout_obj.ok())
		{
			Plate* layout = zobj_toc<Plate>(layout_obj);

			layout->addSection(PLD.content_key, result);

			result = layout->render(layoutData_);
		}
	}
	return result;
}

zstr_mgr 
Plate::insert(zstr_user name, htab_read data)
{
	PlateEngine* pe = zobj_toc<PlateEngine>(engine_);
	if (data.isNull())
	{
		data = data_;
	}
	return pe->render(name, data);
}

zstr_mgr Plate::fetch(zstr_user name, htab_read data)
{
	return insert(name, data);
}

zstr_mgr Plate::getContent()
{
	zval_user content = htab_read(sections_).get(PLD.content_key);
	if (content.isNull())
	{
		return zstr_temp("<pre>\n-- Missing Content--\n</pre>\n");
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

	zstr_mgr newContent = OBfn.obgetclean.call_fn();

	htab_write slabs(sections_);

	zstr_buffer buf;

	if (isPushed_) {

		zstr_user prevContent = slabs.get(sectionName_);

		if (prevContent.size())
		{
			buf << prevContent;
		}
		isPushed_ = false;
	}
	buf << newContent;
	zstr_mgr gather(std::move(buf));
	slabs.set(sectionName_,gather);
}

void Plate::push(zstr_user name)
{
	isPushed_ = true;
	start(name);
}

void Plate::start(zstr_user name)
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
	zval_mgr level = OBfn.obgetlevel.call_fn();
	return zval_user(level).zlong();
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
	zval_mgr styles = OBfn.obgetcontents.call_fn();
	OBfn.obendclean.call_fn();

	auto services = Services::cpp_global();

	zval_mgr assets_z = services->get(PLD.assets_key);
	zobj_user assets(assets_z);
	if (assets.ok()) {
		zobj_user(assets).call(PLD.addstyle_fn,  styles);
	}
}

void Plate::layout(zstr_user leaf, htab_read data)
{
	layout_ = leaf;

	layoutData_ = data;

}

void Plate::setLayout(zstr_user leaf)
{
	layout_ = leaf;
}

void Plate::setLayoutData(htab_read data)
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

	htab_mgr data = cobj->getPublish();
	//showarray("moved", data);
	data.move_zv(return_value);

	//showmem("getPublish", return_value);
}

ZEND_METHOD(Wcc_Plate, getData)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Plate>(ZEND_THIS);
	htab_read data = cobj->getData();
	//showarray("Plate getData", data);
	data.return_zv(return_value);
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
	zstr_mgr val = cobj->escape(s, func);

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
	zstr_mgr ret = cobj->fetch(name, data);
	ret.move_zv(return_value);
}


ZEND_METHOD(Wcc_Plate, getContent)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Plate>(ZEND_THIS);
	zstr_mgr ret = cobj->getContent();
	ret.move_zv(return_value);
}


ZEND_METHOD(Wcc_Plate, getPath)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Plate>(ZEND_THIS);
	zstr_user ret = cobj->getPath();
	ret.return_zv(return_value);
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

	zstr_mgr ret = cobj->insert(name, data);
	ret.move_zv(return_value);
}


ZEND_METHOD(Wcc_Plate, layout)
{
	zend_string* name = nullptr;
	zval* data = nullptr;

	zval_mgr nodata;

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

	zstr_mgr ret = cobj->render(data);

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

	zstr_user ret = cobj->getSection(name, ifnot);

	ret.return_zv(return_value);
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