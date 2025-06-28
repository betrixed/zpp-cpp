#ifndef WCD_IROW_CPP
#define WCD_IROW_CPP

#ifndef WCD_IROW_H
#include "irow.h"
#endif

#ifndef IROW_ARGINFO_H
#define IROW_ARGINFO_H
extern "C" {
	#include "stub/irow_arginfo.h"
}
#endif

#ifndef DB_ARGINFO_H
#define DB_ARGINFO_H
extern "C" {
	#include "stub/db_arginfo.h"
}
#endif


namespace wcd {

using namespace zpp;

IRow_mgr IRow::omg;

class IRInit : public state_init {
public:
	IRInit() : state_init() {}

	void init() override;

	zstr_intern  model;
	zstr_intern  original_data;
	zstr_intern  save_key;
	zstr_intern  read_key;
	zstr_intern  delete_key;
	zstr_intern  getcoldefs;
	zstr_intern  stamptime;

};

IRInit  IRSTR;

zend_class_entry* ce_iface_crud;

void 
IRInit::init()
{
	model = "model";
	original_data = "original";
	save_key = "save";
	read_key = "read";
	delete_key = "delete";
	getcoldefs = "getcoldefs";
	stamptime = "stamptime";
}

IRow::IRow() : Hmap ()
{
	original_ = htab_mgr::empty_array();
}

IRow::~IRow()  
{
}

void 
IRow::construct(zobj_user tmodel, zval_user data, bool exists)
{
	table_model_ = tmodel;
	setData(data, exists);
}

void IRow::debug_info(htab_write di)
{
	Hmap::debug_info(di);
	di.set(IRSTR.model, table_model_);
	di.set(IRSTR.original_data, original_);
}

void 
IRow::setData(zval_user data, bool exists)
{
	if (data.isArray())
	{
		HashTable* ht = data.zarray();

		(htab_mgr&) data_ = ht;
		if (exists)
		{
			(htab_mgr&) original_ = ht;
		}
	}
	else {
		data_.init();
		if (exists) {
			original_.init();
		}
	}
}

bool 
IRow::create(bool reload)
{
	return save(reload);
}

void 
IRow::delete_row()
{
	zval_mgr self(vobj());
	table_model_.call(IRSTR.delete_key, self);
}

bool 
IRow::exists()
{
	return (original_.size() > 0);
}


bool 
IRow::save(bool reload)
{
	zval_mgr self(vobj());
	zval_mgr arg;

	arg.set_bool(reload);

	zval_mgr result = table_model_.call(IRSTR.save_key, self, arg);
	return zval_user(result).isTrue(); 
}


bool 
IRow::update(bool reload)
{
	return save(reload);
}


void 
IRow::read()
{
	zval_mgr self(vobj());
	table_model_.call(IRSTR.read_key, self);	
}


htab_mgr 
IRow::getDataValues(zval_user attrlist)
{
	htab_mgr result;

	if (attrlist.isArray())
	{
		if (data_.size())
		{
			result = htab_mgr::subset(attrlist, data_, true);
		}
	}
	return result;
}

bool 
IRow::hasValue(zstr_user key)
{
	zval_user value = data_.get(key);
	return value.ok();
}

htab_mgr 
IRow::getDirty()
{
	htab_mgr result;
	result = htab_mgr::empty_array();

	htab_write dirty(result);

	if (original_ == data_) {
		return result;
	}

	htab_walk wk;

	auto skey = wk.key();
	auto dvalue = wk.value();

	for(wk.start(data_); wk.ok(); wk.next())
	{
		zval_user orig = original_.get(skey);
		if (orig != dvalue) 
		{
			dirty.push_back(skey);
		}
	}
	return result;
}


bool 
IRow::isDirty(zstr_user colname)
{
	if (data_.size() == 0)
		return false;

	if (original_.size() == 0)
		return true;

	if (colname.size())
	{
		zval_user orig = original_.get(colname);
		zval_user dvalue = data_.get(colname);
		return (orig != dvalue);
	}
	htab_walk wk;

	auto skey = wk.key();
	auto dvalue = wk.value();

	for(wk.start(data_); wk.ok(); wk.next())
	{
		zval_user orig = original_.get(skey);
		if (	((zval*) orig) == ((zval*) dvalue) )
			continue;
		if (orig != dvalue) 
		{
			return true;
		}
	}
	return false;
}


void 
IRow::mergeData(zval_user attrlist)
{
	if (attrlist.isArray())
	{
		htab_read attrs(attrlist.zarray());

		if (attrs.size() == 0)
		{
			return;

		}
		zval_mgr defs_zval_mgr = table_model_.call(IRSTR.getcoldefs);
		zval_user defs_zval(defs_zval_mgr);
		htab_mgr cdefs;


		if (defs_zval.isArray())
		{
			cdefs = defs_zval.zarray();
		}


		htab_write hw(data_);

		htab_walk wk;
		auto key = wk.key();
		auto value = wk.value();


		for(wk.start(attrs); wk.ok(); wk.next())
		{
			zstr_user cname = key.zstr();
			if (cdefs.size() && !cdefs.has_key(cname))
			{
				zend_throw_error(zend_ce_error, "Unknown attribute %s", cname.data());
				return;
			}
			hw.set(cname, value);
		}
	}
}


void 
IRow::setExists()
{
	original_ = data_;
}

void IRow::copy(zobj_mgr recobj)
{
	IRow* rec = zobj_toc<IRow>(recobj);
	data_ = rec->data_;
	original_ = rec->original_;
}

htab_mgr 
IRow::stampTime(zstr_user value, int dtflags)
{
	htab_mgr result;

	result = htab_mgr::empty_array();

	zval_mgr arg1(value);
	zval_mgr arg2(dtflags);

	zval_mgr ta_array = table_model_.call(IRSTR.stamptime, arg1, arg2);
	
	zval_user test(ta_array);

	if (test.size())
	{
		mergeData(test);
		result = test.zarray();
	}
	
	return result;
}


}; // namespace wcd

