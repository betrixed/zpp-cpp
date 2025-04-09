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
	return zobj_user(obj).property_get(offset, return_value);
}

void 
Config::write_dimension(zend_object* obj, zval* offset,  zval* set_value)
{
	//showmem("write dimension offset", offset);
	Config* cobj = zobj_toc<Config>(obj);
	cobj->set(zval_user(offset), zval_user(set_value));

	//zobj_user(obj).property(offset, set_value);
}

int   
Config::has_dimension(zend_object *object, zval *offset, int check_empty)
{
	return zobj_user(object).has_property(offset);
}

void   
Config::unset_dimension(zend_object *object, zval *unset)
{
	return zobj_user(object).unset_property(unset);
}
#endif

/** The source for this in spl_array.c is so complicated, and so likely justified in its context,
 *  that a simplification of cases is attempted here, as this seems to be a common subroutine.
 *  For instance, not checking for inheritance, and no additional ce_name.
 */
/*
zval* Config::dimension_ptr(
	bool check_inherited, 
	zval *offset, int type)  
{
	HashTable* ht = cfg_.ptr();

	if (!offset || Z_ISUNDEF_P(offset) || !ht) {
		return &EG(uninitialized_zval);
	}

	bool for_write = (type == BP_VAR_W || type == BP_VAR_RW);

	if (get_hash_key(&key, intern, offset) == FAILURE) {
		zend_illegal_container_offset(ce_name, offset, type);
		return (type == BP_VAR_W || type == BP_VAR_RW) ?
			&EG(error_zval) : &EG(uninitialized_zval);
	}

	if (key.key) {
		retval = zend_hash_find(ht, key.key);
		if (retval) {
			if (Z_TYPE_P(retval) == IS_INDIRECT) {
				retval = Z_INDIRECT_P(retval);
				if (Z_TYPE_P(retval) == IS_UNDEF) {
					switch (type) {
						case BP_VAR_R:
							zend_error(E_WARNING, "Undefined array key \"%s\"", ZSTR_VAL(key.key));
							ZEND_FALLTHROUGH;
						case BP_VAR_UNSET:
						case BP_VAR_IS:
							retval = &EG(uninitialized_zval);
							break;
						case BP_VAR_RW:
							zend_error(E_WARNING,"Undefined array key \"%s\"", ZSTR_VAL(key.key));
							ZEND_FALLTHROUGH;
						case BP_VAR_W: {
							ZVAL_NULL(retval);
						}
					}
				}
			}
		} else {
			switch (type) {
				case BP_VAR_R:
					zend_error(E_WARNING, "Undefined array key \"%s\"", ZSTR_VAL(key.key));
					ZEND_FALLTHROUGH;
				case BP_VAR_UNSET:
				case BP_VAR_IS:
					retval = &EG(uninitialized_zval);
					break;
				case BP_VAR_RW:
					zend_error(E_WARNING,"Undefined array key \"%s\"", ZSTR_VAL(key.key));
					ZEND_FALLTHROUGH;
				case BP_VAR_W: {
				    zval value;
					ZVAL_NULL(&value);
				    retval = zend_hash_update(ht, key.key, &value);
				}
			}
		}
		spl_hash_key_release(&key);
	} else {
		if ((retval = zend_hash_index_find(ht, key.h)) == NULL) {
			switch (type) {
				case BP_VAR_R:
					zend_error(E_WARNING, "Undefined array key " ZEND_LONG_FMT, key.h);
					ZEND_FALLTHROUGH;
				case BP_VAR_UNSET:
				case BP_VAR_IS:
					retval = &EG(uninitialized_zval);
					break;
				case BP_VAR_RW:
					zend_error(E_WARNING, "Undefined array key " ZEND_LONG_FMT, key.h);
					ZEND_FALLTHROUGH;
				case BP_VAR_W: {
				    zval value;
					ZVAL_NULL(&value);
					retval = zend_hash_index_update(ht, key.h, &value);
			   }
			}
		}
	}
	return retval;
} /* }}} */

/*
zval*  
Config::get_property_ptr_ptr(zend_object *object, zend_string *name, int type, void **cache_slot)
{
	if (!zend_std_has_property(object, name, ZEND_PROPERTY_EXISTS, NULL)) {
		/* If object has offsetGet() overridden, then fallback to read_property,
		 * which will call offsetGet(). //
		zval member;
		/** if (intern->fptr_offset_get) {
			return NULL; // spl_array stores any child overriding implementation of offset_get.
		} //
		ZVAL_STR(&member, name);
		return spl_array_get_dimension_ptr(1, intern, object->ce->name, &member, type);
	}
	return zend_std_get_property_ptr_ptr(object, name, type, cache_slot);
}


HashTable* 
Config::get_gc(zend_object *obj, zval **gc_data, int *gc_data_count)
{

}



zval* 
Config::read_dimension(zend_object* obj, zval* offset, int type,  zval* return_value)
{

	zend_printf("read_dim type %d ", type);
	showmem("ret val", return_value);
	showmem("offset", offset);

	Wcc_Config* cobj = zobj_toc<Config>(obj);
	zval* result = cobj->get(zval_user(offset));

	if ((type == BP_VAR_W || type == BP_VAR_RW || type == BP_VAR_UNSET) &&
	    !Z_ISREF_P(result) &&
	    EXPECTED(result != &EG(uninitialized_zval))) {
			ZVAL_NEW_REF(result, result);
	}
	return result;
}

void  
Config::write_dimension(zend_object* obj, zval* offset,  zval* set_value)
{
	zend_printf("write_dim ");
	showmem("offset", offset);
	showmem("set val", set_value);
	Wcc_Config* cobj = zobj_toc<Config>(obj);
	cobj->set(zval_user(offset), zval_user(set_value));
}


void  
Config::unset_dimension(zend_object *obj, zval *unset)
{
	Wcc_Config* cobj = zobj_toc<Config>(obj);
	cobj->unset(zval_user(unset));
}


int   
Config::has_dimension(zend_object *obj, zval *offset, int check_empty)
{
	zend_printf("has_dim ");
	showmem("offset", offset);
	Wcc_Config* cobj = zobj_toc<Config>(obj);
	return cobj->has(zval_user(offset));
}

ZEND_RESULT_CODE 
Config::count_elements(zend_object *obj, zend_long *count)
{
	Wcc_Config* cobj = zobj_toc<Config>(obj);
	*count =  cobj->count();
	return SUCCESS;
}
*/

