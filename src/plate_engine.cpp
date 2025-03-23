#ifndef PLATE_ENGINE_CPP
#define PLATE_ENGINE_CPP

#ifndef SEARCH_LIST_H
#include "search_list.h"
#endif

#ifndef PLATE_ENGINE_H
#include "plate_engine.h"
#endif


namespace wcc {

base_obj_mgr<PlateEngine> PlateEngine::omg;

zend_class_entry* gIfLoadHtmlCE;

class PEng_init : public state_init {
public:
	PEng_init() : state_init() {}

	zstr_intern search_list;
	zstr_intern shared_data;
	zstr_intern plate_data;
	zstr_intern extensions;
	zstr_intern stored_key;
	zstr_intern load_html;
	zstr_intern label_str;
	zstr_intern getpaths;
	zstr_intern clear_key;
	zstr_intern find_leaf;

	virtual void init()
	{
		search_list = "search";
		shared_data = "shared_data";
		plate_data = "plate_data";
		extensions = "extensions";
		stored_key = "stored";
		load_html = "loadhtml";
		label_str = "dolabel";
		getpaths = "getpaths";
		clear_key = "clear";
		find_leaf = "findleaf";
	}
};

PEng_init PEI;

void
PlateEngine::debug_info(htab_write d)
{

	d.set(PEI.search_list, search_);
	d.set(PEI.shared_data, shared_data_);
	d.set(PEI.plate_data, plates_data_);
	d.set(PEI.extensions, extensions_);
	d.set(PEI.stored_key, stored_);
	d.set(PEI.load_html,  loadintf_);
	d.set(PEI.label_str, doLabel_);

}

void PlateEngine::setLoadHtml(zobj_user obj)
{
	 loadintf_ = obj;
}

zobj_user PlateEngine::getLoadHtml()
{
	 return loadintf_;
}

void PlateEngine::setLabel(bool showLabel)
{
	doLabel_ = showLabel;
	//showobj("setLabel", this->zobj());
}


bool PlateEngine::getLabel()
{
	return doLabel_;
}

void PlateEngine::store(zstr_user name, zobj_user plate)
{
	htab_write(stored_).set(name, plate);
}

void PlateEngine::clearPlates()
{
	//showobj("clearPlates1", this->zobj());
	//* break circular references to self
	htab_write map(stored_);

	if (map.size()) {
		htab_walk wk;
		auto value = wk.value();
		for(wk.start(map); wk.ok(); wk.next())
		{
			Plate* pp = zval_toc<Plate>(value);
			pp->clean();
			//showobj("plate pp", pp->zobj());
		}
	}
	stored_.init();
}

void PlateEngine::shareWithAll(htab_read data)
{
	htab_write(shared_data_).merge(data);
	//showobj("shareWithAll", this->zobj());
}

void PlateEngine::setFinder(zobj_user pathlist)
{
	search_ = pathlist;
}

zobj_user 
PlateEngine::getFinder()
{
	//showobj("engine getPaths", this->zobj());
	return search_;
}

zstr_mgr 
PlateEngine::dumpPaths()
{
	if (search_.isNull())
	{
		return zstr_mgr();
	}
	zval_mgr ptab = zobj_user(search_).call(PEI.getpaths);

	htab_read paths(ptab);

	size_t pcount = paths.size();
	zstr_buffer dump;

	dump << "<p>Search in (" << (int)pcount << ") <br>\n";
	while(pcount)
	{
		--pcount;
		zstr_user pval = paths.get(pcount);
		dump << pval << "<br>\n";
	}
	dump << "</p>\n";
	return dump.zstr();

}

// combine shared with override from plates_data
htab_mgr
PlateEngine::getData(zstr_user name)
{
	htab_mgr result;
	
	htab_write hw(result);

	hw.merge(shared_data_);

	htab_read pdata = htab_read(plates_data_).get(name);
	if (pdata.ok())
	{
		hw.merge(pdata);
	}
	return result;
}

void PlateEngine::setExtensions(zval_user sval)
{
	extensions_ = sval.zarray();
}

zval_mgr
PlateEngine::getExtensions()
{
	return zval_mgr(extensions_);
}

void
PlateEngine::registerFunction(zstr_user name, zval_user callback)
{
	htab_write(functions_).set(name, callback);
}

void
PlateEngine::clearPaths()
{
	if (search_.isNull())
		return;
	zobj_user(search_).call(PEI.clear_key);
}

zstr_mgr 
PlateEngine::find(zstr_user name)
{
	if (search_.isNull())
	{
		return zstr_mgr();
	}

	zval_mgr arg1(name);
	zval_mgr arg2(extensions_); // needs a wrapper
	return  zobj_user(search_).call(PEI.find_leaf, arg1, arg2);
}

zval_mgr
PlateEngine::getFunction(zstr_user name)
{
	zval_mgr result;
	htab_read hr(functions_);

	if (hr.ok())
	{
		result = hr.get(name);
	}
	return result;
}

zstr_mgr PlateEngine::fileLabel(zstr_user file)
{
	zstr_buffer buf;

	buf << "<!--" << file << " -->\n";

	return buf;
}

zstr_mgr 
PlateEngine::render(zstr_user name, htab_read data)
{
	zobj_mgr plate = getPlate(name);
	if (!plate.ok())
	{
		plate = newPlate(name, false);
	}
	Plate* p = zobj_toc<Plate>(plate);
	return  p->render(data);
}
zobj_mgr 
PlateEngine::newPlate(zstr_user name, bool store)
{
	zobj_mgr result;

	//zend_printf("newPlate fn\n");
	result = Plate::omg.new_zobj();

	zobj_user plate(result);
	//showobj("plate zobj", plate);

	fn_call_args<2> cplate;
	cplate.set_fci(plate, STAB.construct_key);
	zval* args = cplate.argsptr();
	ZVAL_STR(args, name);
	ZVAL_OBJ(args+1, this->vobj());
	
	cplate.call_fn();

	if (store)
	{
		htab_write hw(stored_);
		hw.set(name, plate);
	}
	return result;
}

zobj_mgr 
PlateEngine::getPlate(zstr_user name)
{
	zobj_mgr result;

	htab_read hw(stored_);

	zobj_user test = hw.get(name);

	if (test.ok()) {
		//showobj("return stored object", pobj);
		result = test;
		return result;
	}
	return result;
}

void PlateEngine::storePlate(zobj_user plate)
{
	htab_write hw(stored_);
	if (plate.ok())
	{
		Plate* p = zobj_toc<Plate>(plate);
		hw.set(p->getName(), plate);
	}
}

void 
PlateEngine::mergePlateData(htab_read data, zstr_user tname)
{
	htab_write hw(plates_data_);
	htab_write pdata = hw.get(tname);
	if (pdata.isNull())
	{
		hw.set(tname, data);
	}
	else {
		htab_write ta(pdata);
		ta.merge(data);
	}
}

// templates should be  Array - list of names, or name
void 
PlateEngine::shareData(htab_read data, zval_user where)
{
	if (where.isNull())
	{
		htab_write(shared_data_).merge(data);
		return;
	}

	htab_mgr tarray;

	if (where.isString())
	{
		htab_write temp(tarray);
		temp.push_back(where);
	}
	else if (where.isArray())
	{
		tarray = where.zarray();
	}
	else {
		return;
	}

	htab_walk wk;

	auto name = wk.value();
	for(wk.start(tarray); wk.ok(); wk.next())
	{
		mergePlateData(data, name.zstr());
	}
}

}; //namespace @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 
ZEND_METHOD(Wcc_PlateEngine, fileComment)
{
	zend_string* file;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(file)
	ZEND_PARSE_PARAMETERS_END();

	zstr_mgr result = PlateEngine::fileLabel(file);
	result.move_zv(return_value);
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

	auto cobj = zval_toc<PlateEngine>(ZEND_THIS);

	cobj->shareData(data, where);
}


