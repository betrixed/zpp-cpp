#ifndef PLATE_WCC_CPP
#define PLATE_WCC_CPP

#ifndef PLATE_WCP_H
#include "plate_wcp.h"
#endif

#ifndef PLATE_ENGINE_H
#include "plate_engine.h"
#endif

/*#ifndef VIEW_OUTPUT_H
#include "view_output.h"
#endif
*/

#ifndef WCC_SERVICES_H
#include "services.h"
#endif

extern "C" {
	#include "ext/standard/html.h"
};

namespace wcc {
	const char* Wcc_Plate::class_name = "Wcc\\Plate";

	Wcc_Plate_Mgr plate_mgr;
};

using namespace wcc;


void Wcc_Plate::debug_info(HashTable* ht)
{

	ServiceAccess::debug_info(ht);


	htab_ptr d(ht);
	//zend_printf("debug info Plate\n");

	d.set(wis->engine_key, engine_);
	//zend_printf("debug info name_\n");
	//showobj("name_", name_);
	//showstr("namekey", wis->namekey);

	d.set(wis->namekey, leaf_);
	//zend_printf("debug info data_\n");
	
	d.set(wis->data_key, data_);
	//zend_printf("debug info sections_\n");
	d.set(wis->sections, sections_);
	//zend_printf("debug info layoutName_\n");
	d.set(wis->layout_leaf, layoutLeaf_);
	//zend_printf("debug info layoutData_\n");
	d.set(wis->layout_data, layoutData_);
	//zend_printf("debug info style_level_\n");
	d.set(wis->style_level, style_level_);
	//zend_printf("debug info pushedSection_\n");
	d.set(wis->pushed, pushedSection_);
	//zend_printf("debug info raw_\n");
	d.set(wis->raw_key, raw_);



}

void Wcc_Plate::construct(zval_ptr engine, zend_string* name)
{	

	ServiceAccess::construct(Wcc_Services::instance());
	
	engine_ = engine.zobject();
	
	
	leaf_ = name;
	//showstr("leaf_ name", leaf_);
	//showobj("this", this->zobj());

	pushedSection_ = false;
	zval_own zthis(this->zobj());

	this->setExtender(zthis.ptr());
}


Wcc_Plate* 
Wcc_Plate::make(zend_object* engine, zend_string* name)
{
	Wcc_Plate* cobj = plate_mgr.make_new();

	zval_own temp(engine);
	cobj->construct(zval_ptr(temp), name);
	return cobj;
}

void Wcc_Plate::setData(zval_ptr data)
{
	//showmem("setData arg", data);
	data_ = data.cow_array();
	//showarray("setData data", data_);
}

htab_own& Wcc_Plate::getData()
{
	return data_;
}


bool Wcc_Plate::pathExists() {

	return !path_.isNull();
}


zstr_own 
Wcc_Plate::escape(zend_string* s, zend_string* func) 
{
	int flags = ENT_QUOTES|ENT_SUBSTITUTE| ENT_HTML401;

	zend_string* ns = php_escape_html_entities_ex(
		(unsigned char*)ZSTR_VAL(s), ZSTR_LEN(s), /*all*/ 0, 
		flags, /*hint charset*/ nullptr, 
		/*double-encode*/ true, /*quiet*/ 0);
	return zstr_own(ns);

}

zstr_own& Wcc_Plate::getPath()
{
	if (path_.isNull())
	{
		path_ = engine()->find(leaf_.ptr());
	}
	return path_;
}


zstr_own& Wcc_Plate::getName() 
{
	//showstr("getName leaf_", leaf_);
	return leaf_;
}

htab_own
Wcc_Plate::getPublish()
{

	//zend_printf("getPublish\n");
	htab_own publish;


	//showarray("publish 1", publish_);

	if (!leaf_.isNull())
	{
		publish.merge(engine()->getData(leaf_));
	}


	//showarray("publish 2", publish_);
	if (data_.size() > 0) {
		publish.merge(data_);
	}

	//showarray("publish 3", publish_);
	publish.set(wis->plate_key, this->zobj());

	//showarray("publish 4", publish_);
	//showarray("publish_ return", publish_);

	return std::move(publish);
}

void Wcc_Plate::addSection(zend_string* name, zstr_own& val) {
	//showstr("addSection arg", val);
	sections_.set(name,val);
	//showstr("addSection added", val);
}

zstr_own Wcc_Plate::getSection(zstr_ptr name, zstr_ptr defaultval)
{
	zval_ptr result = sections_[name];
	if (result.isNull())
	{
		return defaultval;
	}
	return result;
}


