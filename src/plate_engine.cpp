#ifndef PLATE_ENGINE_CPP
#define PLATE_ENGINE_CPP

#ifndef SEARCH_LIST_H
#include "search_list.h"
#endif

#ifndef PLATE_ENGINE_H
#include "plate_engine.h"
#endif


namespace wcc {
	Wcc_PlateEngine_Mgr plate_engine_mgr;

	const char* Wcc_PlateEngine::class_name = "Wcc\\PlateEngine";

	zend_class_entry* gIfLoadHtmlCE;
};

using namespace wcc;


void
Wcc_PlateEngine::debug_info(HashTable* ht)
{
	//zend_printf("debug info PlateEngine\n");
	htab_ptr d(ht);

	d.set(wis->mfind_leaf, finder_);
	d.set(wis->data_key, shared_data_);
	d.set(wis->plate_data, plates_data_);
	d.set(wis->extension, extensions_);
	d.set(wis->ready_key, made_);
	d.set(wis->load_html,  loadIntf_);
	d.set(wis->labelkey, doLabel_);

}

void Wcc_PlateEngine::__construct()
{
}

void Wcc_PlateEngine::setLoadHtml(zval_ptr obj)
{
	 loadIntf_ = obj.zobject();
}

zobj_own Wcc_PlateEngine::getLoadHtml()
{
	 return loadIntf_;
}

void Wcc_PlateEngine::setLabel(bool showLabel)
{
	doLabel_ = showLabel;
	//showobj("setLabel", this->zobj());
}


bool Wcc_PlateEngine::getLabel()
{
	return doLabel_;
}

void Wcc_PlateEngine::store(zend_string* name, zval_ptr plate)
{
	made_.set(name, plate.ptr());
}

void Wcc_PlateEngine::clearPlates()
{
	//showobj("clearPlates1", this->zobj());
	//* break circular references to self
	if (made_.size()) {
		htab_walk wk;
		auto& value = wk.value();
		for(wk.start(made_); wk.ok(); wk.next())
		{
			Wcc_Plate* pp = zval_toc<Wcc_Plate>(value);
			pp->clean();
			//showobj("plate pp", pp->zobj());
		}
	}
	made_.clear();
	//shared_data_.clear();
	//plates_data_.clear();
	//showobj("clearPlates2", this->zobj());
}

void Wcc_PlateEngine::shareWithAll(zval_ptr data)
{
	shared_data_.merge(data.zarray());
	//showobj("shareWithAll", this->zobj());
}

void Wcc_PlateEngine::setFinder(zval_ptr pathlist)
{
	finder_ = pathlist.zobject();
}

zval_own 
Wcc_PlateEngine::getFinder()
{
	//showobj("engine getPaths", this->zobj());
	return finder_;
}

zstr_own 
Wcc_PlateEngine::dumpPaths()
{
	if (finder_.isNull())
	{
		return zstr_own();
	}
	htab_own ptab = finder_.call("getpaths");
	size_t pcount = ptab.size();
	zstr_buffer dump;

	dump << "<p>Search in (" << (int)pcount << ") <br>\n";
	while(pcount)
	{
		--pcount;
		zstr_own pval = ptab.get(pcount);
		dump << pval << "<br>\n";
	}
	dump << "</p>\n";
	return dump.zstr();

}

// combine shared with override from plates_data
htab_own
Wcc_PlateEngine::getData(zend_string* name)
{
	htab_own all_data;
	
	all_data.merge(shared_data_);

	zval_ptr pdata = plates_data_[name];
	if (pdata.isArray())
	{
		all_data.merge(pdata);
	}
	return all_data;
}

void Wcc_PlateEngine::setExtensions(zval_ptr sval)
{
	extensions_ = sval.zarray();
}

zval_own
Wcc_PlateEngine::getExtensions()
{
	return zval_own(extensions_);
}

void
Wcc_PlateEngine::registerFunction(zend_string* name, zval_ptr callback)
{
	functions_.set(name,callback.ptr());
}

void
Wcc_PlateEngine::clearPaths()
{
	if (finder_.isNull())
		return;
	finder_.call("clear");
}

zstr_own 
Wcc_PlateEngine::find(zend_string* name)
{
	if (finder_.isNull())
	{
		return zstr_own();
	}

	zval_own arg1(name);
	zval_own arg2(extensions_); // needs a wrapper
	return  finder_.call(wis->mfind_leaf, arg1, arg2);
}

