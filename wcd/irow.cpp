#ifndef WCD_IROW_CPP
#define WCD_IROW_CPP

#ifndef WCD_IROW_H
#include "irow.h"
#endif

#ifndef MODEL_ARGINFO_H
#define MODEL_ARGINFO_H
extern "C" {
     #include "stub/model_arginfo.h"
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
	zstr_intern  data_str;
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
	data_str = "data";
	original_data = "original";
	save_key = "saverow";

	read_key = "readrow";
	delete_key = "deleterow";
	getcoldefs = "getcoldefs";
	stamptime = "stamptime";
}

IRow::IRow() : Hmap ()
{
	original_ = htab_rc::empty_array();
}

IRow::~IRow()  
{
}

void 
IRow::construct(obj_ptr tmodel, htab_ptr data, bool exists)
{
	table_model_ = tmodel;
	setData(data, exists);
}

void IRow::debug_info(htab_rw di)
{
	
	di.set(IRSTR.data_str, data_);
	di.set(IRSTR.original_data, original_);
	di.set(IRSTR.model, table_model_);

}

void 
IRow::setData(htab_ptr data, bool exists)
{
	if (data.size())
	{
		data_ = data;
		if (exists)
		{
			original_ = data;
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
	val_rc self(vobj());
	//zend_printf("IRow delete\n");
	//showobj("table_model", table_model_);

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
	val_rc self(vobj());
	val_rc arg;

	arg.set_bool(reload);

	val_rc result = table_model_.call(IRSTR.save_key, self, arg);
	return val_ptr(result).isTrue(); 
}


bool 
IRow::update(bool reload)
{
	return save(reload);
}


void 
IRow::read()
{
	val_rc self(vobj());
	table_model_.call(IRSTR.read_key, self);	
}


htab_rc 
IRow::getDataValues(htab_ptr attrlist)
{
	htab_rc result;

	if (data_.size())
	{
		result = htab_rc::subset(attrlist, data_, true);
	}
	return result;
}

bool 
IRow::hasValue(str_ptr key)
{
	val_ptr value = data_.get(key);
	return value.ok();
}

htab_rc 
IRow::getDirty()
{
	htab_rc result;
	result = htab_rc::empty_array();

	htab_rw dirty(result);

	if (original_ == data_) {
		return result;
	}

	htab_walk wk;

	auto skey = wk.key();
	auto dvalue = wk.value();

	for(wk.start(data_); wk.ok(); wk.next())
	{
		val_ptr orig = original_.get(skey);
		if (orig != dvalue) 
		{
			dirty.push_back(skey);
		}
	}
	return result;
}

htab_ptr 
IRow::getData() const
{
	return data_;
}

bool 
IRow::isDirty(str_ptr colname)
{
	if (data_.size() == 0)
		return false;

	if (original_.size() == 0)
		return true;

	if (colname.size())
	{
		val_ptr orig = original_.get(colname);
		val_ptr dvalue = data_.get(colname);
		return (orig != dvalue);
	}
	htab_walk wk;

	auto skey = wk.key();
	auto dvalue = wk.value();

	for(wk.start(data_); wk.ok(); wk.next())
	{
		val_ptr orig = original_.get(skey);
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
IRow::mergeData(htab_ptr attrs)
{
	if (attrs.size() == 0)
	{
		return;
	}
	val_rc defs_zval_mgr = table_model_.call(IRSTR.getcoldefs);
	val_ptr defs_zval(defs_zval_mgr);
	htab_rc cdefs;


	if (defs_zval.isArray())
	{
		cdefs = defs_zval.zarray();
	}

	htab_rw hw(data_);

	htab_walk wk;
	auto key = wk.key();
	auto value = wk.value();

	for(wk.start(attrs); wk.ok(); wk.next())
	{
		str_ptr cname = key.zstr();
		if (cdefs.size() && !cdefs.has_key(cname))
		{
			zend_throw_error(zend_ce_error, "Unknown attribute %s", cname.data());
			return;
		}
		hw.set(cname, value);
	}
}


void 
IRow::setExists()
{
	original_ = data_;
}

void IRow::copy(obj_rc recobj)
{
	IRow* rec = zobj_toc<IRow>(recobj);
	data_ = rec->data_;
	original_ = rec->original_;
}

htab_rc 
IRow::stampTime(str_ptr value, int dtflags)
{
	htab_rc result;

	result = htab_rc::empty_array();

	val_rc arg1(value);
	val_rc arg2(dtflags);

	val_rc ta_array = table_model_.call(IRSTR.stamptime, arg1, arg2);
	

	val_ptr test(ta_array);

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
	HashTable* data = nullptr;
	bool  existsFlag = false;

	ZEND_PARSE_PARAMETERS_START(1,3)
	Z_PARAM_OBJECT_OF_CLASS(model, ce_iface_crud);
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY_HT_OR_NULL(data)
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
	htab_ptr data = cobj->toArray();
	data.return_zv(return_value);

}


ZEND_METHOD(Wcd_IRow, getDirty)
{
	ZEND_PARSE_PARAMETERS_NONE();
	auto cobj = zval_toc<IRow>(ZEND_THIS);
	htab_rc data = cobj->getDirty();
	data.move_zv(return_value);
}

ZEND_METHOD(Wcd_IRow, getModel)
{
	ZEND_PARSE_PARAMETERS_NONE();
	auto cobj = zval_toc<IRow>(ZEND_THIS);
	obj_ptr data = cobj->getModel();
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

	obj_rc rowobj = cobj->vobj();
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
	obj_rc rowobj = cobj->vobj();
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
	htab_rc data = cobj->stampTime(key, tsflags);
	data.move_zv(return_value);
}

ZEND_METHOD(Wcd_IRow, getDataValues)
{
	zval* names;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(names)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<IRow>(ZEND_THIS);
	htab_rc data = cobj->getDataValues(names);
	data.move_zv(return_value);
}


//irow.cpp
#endif