using namespace wcd;

ZEND_METHOD(Wcd_IRow, __construct)
{
	zval* model;
	zval* data = nullptr;
	bool  existsFlag = false;

	ZEND_PARSE_PARAMETERS_START(1,3)
	Z_PARAM_OBJECT_OF_CLASS(model, ce_iface_crud);
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY_OR_NULL(data)
	Z_PARAM_BOOL(existsFlag)

	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<IRow>(ZEND_THIS);
	cobj->construct( model, data, existsFlag);

}

ZEND_METHOD(Wcd_IRow, setData)
{
	zval* data = nullptr;
	bool  existsFlag = false;

	ZEND_PARSE_PARAMETERS_START(1,2)
	Z_PARAM_ARRAY_OR_NULL(data)
	Z_PARAM_OPTIONAL
	Z_PARAM_BOOL(existsFlag)

	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<IRow>(ZEND_THIS);
	cobj->setData(data, existsFlag);
}

ZEND_METHOD(Wcd_IRow, getData)
{
	ZEND_PARSE_PARAMETERS_NONE();
	auto cobj = zval_toc<IRow>(ZEND_THIS);
	htab_read data = cobj->toArray();
	data.return_zv(return_value);

}


ZEND_METHOD(Wcd_IRow, getDirty)
{
	ZEND_PARSE_PARAMETERS_NONE();
	auto cobj = zval_toc<IRow>(ZEND_THIS);
	htab_mgr data = cobj->getDirty();
	data.move_zv(return_value);
}

ZEND_METHOD(Wcd_IRow, getModel)
{
	ZEND_PARSE_PARAMETERS_NONE();
	auto cobj = zval_toc<IRow>(ZEND_THIS);
	zobj_user data = cobj->getModel();
	data.return_zv(return_value);
}

ZEND_METHOD(Wcd_IRow, hasValue)
{
	zend_string* key;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(key);
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<IRow>(ZEND_THIS);
	RETURN_BOOL(cobj->hasValue(key));
}