// sub-routine for internal
zstr_own
Wcc_Plate::full_render(htab_ptr rdata)
{	
	//showstr("full_render", leaf_);

	zstr_own& name = getName();

	//showstr("getname", name);

	Wcc_PlateEngine* engine = this->engine();
	
	data_.clear();
	if (rdata.size())
	{
		data_.merge(rdata);
	}
	zstr_own& mypath = getPath();
	
	if (mypath.isNull())
	{
		zstr_buffer buf;
		buf << "<p>The template '" << name << "' could not be found</p>\n";
		buf << engine->dumpPaths();

		return std::move(buf);
	}

	zobj_own loadHtml = engine->getLoadHtml();

	if (loadHtml.isNull()) {
		throw Error("Engine object to implement IfLoadHtml");
	}
	zval_own arg1 (this->getPath());
	zval_own arg2 (this->getPublish());
	zstr_own result = loadHtml.call(wis->mget_html, arg1, arg2);

	return std::move(result);
}

void Wcc_Plate::clean()
{
	// break pointer cycle
	engine_.lose(); 
}

zstr_own Wcc_Plate::render(htab_ptr data)
{
	//showstr("plate render for ", leaf_);

	zstr_own result;
	// showstr("raw_", raw_);

	if (raw_.size()) {
		result = raw_;
	}
	else {
		result = full_render(data);
	}

	if (layoutLeaf_.size()) {
		zobj_own layout_obj = engine()->make(layoutLeaf_);
		//showobj("layout_obj", layout_obj);

		Wcc_Plate* layout = zobj_toc<Wcc_Plate>(layout_obj);

		layout->addSection(wis->content_key, result);

		zval_own ldata(layoutData_);

		result = layout->render(zval_ptr(ldata));
	}
	return result;
}

zstr_own Wcc_Plate::insert(zend_string* name, zval_ptr data)
{
	zval_own pass_data  = data;
	if (!pass_data.isArray())
	{
		pass_data.init_array();
		//showmem("pass_data insert", pass_data);
	}
	else {
		//showmem("passed to insert", pass_data);
	}
	return engine()->render(name, htab_ptr(pass_data.zarray()));
}

zstr_own Wcc_Plate::fetch(zend_string* name, zval_ptr data)
{
	return insert(name, data);
}

zstr_own Wcc_Plate::getContent()
{
	zval_ptr content = sections_[wis->content_key];
	if (content.isNull())
	{
		return std::string("<pre>\n-- Missing Content--\n</pre>\n");
	}
	return content;
}


void Wcc_Plate::stop()
{
	if (sectionName_.isNull()) 
	{
		throw Error("stop() section without start()");
	}
	zval_own newContent = wis->obgetclean_fn.callme();
	zstr_buffer buf;

	if (pushedSection_) {
		zval_ptr prevContent = sections_[sectionName_];
		zend_string* temp;

		if (prevContent.getStringData(&temp))
		{
			buf << temp;
		}
		pushedSection_ = false;
	}
	buf << newContent;
	zval_own val(std::move(buf));
	sections_.set(sectionName_,val);
}

void Wcc_Plate::push(zend_string* name)
{
	pushedSection_ = true;
	start(name);
}

void Wcc_Plate::start(zend_string* name)
{
	if (name == wis->content_key) {
		throw Error("Section name 'content' is reserved");
	}
	if (sectionName_.size()) {
		throw Error("Sections cannot be nested");
	}
	
	sectionName_ = name;
	wis->obstart_fn.callme();
}

int Wcc_Plate::getObLevel() {
	zval_own level = wis->obgetlevel_fn.callme();
	return level.zlong();
}

void Wcc_Plate::styleBegin()
{
	wis->obstart_fn.callme();
	style_level_ = getObLevel();
}

void Wcc_Plate::styleEnd()
{
	int levelDiff = getObLevel() - style_level_;
	if (levelDiff != 0)
	{
		throw Error("StyleEnd ob_level() changed");
	}
	zval_own styles = wis->obgetcontents_fn.callme();
	wis->obendclean_fn.callme();

	auto services = Wcc_Services::cpp_global();

	zval_own assets = services->get(wis->assets_key);

	if (assets.isObject()) {
		zobj_ptr am(assets);
		am.call(wis->addstyle_fn,  styles);
	}
}

void Wcc_Plate::layout(zend_string* leaf, zval_ptr data)
{
	layoutLeaf_ = leaf;
	if (data.isArray())
	{
		//showmem("layout data = ", data);
		layoutData_ = data;
		//showarray("layout data_ now", layoutData_);
	}
}

void Wcc_Plate::setLayoutLeaf(zend_string* leaf)
{
	layoutLeaf_ = leaf;
}

void Wcc_Plate::setLayoutData(zval_ptr data)
{
	layoutData_ = data;
}
	
ZEND_METHOD(Wcc_Plate, __construct)
{
	zval* engine = nullptr;
	zend_string* name = nullptr;
	zend_class_entry* ece = plate_engine_mgr.classEntry();

	ZEND_PARSE_PARAMETERS_START(2, 2)
	Z_PARAM_OBJECT_OF_CLASS(engine, ece)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_Plate>(ZEND_THIS);

	cobj->construct(engine, name);
}