zval_own
Wcc_PlateEngine::getFunction(zend_string* name)
{
	return functions_[name];
}

zstr_own Wcc_PlateEngine::fileLabel(zend_string* file)
{
	zstr_buffer buf;

	buf << "<!--" << file << " -->\n";

	return buf;
}

zstr_own 
Wcc_PlateEngine::render(zend_string* name, htab_ptr data)
{
	// note request not stored, but maybe already stored
	zobj_own plate = make(name, false, nullptr);
	//show_obj("render by engine", plate);
	Wcc_Plate* p = zobj_toc<Wcc_Plate>(plate.ptr());
	return  p->render(data);
}

zobj_own
Wcc_PlateEngine::make(zend_string* name, bool store, zend_string* raw)
{
	
	zval_ptr test = made_[name];

	if (test.isObject()) {
		zend_object* pobj = test.zobject();
		Wcc_Plate* p = zobj_toc<Wcc_Plate>(pobj);
		p->setRaw(raw);
		//showobj("return stored object", pobj);
		return zobj_own(pobj);
	}
	

	Wcc_Plate* cobj = Wcc_Plate::make(this->zobj(), name);

	if (raw)
	{
		cobj->setRaw(raw);
	}
	// zobj_pass is an zobj_ptr - 
	zobj_own result = std::move(zobj_pass(cobj->zobj()));

	//showobj("make plate", first_wrap);

	if (store) {
		made_.set(name, result);
	}
	//showobj("return new object", first_wrap);
	return result;
}


void 
Wcc_PlateEngine::mergePlateData(zval_ptr data, zend_string* tname)
{
	zval_ptr tdata = plates_data_[tname];
	if (tdata.isNull())
	{
		plates_data_.set(tname, data.ptr());
	}
	else {
		htab_ptr ta(tdata);
		ta.merge(data);
	}
}

// templates should be  Array - list
void 
Wcc_PlateEngine::shareData(zval_ptr data, zval_ptr where)
{
	htab_own tarray;

	if (where.isNull())
	{
		shared_data_.merge(data);
		return;
	}
	if (where.isString())
	{
		tarray.push_back(where);
	}
	else if (where.isArray()){
		tarray = where.zarray();
	}

	htab_walk wk;

	auto& name = wk.value();
	for(wk.start(tarray); wk.ok(); wk.next())
	{
		mergePlateData(data, name.zstr());
	}
}

// static
ZEND_METHOD(Wcc_PlateEngine, fileComment)
{
	zend_string* file;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(file)
	ZEND_PARSE_PARAMETERS_END();

	zstr_own result = Wcc_PlateEngine::fileLabel(file);
	result.move_zv(return_value);
}


ZEND_METHOD(Wcc_PlateEngine, __construct)
{

	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_PlateEngine>(ZEND_THIS);

	cobj->__construct();
}


ZEND_METHOD(Wcc_PlateEngine, shareData)
{	
	zval* data;
	zval* where = nullptr;

	ZEND_PARSE_PARAMETERS_START(1, 2)
	Z_PARAM_ARRAY(data)
	Z_PARAM_OPTIONAL
	Z_PARAM_ZVAL(where)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_PlateEngine>(ZEND_THIS);

	cobj->shareData(data, where);
}


ZEND_METHOD(Wcc_PlateEngine, clearPaths)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_PlateEngine>(ZEND_THIS);

	cobj->clearPaths();
}




ZEND_METHOD(Wcc_PlateEngine, getData)
{
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_PlateEngine>(ZEND_THIS);

	htab_own data = cobj->getData(name);
	data.move_zv(return_value);
}




ZEND_METHOD(Wcc_PlateEngine, getExtensions)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_PlateEngine>(ZEND_THIS);
	zval_own ref = cobj->getExtensions();
	ref.move_zv(return_value);
}


ZEND_METHOD(Wcc_PlateEngine, getFinder)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_PlateEngine>(ZEND_THIS);
	zval_own ref = cobj->getFinder();

	ref.move_zv(return_value);
}


ZEND_METHOD(Wcc_PlateEngine, getLabel)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_PlateEngine>(ZEND_THIS);
	bool val = cobj->getLabel();
	RETURN_BOOL(val);
}


