#ifndef WCC_HMAP_CPP
#define WCC_HMAP_CPP

#ifndef WCC_HMAP_H
#include "hmap.h"
#endif

extern "C" {
	#include <Zend/zend_attributes.h>
  #include <Zend/zend_interfaces.h>
}

#ifndef HMAP_ARGINFO_H
#define HMAP_ARGINFO_H
extern "C" {
	#include "stub/hmap_arginfo.h"
}
#endif

namespace wcc {
	Hmap::Hmap_Mgr Hmap::omg;

	using namespace zpp;


	class Hmap_init : public state_init {
	public:
		Hmap_init() : state_init() {}

		zstr_intern data_key;
		zstr_intern obj_key;

		void init() override {
			data_key = "data";
			obj_key = "__obj";
		}
	};

Hmap_init HMAPit;

zval* 
Hmap::get_property_ptr_ptr(zend_object* object, zend_string* name, 
				int type, void** cache_slot)
{
	//if (!zend_std_has_property(object, name, ZEND_PROPERTY_EXISTS, cache_slot)) {
		Hmap* cobj = zobj_toc<Hmap>(object);
		htab_read look(cobj->data_);
		if (look.isNull())
		{
			return nullptr;
		}
		return look.get(name);
	//}
	/** type value must be good for something? */
	//return zend_std_get_property_ptr_ptr(object, name, type, cache_slot);
}


zval* 	
Hmap::read_property(zend_object* object, zend_string* name, int type, 
						void** cache_slot, zval* rv)
{
	//if (!zend_std_has_property(object, name, ZEND_PROPERTY_EXISTS, cache_slot)) 
	//{
		Hmap* cobj = zobj_toc<Hmap>(object);
		htab_read look(cobj->data_);
		if (look.isNull())
		{
			return nullptr;
		}
		return look.get(name);
	//}
	/** type value must be good for something? */
	//return zend_std_read_property(object, name, type, cache_slot, rv);
}

zval* 	
Hmap::write_property(zend_object* object, zend_string* name, zval* value, void** cache_slot)
{
	//if (!zend_std_has_property(object, name, ZEND_PROPERTY_EXISTS, cache_slot)) 
	//{
		Hmap* cobj = zobj_toc<Hmap>(object);
		htab_write hw(cobj->data_);
		zval* result = zend_hash_update(hw, name, value);
		if (Z_TYPE_FLAGS_P(result) != 0)
		{
			zval_mgr::try_addref(result);
		}
		return result;
	//}
	//return zend_std_write_property(object, name, value, cache_slot);
}

int
Hmap::has_property(zend_object* object, zend_string* name, int has_set_exists, void **cache_slot)
{
	/*int result = zend_std_has_property(object, name, ZEND_PROPERTY_EXISTS, cache_slot);
	if (result)
	{
		return result;
	}*/

	Hmap* cobj = zobj_toc<Hmap>(object);
	htab_read look(cobj->data_);
	if (look.isNull())
	{
		return false;
	}
	return look.has_key(name);
}

void  
Hmap::unset_property(zend_object* object, zend_string* name, void **cache_slot)
{
	/*if (zend_std_has_property(object, name, ZEND_PROPERTY_EXISTS, cache_slot))
	{
		zend_std_unset_property(object, name, cache_slot);
		return;
	}
	*/
	Hmap* cobj = zobj_toc<Hmap>(object);
	htab_write hw(cobj->data_);
	zend_hash_del(hw, name);
}

// seems to be direct easy way to count number of properties. Maybe not needed.
ZEND_RESULT_CODE 
Hmap::count_elements(zend_object* object, zend_long *count)
{
	Hmap* cobj = zobj_toc<Hmap>(object);
	htab_read look(cobj->data_);
	*count = look.size();
	return SUCCESS;
}

/*
HashTable*
Hmap::get_properties(zend_object* object)
{
	Hmap* cobj = zobj_toc<Hmap>(object);
	htab_read look(cobj->data_);
	return look;
}
*/
HashTable*
Hmap::get_properties_for(zend_object* object, zend_prop_purpose purpose)
{
	Hmap* cobj = zobj_toc<Hmap>(object);
	htab_read look(cobj->data_);
	switch(purpose)
	{
	case ZEND_PROP_PURPOSE_DEBUG:
	case ZEND_PROP_PURPOSE_ARRAY_CAST: 	
	case ZEND_PROP_PURPOSE_SERIALIZE:
	case ZEND_PROP_PURPOSE_VAR_EXPORT:
	case ZEND_PROP_PURPOSE_JSON:
	case ZEND_PROP_PURPOSE_GET_OBJECT_VARS:	
		HashTable* ht = (HashTable*) look;
		htab_mgr::try_addref(ht);
		return ht;
	}
	return nullptr;
}

zobj_mgr
Hmap::fromArray(zval_user htab)
{
	zobj_mgr result = Hmap::omg.new_zobj();
	Hmap* cobj = zobj_toc<Hmap>(result);
	cobj->construct(htab.zarray());
	return result;
}

#ifdef HMAP_DIMENSIONS
zval* 
Hmap::read_dimension(zend_object* obj, zval* offset, int type, zval* return_value)
{
	Hmap* cobj = zobj_toc<Hmap>(obj);
	htab_read look(cobj->data_);
	if (look.isNull())
	{
		return nullptr;
	}
	return look.get(offset);
}

void 
Hmap::write_dimension(zend_object* obj, zval* offset, zval* set_value)
{
	Hmap* cobj = zobj_toc<Hmap>(obj);
	htab_write hw(cobj->data_);
	hw.set(offset, set_value);
}

int  
Hmap::has_dimension(zend_object* object, zval* offset, int check_empty)
{
	Hmap* cobj = zobj_toc<Hmap>(object);
	htab_read look(cobj->data_);
	if (look.isNull())
	{
		return false;
	}
	zval* check = look.get(offset);
	if (!check) {
		return 0;
	}
	if (check_empty)
	{
		if (zval_user(check).ok())
		{
			return 1;
		}
	}
	return 1;
}

void  
Hmap::unset_dimension(zend_object* object, zval* unset)
{
	Hmap* cobj = zobj_toc<Hmap>(object);
	htab_write hw(cobj->data_);
	hw.unset(unset);
}
#endif



/*
HashTable* 
Hmap::get_gc(zend_object* obj, zval** gc_data, int *gc_data_count)
{

}



zval* 
Hmap::read_dimension(zend_object* obj, zval* offset, int type, zval* return_value)
{

	zend_printf("read_dim type %d ", type);
	showmem("ret val", return_value);
	showmem("offset", offset);

	Wcc_Hmap* cobj = zobj_toc<Hmap>(obj);
	zval* result = cobj->get(zval_user(offset));

	if ((type == BP_VAR_W || type == BP_VAR_RW || type == BP_VAR_UNSET) &&
	  !Z_ISREF_P(result) &&
	  EXPECTED(result != &EG(uninitialized_zval))) {
			ZVAL_NEW_REF(result, result);
	}
	return result;
}

void 
Hmap::write_dimension(zend_object* obj, zval* offset, zval* set_value)
{
	zend_printf("write_dim ");
	showmem("offset", offset);
	showmem("set val", set_value);
	Wcc_Hmap* cobj = zobj_toc<Hmap>(obj);
	cobj->set(zval_user(offset), zval_user(set_value));
}


void 
Hmap::unset_dimension(zend_object* obj, zval* unset)
{
	Wcc_Hmap* cobj = zobj_toc<Hmap>(obj);
	cobj->unset(zval_user(unset));
}


int  
Hmap::has_dimension(zend_object* obj, zval* offset, int check_empty)
{
	zend_printf("has_dim ");
	showmem("offset", offset);
	Wcc_Hmap* cobj = zobj_toc<Hmap>(obj);
	return cobj->has(zval_user(offset));
}

*/

/*
void 
Hmap::debug_info(htab_write hw)
{
	// base_d::debug_info(hw);

	//hw.set(HMAPit.data_key, data_);
}
*/

void 
Hmap::construct(htab_read values)
{
	if (values.size() > 0)
	{
		(htab_mgr&) data_ = values;
		//showdata("construct ", data_);
	}
}

zval_mgr 
Hmap::getOrNot(zstr_user name, zval_user ifnot)
{
	zval_user result;

	htab_read look(data_);

	if (look.ok())
	{
		if (!look.try_fetch(name, result))
		{
			result = ifnot;
		}
	}
	return result;
}

bool   
Hmap::has(zstr_user name)
{
	htab_read look(data_);
	return look.ok() && look.has_key(name);
}

zval_mgr
Hmap::get(zstr_user name)
{	
	zval_mgr result;
	htab_read look(data_);
	if (look.ok())
	{
		result = look.get(name);
	}
	return result;
}


///zend_std_unset_property
void  
Hmap::unset(zstr_user name)
{
	htab_write hw(data_);
	if (hw.ok())
	{
		hw.unset(name);
	}
}

void   
Hmap::set(zstr_user name, zval_user value)
{
	htab_write hw(data_);
	hw.set(name, value);
}

#ifdef CONFIG_DIMENSIONS

bool   
Hmap::has(zval_user key)
{
	zstr_mgr skey = key.to_zstr();
	return has(skey);
}

void   
Hmap::set(zval_user key, zval_user value)
{
	//showmem("set key", key);
	//showmem("set value", value);

	if (key.isString())
	{
		set(key.zstr(), value);
	}
	else {
		zstr_mgr skey = key.to_zstr();
		//showstr("set interned key", skey);
		set(skey, value);
	}
}

zval_mgr 
Hmap::get(zval_user name)
{
	zstr_mgr skey = name.to_zstr();
	//showstr("config get", skey);

	zval_mgr result = zobj_user(this->vobj()).property(skey);
	//showmem("config get", result);
	return result;

}

void   
Hmap::unset(zval_user key)
{
	zstr_mgr skey = key.to_zstr();
	zobj_user(this->vobj()).unset_property(skey);
}
#endif


htab_mgr 
Hmap::subsetkey(zstr_user key)
{
	htab_mgr result;
	htab_write hw(result);
	zval_mgr value = get(key);
	hw.set(key, value);
	return result;
}

htab_mgr 
Hmap::subset(htab_read data)
{
	htab_mgr result;
	htab_write hw(result);

	for_key_value wk;
	zval_user vkey;

	for(wk.start(data); wk.ok(); wk.next())
	{
		vkey = wk.value();
		zstr_mgr key(vkey.to_zstr());
		zval_mgr value = this->get(key);
		hw.set(key, value);
	}
	return result;
}

void   
Hmap::addArray(htab_read data)
{
	for_key_value fkv;
	htab_write hw(data_);

	for(fkv.start(data); fkv.ok(); fkv.next())
	{
		hw.set(fkv.key(), fkv.value());
	}
}

htab_read 
Hmap::toArray()
{
	return htab_read(data_);
}

zstr_mgr 
Hmap::unhive(zstr_user subj)
{
	preg sfind("#@([a-zA-Z][\\w\\d]*)#", preg::OFFSET_CAPTURE, true);

	int ct = sfind.matches(subj);
	if (ct > 0) {
		htab_read m = sfind.results();

		htab_read replace_list = m.get((int)0);
		htab_read keys_list = m.get(1);

		std::string_view original = subj.vstr();

		zstr_buffer result;
		size_t ipos = 0;

		htab_read mydata(data_);

		for(int i = 0; i < ct; i++)
		{
			htab_read k1 = keys_list.get(i);
			zval_user fkey = k1.get((int)0);
			//showmem("get key", fkey);
			
			zval_mgr rval = mydata.get(fkey);

			//showmem("replace value", rval);
			zstr_user replace_str = zval_user(rval).zstr();

			htab_read f1 = replace_list.get(i);
			zstr_user slen_f1 = f1.get((int)0);
			zval_user soffset_f1 = f1.get(1);

			size_t slen = slen_f1.size();
			zend_long soffset = soffset_f1.zlong();

			if (!replace_str)
			{
				result << original.substr(ipos, soffset-ipos);
			} 
			else {
				result << original.substr(ipos, soffset-ipos);
				result << replace_str;
			}
			ipos = soffset + slen;
		}
		if (ipos < original.size()) {
			result << original.substr(ipos);
		}
		return std::move(result);
	}
	else {
		return zstr_mgr(subj);
	}
}

htab_mgr 
Hmap::serialize()
{
	zobj_user self(vobj());

	htab_mgr result;

	HashTable* ht = Hmap::omg.handlers_.get_properties_for(self, ZEND_PROP_PURPOSE_SERIALIZE);

	result.adopt(ht);

	return result;
}

void 
Hmap::unserialize(htab_read htab)
{

	/*
	zobj_user self(vobj());

	for_key_value wk;
	htab_mgr properties = htab.get(HMAPit.obj_key);
	//showdata("unserialize ", properties);
	for(wk.start(properties); wk.ok(); wk.next())
	{
		zend_std_write_property(self, wk.key(), wk.value(), nullptr);
	}
	(htab_mgr&) data_ = htab;
	htab_write hw(data_);
	hw.unset(HMAPit.property_key);
	*/

	(htab_mgr&)data_ = htab;
}

void
Hmap::clear()
{
	data_.init();
}

}; // namespace wcc //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@&&&&&&&&&&&&&&&&&&&&&&&&@@@@@@@@@@@@@@@

