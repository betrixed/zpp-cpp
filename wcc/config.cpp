#ifndef WCC_CONFIG_CPP
#define WCC_CONFIG_CPP

#ifndef WCC_CONFIG_H
#include "config.h"
#endif

extern "C" {
	#include <Zend/zend_attributes.h>
	#include "stub/config_arginfo.h"
	
    #include <Zend/zend_interfaces.h>
}


namespace wcc {
	Config::Config_Mgr Config::omg;

	using namespace zpp;

/** type must be good for something? */

#ifdef CONFIG_DIMENSIONS
zval* 
Config::read_dimension(zend_object* obj, zval* offset, int type,  zval* return_value)
{
	return obj_ptr(obj).property_get(offset, return_value);
}

void 
Config::write_dimension(zend_object* obj, zval* offset,  zval* set_value)
{
	//showmem("write dimension offset", offset);
	Config* cobj = zobj_toc<Config>(obj);
	cobj->set(val_ptr(offset), val_ptr(set_value));

	//obj_ptr(obj).property(offset, set_value);
}

int   
Config::has_dimension(zend_object *object, zval *offset, int check_empty)
{
	return obj_ptr(object).has_property(offset);
}

void   
Config::unset_dimension(zend_object *object, zval *unset)
{
	return obj_ptr(object).unset_property(unset);
}
#endif


obj_rc // static
Config::make(htab_rd initdata)
{
	obj_rc result = Config::omg.new_zobj();
	Config* cobj = zobj_toc<Config>(result);

	cobj->construct(initdata);
	return result;
}

void 
Config::debug_info(htab_rw hw)
{
	base_d::debug_info(hw);
}

void 
Config::construct(htab_rd values)
{
	if (values.size() > 0)
	{
		obj_ptr self(this->vobj());
		for_key_value wk;

		for(wk.start(values); wk.ok(); wk.next())
		{
			set(wk.key(),wk.value());
		}
	}
}


val_rc 
Config::getOrNot(str_ptr name, val_ptr ifnot)
{
	val_rc result;

	obj_ptr self(this->vobj());

	//result = self.property(name);

	//zend_std_has_property(object, name, ZEND_PROPERTY_EXISTS, NULL)
	int hasprop = Config::omg.handlers_.has_property(self, name, ZEND_PROPERTY_EXISTS, nullptr);
	if (hasprop)
	{
		//showstr("exists", name);
		//static  zval* read_property(zend_object *object, zend_string *name, int type, void **cache_slot, zval *rv);
		zval* direct = Config::omg.handlers_.read_property(self, name, BP_VAR_R, nullptr, result);
		if (direct)
		{
			//showmem("indirect result", result);
			result = direct;
			//showmem("direct result", result);
		}
	}
	else {
		//showstr("missing", name);
		//showmem("result", result);
	}
	if (val_ptr(result).isNull() && !ifnot.isNull())
	{
		result = ifnot;
	}
	return result;
}

bool      
Config::has(str_ptr name)
{
	obj_ptr self(this->vobj());

	return self.has_property(name);
}

val_rc
Config::get(str_ptr name)
{
	obj_ptr self(this->vobj());

	return self.property(name);
}


///zend_std_unset_property
void   
Config::unset(str_ptr  name)
{
	zend_std_unset_property(this->vobj(), name, nullptr);
}

void      
Config::set(str_ptr  name, val_ptr value)
{
	//showstr("set str_ptr", name);
	obj_ptr(this->vobj()).property(name, value);
}

#ifdef CONFIG_DIMENSIONS

bool      
Config::has(val_ptr key)
{
	str_rc skey = key.to_zstr();
	return has(skey);
}

void      
Config::set(val_ptr  key,  val_ptr value)
{
	//showmem("set key", key);
	//showmem("set value", value);

	if (key.isString())
	{
		set(key.zstr(), value);
	}
	else {
		str_rc skey = key.to_zstr();
		//showstr("set interned key", skey);
		set(skey, value);
	}
}

val_rc  
Config::get(val_ptr name)
{
	str_rc skey = name.to_zstr();
	//showstr("config get", skey);

	val_rc result = obj_ptr(this->vobj()).property(skey);
	//showmem("config get", result);
	return result;

}

void      
Config::unset(val_ptr  key)
{
	str_rc skey = key.to_zstr();
	obj_ptr(this->vobj()).unset_property(skey);
}
#endif


htab_rc  
Config::subsetkey(str_ptr key)
{

	htab_rc result;
	htab_rw hw(result);
	val_rc value = get(key);
	hw.set(key, value);

	return result;
}

htab_rc  
Config::subset(htab_rd data)
{
	for_key_value wk;
	htab_rc result;
	htab_rw hw(result);
	val_ptr vkey;

	for(wk.start(data); wk.ok(); wk.next())
	{
		vkey = wk.value();
		str_rc key(vkey.to_zstr());
		val_rc value = this->get(key);

		hw.set(key, value);
	}
	return result;
}

void      
Config::addArray(htab_rd data)
{
	for_key_value fkv;
	for(fkv.start(data); fkv.ok(); fkv.next())
	{
		set(fkv.key(), fkv.value());
	}
}

htab_rc  
Config::toArray()
{
	htab_rc result;

	HashTable* ht = zend_std_get_properties_for(this->vobj(), ZEND_PROP_PURPOSE_GET_OBJECT_VARS);
	result.adopt(ht);
	return result;
}

void
Config::clear()
{
	htab_rc temp = toArray();

	htab_rd look(temp);

	for_key_value wk;

	obj_ptr self(vobj());

	for(wk.start(look); wk.ok(); wk.next())
	{
		self.unset_property(wk.key());
	}
}

str_rc 
Config::unhive(str_ptr subj)
{
	preg sfind("#@([a-zA-Z][\\w\\d]*)#", preg::OFFSET_CAPTURE, true);

	int ct = sfind.matches(subj);
	if (ct > 0) {
		htab_rd m = sfind.results();

		htab_rd replace_list = m.get((int)0);
		htab_rd keys_list = m.get(1);

		std::string_view original = subj.vstr();

		str_buf result;
		size_t ipos = 0;

		for(int i = 0; i < ct; i++)
		{
			htab_rd  k1 = keys_list.get(i);
			val_ptr fkey = k1.get((int)0);
			//showmem("get key", fkey);
			
			val_rc rval = this->get(fkey);
			//showmem("replace value", rval);
			str_ptr replace_str = val_ptr(rval).zstr();

			htab_rd f1 = replace_list.get(i);
			str_ptr  slen_f1 = f1.get((int)0);
			val_ptr  soffset_f1 = f1.get(1);

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
		return result.zstr();
	}
	else {
		return str_rc(subj);
	}
}

}; // namespace wcc //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@&&&&&&&&&&&&&&&&&&&&&&&&@@@@@@@@@@@@@@@

ZEND_METHOD(Wcc_Config, __construct)
{
	zval* data = nullptr;

	ZEND_PARSE_PARAMETERS_START(0,1)
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY_OR_NULL(data)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Config>(ZEND_THIS);

	htab_rd arg1;

	if (data) {
		arg1 = val_ptr(data).zarray();
	}
	cobj->construct(arg1);
}

ZEND_METHOD(Wcc_Config, getOrNot)
{
	zend_string* key;
	zval* ifnot = nullptr;

	ZEND_PARSE_PARAMETERS_START(1,2)
	Z_PARAM_STR(key)
	Z_PARAM_OPTIONAL
	Z_PARAM_ZVAL_OR_NULL(ifnot)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Config>(ZEND_THIS);
	val_ptr temp;

	if (ifnot) {
		temp = ifnot;
	}
	val_rc result = cobj->getOrNot(key, temp);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_Config, has)
{
	zend_string* key;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Config>(ZEND_THIS);

	bool temp = cobj->has(key);
	RETURN_BOOL(temp);
}

ZEND_METHOD(Wcc_Config, get)
{
	zend_string* key;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Config>(ZEND_THIS);

	val_rc temp = cobj->get(key);
	temp.move_zv(return_value);
}

ZEND_METHOD(Wcc_Config, set)
{
	zend_string* key;
	zval*        value;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_STR(key)
	Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Config>(ZEND_THIS);

	showstr("set call", key);
	cobj->set(key, value);
}

ZEND_METHOD(Wcc_Config, unset)
{
	zend_string* key;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Config>(ZEND_THIS);

	cobj->unset(key);

}

ZEND_METHOD(Wcc_Config, subset)
{
	HashTable*   list;
	zend_string* item;


	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY_HT_OR_STR_EX(list, item, true)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Config>(ZEND_THIS);
	htab_rc temp;

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

ZEND_METHOD(Wcc_Config, addArray)
{
	zval* elist;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(elist)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Config>(ZEND_THIS);

	cobj->addArray(elist);

}

ZEND_METHOD(Wcc_Config, clear)
{
	ZEND_PARSE_PARAMETERS_NONE();

	auto cobj = zval_toc<Config>(ZEND_THIS);

	cobj->clear();
}


ZEND_METHOD(Wcc_Config, toArray)
{
	ZEND_PARSE_PARAMETERS_NONE();

	auto cobj = zval_toc<Config>(ZEND_THIS);

	htab_rc result = cobj->toArray();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Config, unhive)
{
	zend_string* trans;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(trans)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Config>(ZEND_THIS);

	str_rc result = cobj->unhive(trans);
	result.move_zv(return_value);
}

/*
ZEND_METHOD(Wcc_Config, offsetGet)
{
	zval* key;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Config>(ZEND_THIS);

	val_rc temp = cobj->get(val_ptr(key));
	temp.move_zv(return_value);
}

ZEND_METHOD(Wcc_Config, offsetSet)
{
	zval*	key;
	zval*	value;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_ZVAL(key)
	Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Config>(ZEND_THIS);
		zend_printf("offsetSet called\n");
	cobj->set(val_ptr(key), val_ptr(value));
}

ZEND_METHOD(Wcc_Config, offsetExists)
{
	zval* key;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Config>(ZEND_THIS);

	bool temp = cobj->has(val_ptr(key));
	RETURN_BOOL(temp);
}


ZEND_METHOD(Wcc_Config, offsetUnset)
{
	zval* key;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Config>(ZEND_THIS);

	cobj->unset(val_ptr(key));
}
*/


/*
ZEND_METHOD(Wcc_Config, count)
{
	ZEND_PARSE_PARAMETERS_START(0,0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Config>(ZEND_THIS);

	RETURN_LONG(cobj->count());
}
*/



PHP_MINIT_FUNCTION(Wcc_Config_reg)
{
	//auto ce = register_class_Wcc_Config(zend_ce_arrayaccess, zend_ce_countable);
	//zend_standard_class_def
	auto ce = register_class_Wcc_Config();
	Config::omg.classEntry(ce);

	return SUCCESS;
}

//wcc_config.cpp
#endif