ZEND_METHOD(Wcc_PlateEngine, clearPaths)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<PlateEngine>(ZEND_THIS);

	cobj->clearPaths();
}




ZEND_METHOD(Wcc_PlateEngine, getData)
{
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<PlateEngine>(ZEND_THIS);

	htab_mgr data = cobj->getData(name);
	data.move_zv(return_value);
}




ZEND_METHOD(Wcc_PlateEngine, getExtensions)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<PlateEngine>(ZEND_THIS);
	zval_mgr ref = cobj->getExtensions();
	ref.move_zv(return_value);
}


ZEND_METHOD(Wcc_PlateEngine, getFinder)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<PlateEngine>(ZEND_THIS);
	zobj_user ref = cobj->getFinder();

	ref.return_zv(return_value);
}


ZEND_METHOD(Wcc_PlateEngine, getLabel)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<PlateEngine>(ZEND_THIS);
	bool val = cobj->getLabel();
	RETURN_BOOL(val);
}


ZEND_METHOD(Wcc_PlateEngine, getLoadHtml)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<PlateEngine>(ZEND_THIS);
	zobj_user ref = cobj->getLoadHtml();
	ref.return_zv(return_value);
}


ZEND_METHOD(Wcc_PlateEngine, setLoadHtml)
{
	zval* obj;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_OBJECT_OF_CLASS(obj, gIfLoadHtmlCE);
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<PlateEngine>(ZEND_THIS);
	cobj->setLoadHtml(obj);
}