ZEND_METHOD(Wcc_PlateEngine, getLoadHtml)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_PlateEngine>(ZEND_THIS);
	zobj_own ref = cobj->getLoadHtml();
	ref.move_zv(return_value);
}


ZEND_METHOD(Wcc_PlateEngine, setLoadHtml)
{
	zval* obj;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_OBJECT_OF_CLASS(obj, gIfLoadHtmlCE);
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_PlateEngine>(ZEND_THIS);
	cobj->setLoadHtml(obj);
}


ZEND_METHOD(Wcc_PlateEngine, make)
{
	zend_string* name;
	bool         store = false;
	zend_string* raw = nullptr;

	ZEND_PARSE_PARAMETERS_START(3, 3)
	Z_PARAM_STR(name)
	Z_PARAM_BOOL(store)
	Z_PARAM_STR_OR_NULL(raw)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_PlateEngine>(ZEND_THIS);

	zobj_own ret = cobj->make(name, store, raw);

	//showobj("zobj_own make ", ret);
	ret.move_zv(return_value);
}


ZEND_METHOD(Wcc_PlateEngine, makeRaw)
{
	zend_string* name;
	zend_string* raw;
	bool         store = false;


	ZEND_PARSE_PARAMETERS_START(2, 3)
	Z_PARAM_STR(name)
	Z_PARAM_STR(raw)
	Z_PARAM_OPTIONAL
	Z_PARAM_BOOL(store)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_PlateEngine>(ZEND_THIS);

	zobj_own ret = cobj->makeRaw(name, raw, store);

	ret.move_zv(return_value);
}


ZEND_METHOD(Wcc_PlateEngine, mergePlateData)
{
	zval* data;
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(2, 2)
	Z_PARAM_ARRAY(data)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_PlateEngine>(ZEND_THIS);

	cobj->mergePlateData(data, name);
}


ZEND_METHOD(Wcc_PlateEngine, render)
{
	zend_string* name;
	zval*        data;

	ZEND_PARSE_PARAMETERS_START(2, 2)
	Z_PARAM_STR(name)
	Z_PARAM_ARRAY(data)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_PlateEngine>(ZEND_THIS);

	zstr_own ret = cobj->render(name,zval_ptr(data));
	ret.move_zv(return_value);
}


ZEND_METHOD(Wcc_PlateEngine, setExtensions)
{
	zval* ext;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ARRAY(ext)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_PlateEngine>(ZEND_THIS);

	cobj->setExtensions(ext);
}


ZEND_METHOD(Wcc_PlateEngine, setLabel)
{
	bool value = false;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_BOOL(value)
	ZEND_PARSE_PARAMETERS_END();
	
	auto cobj = zval_toc<Wcc_PlateEngine>(ZEND_THIS);

	cobj->setLabel(value);
}


ZEND_METHOD(Wcc_PlateEngine, setFinder)
{
	zval*        	   list;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_OBJECT_OF_CLASS(list, gIfFindLeafCE)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_PlateEngine>(ZEND_THIS);

	cobj->setFinder(list);
}


ZEND_METHOD(Wcc_PlateEngine, shareWithAll)
{
	zval*        data;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ARRAY(data)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_PlateEngine>(ZEND_THIS);

	cobj->shareWithAll(data);
}


ZEND_METHOD(Wcc_PlateEngine, store)
{
	zend_string* name;
	zval*        tp;
	zend_class_entry* ce = plate_mgr.classEntry();

	ZEND_PARSE_PARAMETERS_START(2, 2)
	Z_PARAM_STR(name)
	Z_PARAM_OBJECT_OF_CLASS(tp, ce)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_PlateEngine>(ZEND_THIS);
	cobj->store(name, tp);
}


ZEND_METHOD(Wcc_PlateEngine, clearPlates)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_PlateEngine>(ZEND_THIS);
	cobj->clearPlates();
}

PHP_MINIT_FUNCTION(Wcc_PlateEngine_reg)
{

	gIfLoadHtmlCE = register_class_Wcc_IfLoadHtml();

	auto ce = register_class_Wcc_PlateEngine();

	plate_engine_mgr.classEntry(ce);

	return SUCCESS;
}


//plate_engine.cpp
#endif