ZEND_METHOD(Wcc_Hmap, __construct)
{
	zval* data = nullptr;

	ZEND_PARSE_PARAMETERS_START(0,1)
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY_OR_NULL(data)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Hmap>(ZEND_THIS);

	htab_read arg1;

	if (data) {
		arg1 = zval_user(data).zarray();
	}
	cobj->construct(arg1);
}

ZEND_METHOD(Wcc_Hmap, getOrNot)
{
	zend_string* key;
	zval* ifnot = nullptr;

	ZEND_PARSE_PARAMETERS_START(1,2)
	Z_PARAM_STR(key)
	Z_PARAM_OPTIONAL
	Z_PARAM_ZVAL_OR_NULL(ifnot)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Hmap>(ZEND_THIS);
	zval_user temp;

	if (ifnot) {
		temp = ifnot;
	}
	zval_mgr result = cobj->getOrNot(key, temp);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_Hmap, has)
{
	zend_string* key;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Hmap>(ZEND_THIS);

	bool temp = cobj->has(key);
	RETURN_BOOL(temp);
}

ZEND_METHOD(Wcc_Hmap, get)
{
	zend_string* key;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Hmap>(ZEND_THIS);

	zval_mgr temp = cobj->get(key);
	temp.move_zv(return_value);
}