ZEND_METHOD(Wcd_IRow, isDirty)
{
	zend_string* key = nullptr;

	ZEND_PARSE_PARAMETERS_START(0,1)
	Z_PARAM_OPTIONAL
	Z_PARAM_STR_OR_NULL(key);
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<IRow>(ZEND_THIS);
	RETURN_BOOL(cobj->isDirty(key));	
}

ZEND_METHOD(Wcd_IRow, mergeData)
{
	zval* vset;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(vset);
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<IRow>(ZEND_THIS);
	cobj->mergeData(vset);

	zobj_mgr rowobj = cobj->vobj();
	rowobj.move_zv(return_value);
}

ZEND_METHOD(Wcd_IRow, create)
{
	bool reload = false;

	ZEND_PARSE_PARAMETERS_START(0,1)
	Z_PARAM_OPTIONAL
	Z_PARAM_BOOL(reload);
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<IRow>(ZEND_THIS);
	RETURN_BOOL(cobj->save(reload));
}

ZEND_METHOD(Wcd_IRow, delete)
{
	ZEND_PARSE_PARAMETERS_NONE();

	auto cobj = zval_toc<IRow>(ZEND_THIS);
	cobj->delete_row();
}

ZEND_METHOD(Wcd_IRow, exists)
{
	ZEND_PARSE_PARAMETERS_NONE();

	auto cobj = zval_toc<IRow>(ZEND_THIS);
	RETURN_BOOL(cobj->exists());
}

ZEND_METHOD(Wcd_IRow, read)
{
	ZEND_PARSE_PARAMETERS_NONE();

	auto cobj = zval_toc<IRow>(ZEND_THIS);
	cobj->read();
	zobj_mgr rowobj = cobj->vobj();
	rowobj.move_zv(return_value);
}

ZEND_METHOD(Wcd_IRow, save)
{
	bool reload = false;

	ZEND_PARSE_PARAMETERS_START(0,1)
	Z_PARAM_OPTIONAL
	Z_PARAM_BOOL(reload);
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<IRow>(ZEND_THIS);
	RETURN_BOOL(cobj->save(reload));	
}

ZEND_METHOD(Wcd_IRow, update)
{
	bool reload = false;

	ZEND_PARSE_PARAMETERS_START(0,1)
	Z_PARAM_OPTIONAL
	Z_PARAM_BOOL(reload);
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<IRow>(ZEND_THIS);
	RETURN_BOOL(cobj->save(reload));	
}

ZEND_METHOD(Wcd_IRow, setExists)
{
	ZEND_PARSE_PARAMETERS_NONE();

	auto cobj = zval_toc<IRow>(ZEND_THIS);
	cobj->setExists();	
}

ZEND_METHOD(Wcd_IRow, stampTime)
{
	zend_string* key;
	zend_long    tsflags = Model::ALL_TS;

	ZEND_PARSE_PARAMETERS_START(1,2)
	Z_PARAM_STR(key);
	Z_PARAM_OPTIONAL
	Z_PARAM_LONG(tsflags)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<IRow>(ZEND_THIS);
	htab_mgr data = cobj->stampTime(key, tsflags);
	data.move_zv(return_value);
}

ZEND_METHOD(Wcd_IRow, getDataValues)
{
	zval* names;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(names)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<IRow>(ZEND_THIS);
	htab_mgr data = cobj->getDataValues(names);
	data.move_zv(return_value);

}

PHP_MINIT_FUNCTION(IRow_reg)
{
	//auto ce = register_class_Wcc_Hmap(zend_ce_arrayaccess, zend_ce_countable);
	//zend_standard_class_def


	ce_iface_crud = register_class_Wcd_IfCrud();

	auto ce = register_class_Wcd_IRow(
		Hmap::omg.classEntry()
		);

	IRow::omg.classEntry(ce);

	//zend_printf("registered IRow\n");
	return SUCCESS;
}

//irow.cpp
#endif