ZEND_METHOD(Wcc_Plate, getPublish)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_Plate>(ZEND_THIS);
	//zend_printf("getPublish zend\n");

	htab_own data = cobj->getPublish();
	//showarray("moved", data);
	data.move_zv(return_value);

	//showmem("getPublish", return_value);
}

ZEND_METHOD(Wcc_Plate, getData)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_Plate>(ZEND_THIS);
	htab_own& data = cobj->getData();
	//showarray("Plate getData", data);
	data.return_zv(return_value);
}

ZEND_METHOD(Wcc_Plate, setData)
{
	zval* data;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ARRAY(data)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_Plate>(ZEND_THIS);
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

	auto cobj = zval_toc<Wcc_Plate>(ZEND_THIS);
	zstr_own val = cobj->escape(s, func);

	val.move_zv(return_value);
}


ZEND_METHOD(Wcc_Plate, pathExists)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_Plate>(ZEND_THIS);
	bool value = cobj->pathExists();
}


ZEND_METHOD(Wcc_Plate, fetch)
{

	zend_string* name = nullptr;
	zval* data = nullptr;
	zval_own   ifnot;

	ZEND_PARSE_PARAMETERS_START(1, 2)
	Z_PARAM_STR(name)
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY(data);
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_Plate>(ZEND_THIS);
	if (!data)
	{
		data = ifnot.ptr();
	}
	zstr_own ret = cobj->fetch(name, zval_ptr(data));
	ret.move_zv(return_value);
}


ZEND_METHOD(Wcc_Plate, getContent)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_Plate>(ZEND_THIS);
	zstr_own ret = cobj->getContent();
	ret.move_zv(return_value);
}


ZEND_METHOD(Wcc_Plate, getPath)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_Plate>(ZEND_THIS);
	zstr_own& ret = cobj->getPath();
	ret.return_zv(return_value);
}


ZEND_METHOD(Wcc_Plate, insert)
{
	zend_string* name = nullptr;
	zval* data = nullptr;
	zval_own    ifnot;

	ZEND_PARSE_PARAMETERS_START(1, 2)
	Z_PARAM_STR(name)
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY(data);
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_Plate>(ZEND_THIS);

	if (!data)
	{
		data = ifnot.ptr();
	}
	zstr_own ret = cobj->insert(name, zval_ptr(data));
	ret.move_zv(return_value);
}


ZEND_METHOD(Wcc_Plate, layout)
{
	zend_string* name = nullptr;
	zval* data = nullptr;

	zval_own nodata;

	ZEND_PARSE_PARAMETERS_START(1, 2)
	Z_PARAM_STR(name)
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY(data);
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_Plate>(ZEND_THIS);

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

	auto cobj = zval_toc<Wcc_Plate>(ZEND_THIS);
	cobj->push(s);
}


ZEND_METHOD(Wcc_Plate, stop)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_Plate>(ZEND_THIS);
	cobj->stop();
}


ZEND_METHOD(Wcc_Plate, render)
{
	zval* data = nullptr;
	zval_own   nodata;

	ZEND_PARSE_PARAMETERS_START(0, 1)
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY(data)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_Plate>(ZEND_THIS);

	if (!data)
	{
		nodata.init_array();
		data = nodata;
	}
	else {
		//showmem("call render", data);
	}

	zstr_own ret = cobj->render(zval_ptr(data));

	ret.move_zv(return_value);
}


ZEND_METHOD(Wcc_Plate, section)
{
	zend_string* name = nullptr;
	zend_string* ifnot = nullptr;
	
	ZEND_PARSE_PARAMETERS_START(1, 2)
	Z_PARAM_STR(name)
	Z_PARAM_OPTIONAL
	Z_PARAM_STR(ifnot);
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_Plate>(ZEND_THIS);

	//zend_printf("section call: ");
	//showstr("ifnot ", ifnot);

	zstr_own ret = cobj->getSection(name, ifnot);

	//showstr("section return", ret);
	ret.move_zv(return_value);
}


ZEND_METHOD(Wcc_Plate, setRaw)
{
	zend_string* s;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(s)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_Plate>(ZEND_THIS);
	cobj->setRaw(s);
}


ZEND_METHOD(Wcc_Plate, start)
{
	zend_string* s;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(s)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_Plate>(ZEND_THIS);
	cobj->start(s);
}


ZEND_METHOD(Wcc_Plate, styleBegin)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_Plate>(ZEND_THIS);
	cobj->styleBegin();
}


ZEND_METHOD(Wcc_Plate, styleEnd)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_Plate>(ZEND_THIS);
	cobj->styleEnd();
}

PHP_MINIT_FUNCTION(Wcc_Plate_reg)
{
	zend_class_entry* sa = service_access_mgr.classEntry();

	zend_class_entry* ce = register_class_Wcc_Plate(sa);

	plate_mgr.classEntry(ce);

	return SUCCESS;
}


//plate_wpp.cpp
#endif