ZEND_METHOD(Wcc_PlateEngine, newPlate)
{
	zend_string* name;
	bool         store = false;

	ZEND_PARSE_PARAMETERS_START(1, 2)
	Z_PARAM_STR(name)
	Z_PARAM_OPTIONAL
	Z_PARAM_BOOL(store)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<PlateEngine>(ZEND_THIS);

	zobj_mgr ret = cobj->newPlate(name, store);

	//showobj("zobj_own make ", ret);
	ret.move_zv(return_value);
}


ZEND_METHOD(Wcc_PlateEngine, getPlate)
{
	zend_string* name;


	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<PlateEngine>(ZEND_THIS);

	zobj_user ret = cobj->getPlate(name);

	ret.return_zv(return_value);
}


ZEND_METHOD(Wcc_PlateEngine, mergePlateData)
{
	zval* data;
	zend_string* name;

	ZEND_PARSE_PARAMETERS_START(2, 2)
	Z_PARAM_ARRAY(data)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<PlateEngine>(ZEND_THIS);

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

	auto cobj = zval_toc<PlateEngine>(ZEND_THIS);

	zstr_mgr ret = cobj->render(name,zval_user(data));
	ret.move_zv(return_value);
}


ZEND_METHOD(Wcc_PlateEngine, setExtensions)
{
	zval* ext;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ARRAY(ext)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<PlateEngine>(ZEND_THIS);

	cobj->setExtensions(ext);
}


ZEND_METHOD(Wcc_PlateEngine, setLabel)
{
	bool value = false;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_BOOL(value)
	ZEND_PARSE_PARAMETERS_END();
	
	auto cobj = zval_toc<PlateEngine>(ZEND_THIS);

	cobj->setLabel(value);
}


ZEND_METHOD(Wcc_PlateEngine, setFinder)
{
	zval*        	   list;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_OBJECT_OF_CLASS(list, gIfFindLeafCE)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<PlateEngine>(ZEND_THIS);

	cobj->setFinder(list);
}


ZEND_METHOD(Wcc_PlateEngine, shareWithAll)
{
	zval*        data;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ARRAY(data)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<PlateEngine>(ZEND_THIS);

	cobj->shareWithAll(data);
}


ZEND_METHOD(Wcc_PlateEngine, storePlate)
{
	zend_class_entry* ce = Plate::omg.classEntry();
	zval* tp;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_OBJECT_OF_CLASS(tp, ce)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<PlateEngine>(ZEND_THIS);
	cobj->storePlate(tp);
}


ZEND_METHOD(Wcc_PlateEngine, clearPlates)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<PlateEngine>(ZEND_THIS);
	cobj->clearPlates();
}

PHP_MINIT_FUNCTION(Wcc_PlateEngine_reg)
{

	gIfLoadHtmlCE = register_class_Wcc_IfLoadHtml();

	auto ce = register_class_Wcc_PlateEngine();

	PlateEngine::omg.classEntry(ce);

	return SUCCESS;
}


//plate_engine.cpp
#endif