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

	str_intern search_list;
	str_intern shared_data;
	str_intern plate_data;
	str_intern extensions;
	str_intern stored_key;
	str_intern load_html;
	str_intern label_str;
	str_intern getpaths;
	str_intern clear_key;
	str_intern find_leaf;

	virtual void init()
	{
		search_list = "search";
		shared_data = "shared_data";
		plate_data = "plate_data";
		extensions = "extensions";
		stored_key = "stored";
		load_html = "loadhtml";
		getpaths = "getpaths";
		clear_key = "clear";
		find_leaf = "findleaf";
	}
};

PEng_init PEI;

void
PlateEngine::debug_info(htab_rw d)
{

	d.set(PEI.search_list, search_);
	d.set(PEI.shared_data, shared_data_);
	d.set(PEI.plate_data, plates_data_);
	d.set(PEI.extensions, extensions_);
	d.set(PEI.stored_key, stored_);
	d.set(PEI.load_html,  loadintf_);

}

void PlateEngine::setLoadHtml(obj_ptr obj)
{
	 loadintf_ = obj;
}

obj_ptr PlateEngine::getLoadHtml()
{
	 return loadintf_;
}



void PlateEngine::store(str_ptr name, obj_ptr plate)
{
	htab_rw(stored_).set(name, plate);
}

void PlateEngine::clearPlates()
{
	//showobj("clearPlates1", this->zobj());
	//* break circular references to self
	htab_rw map(stored_);

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

void PlateEngine::shareWithAll(htab_ptr data)
{
	htab_rw(shared_data_).merge(data);
	//showobj("shareWithAll", this->zobj());
}

void PlateEngine::setFinder(obj_ptr pathlist)
{
	search_ = pathlist;
}

obj_ptr 
PlateEngine::getFinder()
{
	//showobj("engine getPaths", this->zobj());
	return search_;
}

str_rc 
PlateEngine::dumpPaths()
{
	if (search_.isNull())
	{
		return str_rc();
	}
	val_rc ptab = obj_ptr(search_).call(PEI.getpaths);

	htab_ptr paths(ptab);

	size_t pcount = paths.size();
	str_buf dump;

	dump << "<p>Search in (" << (int)pcount << ") <br>\n";
	while(pcount)
	{
		--pcount;
		str_ptr pval = paths.get(pcount);
		dump << pval << "<br>\n";
	}
	dump << "</p>\n";
	return dump.zstr();

}

// combine shared with override from plates_data
htab_rc
PlateEngine::getData(str_ptr name)
{
	htab_rc result;
	
	htab_rw hw(result);

	hw.merge(shared_data_);

	htab_ptr pdata = htab_ptr(plates_data_).get(name);
	if (pdata.ok())
	{
		hw.merge(pdata);
	}
	return result;
}

void PlateEngine::setExtensions(val_ptr sval)
{
	extensions_ = sval.zarray();
}

val_rc
PlateEngine::getExtensions()
{
	return val_rc(extensions_);
}

void
PlateEngine::registerFunction(str_ptr name, val_ptr callback)
{
	htab_rw(functions_).set(name, callback);
}

void
PlateEngine::clearPaths()
{
	if (search_.isNull())
		return;
	obj_ptr(search_).call(PEI.clear_key);
}

str_rc 
PlateEngine::find(str_ptr name)
{
	if (search_.isNull())
	{
		return str_rc();
	}

	val_rc arg1(name);
	val_rc arg2(extensions_); // needs a wrapper
	return  obj_ptr(search_).call(PEI.find_leaf, arg1, arg2);
}

val_rc
PlateEngine::getFunction(str_ptr name)
{
	val_rc result;
	htab_ptr hr(functions_);

	if (hr.ok())
	{
		result = hr.get(name);
	}
	return result;
}

str_rc 
PlateEngine::render(str_ptr name, htab_ptr data)
{
	obj_rc plate = getPlate(name);
	if (!plate.ok())
	{
		plate = newPlate(name, false);
	}
	Plate* p = zobj_toc<Plate>(plate);
	return  p->render(data);
}
obj_rc 
PlateEngine::newPlate(str_ptr name, bool store)
{
	obj_rc result;

	//zend_printf("newPlate fn\n");
	result = Plate::omg.new_zobj();

	//showobj("plate zobj", plate);

	fn_call np(STAB.construct_key, result);
	fn_params<2> cplate(np);

	zval* args = cplate.argsptr();
	val_ptr::string_bind(args, name);
	val_ptr::object_bind(args+1, this->vobj());
	
	cplate.call_fn();

	if (store)
	{
		htab_rw hw(stored_);
		hw.set(name, result);
	}
	return result;
}

obj_rc 
PlateEngine::getPlate(str_ptr name)
{
	obj_rc result;

	htab_ptr hw(stored_);

	obj_ptr test = hw.get(name);

	if (test.ok()) {
		//showobj("return stored object", pobj);
		result = test;
		return result;
	}
	return result;
}

void PlateEngine::storePlate(obj_ptr plate)
{
	htab_rw hw(stored_);
	if (plate.ok())
	{
		Plate* p = zobj_toc<Plate>(plate);
		hw.set(p->getName(), plate);
	}
}

void 
PlateEngine::mergePlateData(htab_ptr data, str_ptr tname)
{
	htab_rw hw(plates_data_);
	htab_rw pdata = hw.get(tname);
	if (pdata.isNull())
	{
		hw.set(tname, data);
	}
	else {
		htab_rw ta(pdata);
		ta.merge(data);
	}
}

// templates should be  Array - list of names, or name
void 
PlateEngine::shareData(htab_ptr data, val_ptr where)
{
	if (where.isNull())
	{
		htab_rw(shared_data_).merge(data);
		return;
	}

	htab_rc tarray;

	if (where.isString())
	{
		htab_rw temp(tarray);
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

	htab_rc data = cobj->getData(name);
	data.move_zv(return_value);
}




ZEND_METHOD(Wcc_PlateEngine, getExtensions)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<PlateEngine>(ZEND_THIS);
	val_rc ref = cobj->getExtensions();
	ref.move_zv(return_value);
}


ZEND_METHOD(Wcc_PlateEngine, getFinder)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<PlateEngine>(ZEND_THIS);
	obj_ptr ref = cobj->getFinder();

	ref.copy_zv(return_value);
}



ZEND_METHOD(Wcc_PlateEngine, getLoadHtml)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<PlateEngine>(ZEND_THIS);
	obj_ptr ref = cobj->getLoadHtml();
	ref.copy_zv(return_value);
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

	obj_rc ret = cobj->newPlate(name, store);

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

	obj_ptr ret = cobj->getPlate(name);

	ret.copy_zv(return_value);
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
	zarg_rd args(execute_data);

	str_ptr name = args.str(args.need(0));
	htab_ptr data;

	if (!args.zarray_null(data, args.option(1)))
	{
		data = htab_ptr::empty_array();
	}

	if (!args.throw_errors())
	{
		auto cobj = zval_toc<PlateEngine>(ZEND_THIS);

		str_rc ret = cobj->render(name,data);
		ret.move_zv(return_value);
	}
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

	STATE_INIT_ADD(PEI)
	
	return SUCCESS;
}


//plate_engine.cpp
#endif