ZEND_METHOD(Wcc_Hmap, set)
{
	zend_string* key;
	zval*    value;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_STR(key)
	Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Hmap>(ZEND_THIS);

	//showstr("set call", key);
	cobj->set(key, value);
}

ZEND_METHOD(Wcc_Hmap, unset)
{
	zend_string* key;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Hmap>(ZEND_THIS);

	cobj->unset(key);

}

ZEND_METHOD(Wcc_Hmap, subset)
{
	HashTable*  list;
	zend_string* item;


	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY_HT_OR_STR_EX(list, item, true)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Hmap>(ZEND_THIS);
	htab_mgr temp;

	if (list)
	{
		temp = cobj->subset(list);
	}
	else if (item)
	{
		temp = cobj->subsetkey(item);
	}
	else {
		temp = cobj->toArray();
	}
	temp.move_zv(return_value);
}

ZEND_METHOD(Wcc_Hmap, addArray)
{
	zval* elist;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(elist)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Hmap>(ZEND_THIS);

	cobj->addArray(elist);

}

ZEND_METHOD(Wcc_Hmap, toArray)
{
	ZEND_PARSE_PARAMETERS_START(0,0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Hmap>(ZEND_THIS);

	htab_read result = cobj->toArray();
	result.return_zv(return_value);
}

ZEND_METHOD(Wcc_Hmap, unhive)
{
	zend_string* trans;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(trans)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Hmap>(ZEND_THIS);

	zstr_mgr result = cobj->unhive(trans);
	result.move_zv(return_value);
}

/*
ZEND_METHOD(Wcc_Hmap, offsetGet)
{
	zval* key;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Hmap>(ZEND_THIS);

	zval_mgr temp = cobj->get(zval_user(key));
	temp.move_zv(return_value);
}

ZEND_METHOD(Wcc_Hmap, offsetSet)
{
	zval*	key;
	zval*	value;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_ZVAL(key)
	Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Hmap>(ZEND_THIS);
		zend_printf("offsetSet called\n");
	cobj->set(zval_user(key), zval_user(value));
}

ZEND_METHOD(Wcc_Hmap, offsetExists)
{
	zval* key;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Hmap>(ZEND_THIS);

	bool temp = cobj->has(zval_user(key));
	RETURN_BOOL(temp);
}


ZEND_METHOD(Wcc_Hmap, offsetUnset)
{
	zval* key;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Hmap>(ZEND_THIS);

	cobj->unset(zval_user(key));
}




ZEND_METHOD(Wcc_Hmap, count)
{
	ZEND_PARSE_PARAMETERS_START(0,0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Hmap>(ZEND_THIS);

	
	RETURN_LONG(cobj->count());
}

*/

ZEND_METHOD(Wcc_Hmap, __serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Hmap* cobj = zval_toc<Hmap>(ZEND_THIS);

	htab_mgr ret = cobj->serialize();
	ret.move_zv(return_value);
}

ZEND_METHOD(Wcc_Hmap, __unserialize)
{
	zval* data;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ARRAY(data)
	ZEND_PARSE_PARAMETERS_END();

	htab_read hw(Z_ARR_P(data));

	Hmap*   cobj = zval_toc<Hmap>(ZEND_THIS);

	cobj->unserialize(hw);
	return;
}

ZEND_METHOD(Wcc_Hmap, clear)
{
	ZEND_PARSE_PARAMETERS_NONE();

	auto cobj = zval_toc<Hmap>(ZEND_THIS);

	cobj->clear();
}

PHP_MINIT_FUNCTION(Wcc_Hmap_reg)
{
	//auto ce = register_class_Wcc_Hmap(zend_ce_arrayaccess, zend_ce_countable);
	//zend_standard_class_def
	auto ce = register_class_Wcc_Hmap();
	Hmap::omg.classEntry(ce);

	return SUCCESS;
}

//Wcc_Hmap.cpp
#endif