zobj_mgr // static
Config::make(htab_read initdata)
{
	zobj_mgr result = Config::omg.new_zobj();
	Config* cobj = zobj_toc<Config>(result);

	cobj->construct(initdata);
	return result;
}

void 
Config::debug_info(htab_write hw)
{
	base_d::debug_info(hw);
}

void 
Config::construct(htab_read values)
{
	if (values.size() > 0)
	{
		zobj_user self(this->vobj());
		for_key_value wk;

		for(wk.start(values); wk.ok(); wk.next())
		{
			set(wk.key(),wk.value());
		}
	}
}


zval_mgr 
Config::getOrNot(zstr_user name, zval_user ifnot)
{
	zval_mgr result;

	zobj_user self(this->vobj());

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
	if (zval_user(result).isNull() && !ifnot.isNull())
	{
		result = ifnot;
	}
	return result;
}

bool      
Config::has(zstr_user name)
{
	zobj_user self(this->vobj());

	return self.has_property(name);
}

zval_mgr
Config::get(zstr_user name)
{
	zobj_user self(this->vobj());

	return self.property(name);
}


///zend_std_unset_property
void   
Config::unset(zstr_user  name)
{
	zend_std_unset_property(this->vobj(), name, nullptr);
}

void      
Config::set(zstr_user  name, zval_user value)
{
	//showstr("set zstr_user", name);
	zobj_user(this->vobj()).property(name, value);
}

#ifdef CONFIG_DIMENSIONS

bool      
Config::has(zval_user key)
{
	zstr_mgr skey = key.to_zstr();
	return has(skey);
}

void      
Config::set(zval_user  key,  zval_user value)
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
Config::get(zval_user name)
{
	zstr_mgr skey = name.to_zstr();
	//showstr("config get", skey);

	zval_mgr result = zobj_user(this->vobj()).property(skey);
	//showmem("config get", result);
	return result;

}

void      
Config::unset(zval_user  key)
{
	zstr_mgr skey = key.to_zstr();
	zobj_user(this->vobj()).unset_property(skey);
}
#endif


htab_mgr  
Config::subsetkey(zstr_user key)
{

	htab_mgr result;
	htab_write hw(result);
	zval_mgr value = get(key);
	hw.set(key, value);

	return result;
}

htab_mgr  
Config::subset(htab_read data)
{
	for_key_value wk;
	htab_mgr result;
	htab_write hw(result);
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
Config::addArray(htab_read data)
{
	for_key_value fkv;
	for(fkv.start(data); fkv.ok(); fkv.next())
	{
		set(fkv.key(), fkv.value());
	}
}

htab_mgr  
Config::toArray()
{
	htab_mgr result;

	HashTable* ht = zend_std_get_properties_for(this->vobj(), ZEND_PROP_PURPOSE_GET_OBJECT_VARS);
	result.adopt(ht);
	return result;
}

void
Config::clear()
{
	htab_mgr temp = toArray();

	htab_read look(temp);

	for_key_value wk;

	zobj_user self(vobj());

	for(wk.start(look); wk.ok(); wk.next())
	{
		self.unset_property(wk.key());
	}
}

zstr_mgr 
Config::unhive(zstr_user subj)
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

		for(int i = 0; i < ct; i++)
		{
			htab_read  k1 = keys_list.get(i);
			zval_user fkey = k1.get((int)0);
			//showmem("get key", fkey);
			
			zval_mgr rval = this->get(fkey);
			//showmem("replace value", rval);
			zstr_user replace_str = zval_user(rval).zstr();

			htab_read f1 = replace_list.get(i);
			zstr_user  slen_f1 = f1.get((int)0);
			zval_user  soffset_f1 = f1.get(1);

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
		return zstr_mgr(subj);
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

	htab_read arg1;

	if (data) {
		arg1 = zval_user(data).zarray();
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
	zval_user temp;

	if (ifnot) {
		temp = ifnot;
	}
	zval_mgr result = cobj->getOrNot(key, temp);
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

	zval_mgr temp = cobj->get(key);
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

	htab_mgr result = cobj->toArray();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Config, unhive)
{
	zend_string* trans;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(trans)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Config>(ZEND_THIS);

	zstr_mgr result = cobj->unhive(trans);
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

	zval_mgr temp = cobj->get(zval_user(key));
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
	cobj->set(zval_user(key), zval_user(value));
}

ZEND_METHOD(Wcc_Config, offsetExists)
{
	zval* key;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Config>(ZEND_THIS);

	bool temp = cobj->has(zval_user(key));
	RETURN_BOOL(temp);
}


ZEND_METHOD(Wcc_Config, offsetUnset)
{
	zval* key;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Config>(ZEND_THIS);

	cobj->unset(zval_user(key));
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