#ifndef WCC_HMAP_CPP
#define WCC_HMAP_CPP

#ifndef WCC_HMAP_H
#include "hmap.h"
#endif

extern "C" {
  #include <Zend/zend_attributes.h>
  #include <Zend/zend_interfaces.h>
  #include <Zend/zend_iterators.h>
};

#ifndef HMAP_ARGINFO_H
#define HMAP_ARGINFO_H
extern "C" {
	#include "stub/hmap_arginfo.h"
};

#endif


namespace wcc {
Hmap_mgr Hmap::omg;

using namespace zpp;


class Hmap_init : public state_init {
public:        
	str_intern data_key;
	str_intern obj_key;

	void init() override {
		data_key = "data";
		obj_key = "__obj";
	}
};


Hmap_init HMAPit;

// static function table instance
zend_object_iterator_funcs 

HmapIterator::it_fntab_ = {
	it_dtor,
	it_valid,
	it_get_data,
	it_get_key,
	it_forward,
	it_rewind,
	it_invalidate,
	nullptr
}; 


void* HmapIterator::operator new(size_t count) 
{
    //zend_printf("operator new for HmapIterator\n");
    void* p = emalloc(count);
    memset(p, 0, sizeof(zend_object_iterator));
    
    return p;
}
void HmapIterator::operator delete(void* ptr) {
    // Let PHP to call efree(ptr):
}

HmapIterator::HmapIterator() : walk(), htab()
{
}

zend_object_iterator* //static
HmapIterator::create(zend_class_entry* ce, zval* zobj, int byref)
{
        
	HmapIterator* iterator = new HmapIterator();

	zend_iterator_init((zend_object_iterator*) &iterator->phpit);
	
	zend_object* hmo = Z_OBJ_P(zobj);
        
        // bump reference holder for iterated object
	ZVAL_OBJ_COPY(&iterator->phpit.data, hmo);

	iterator->phpit.funcs = &it_fntab_;

	Hmap* cobj = zobj_toc<Hmap>(hmo);

        // refcount++ iterated source HashTable
	iterator->htab = cobj->toArray(); 
	iterator->walk.start(iterator->htab);

	return &iterator->phpit; 
}

void //static - undo creation work
HmapIterator::it_dtor(zend_object_iterator *iter)
{
	//zend_object* obj = Z_OBJ_P(&iter->data);
	//obj_rc::try_decref(obj);
	HmapIterator *iterator = phmi(iter);
        iterator->walk.init();
        iterator->htab.init();
        
	zval_ptr_dtor(&iterator->phpit.data);
        
        // leave memory freeing to zend.
}


zend_result //static
HmapIterator::it_valid(zend_object_iterator *iter)
{
	
	HmapIterator *iterator = phmi(iter);
	zend_result result = iterator->walk.ok() ? SUCCESS : FAILURE;
	return result;

}

zval * //static
HmapIterator::it_get_data(zend_object_iterator *iter)
{
	HmapIterator *iterator = phmi(iter);
	return iterator->walk.value();
}

void //static
HmapIterator::it_get_key(zend_object_iterator *iter, zval *key)
{
	HmapIterator *iterator = phmi(iter);
	ZVAL_COPY(key, iterator->walk.key());
}

void //static
HmapIterator::it_forward(zend_object_iterator *iter)
{
	HmapIterator *iterator = phmi(iter);
	iterator->walk.next();
}

void  //static
HmapIterator::it_rewind(zend_object_iterator *iter)
{
	HmapIterator *iterator = phmi(iter);
	iterator->walk.rewind();
}

void  //static
HmapIterator::it_invalidate(zend_object_iterator *iter)
{
	HmapIterator* mem = phmi(iter);
	// clean up walker, refcount--;
        // lose any reference to current key and value
        mem->walk.release();
}

zval* 
Hmap_php::get_property_ptr_ptr(zend_object* object, zend_string* name, 
				int type, void** cache_slot)
{
	//if (!zend_std_has_property(object, name, ZEND_PROPERTY_EXISTS, cache_slot)) {
		Hmap* cobj = zobj_toc<Hmap>(object);
		htab_ptr look(cobj->data_);
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
Hmap_php::read_property(zend_object* object, zend_string* name, int type, 
						void** cache_slot, zval* rv)
{
	//if (!zend_std_has_property(object, name, ZEND_PROPERTY_EXISTS, cache_slot)) 
	//{
		Hmap* cobj = zobj_toc<Hmap>(object);
		htab_ptr look(cobj->data_);

		if (look.isNull())
		{
			//zend_printf("Hmap null property %lx\n", (long unsigned int)rv);
			if (rv) { 
				ZVAL_NULL(rv);
			    return rv;
			}
			return nullptr;
		}
		zval* result = look.get(name);
		if (!result)
		{
			if (rv) { 
				ZVAL_NULL(rv);
			    return rv;
			}
		}
		return result;
	//}
	/** type value must be good for something? */
	//return zend_std_read_property(object, name, type, cache_slot, rv);
}

zval* 	
Hmap_php::write_property(zend_object* object, zend_string* name, zval* value, void** cache_slot)
{
	//if (!zend_std_has_property(object, name, ZEND_PROPERTY_EXISTS, cache_slot)) 
	//{
		Hmap* cobj = zobj_toc<Hmap>(object);
		
		htab_rw hw(cobj->data_);

		zval* result = zend_hash_update(hw, name, value);
		if (Z_TYPE_FLAGS_P(result) != 0)
		{
			val_rc::try_addref(result);
		}
		return result;
	//}
	//return zend_std_write_property(object, name, value, cache_slot);
}

int
Hmap_php::has_property(zend_object* object, zend_string* name, int has_set_exists, void **cache_slot)
{
	/*int result = zend_std_has_property(object, name, ZEND_PROPERTY_EXISTS, cache_slot);
	if (result)
	{
		return result;
	}*/

	Hmap* cobj = zobj_toc<Hmap>(object);
	htab_ptr look(cobj->data_);
	if (look.isNull())
	{
		  return false;
	}
	return look.has_key(name);
}

void  
Hmap_php::unset_property(zend_object* object, zend_string* name, void **cache_slot)
{
	/*if (zend_std_has_property(object, name, ZEND_PROPERTY_EXISTS, cache_slot))
	{
		zend_std_unset_property(object, name, cache_slot);
		return;
	}
	*/
	Hmap* cobj = zobj_toc<Hmap>(object);
	htab_rw hw(cobj->data_);
	zend_hash_del(hw, name);
}

// seems to be direct easy way to count number of properties. Maybe not needed.
ZEND_RESULT_CODE 
Hmap_php::count_elements(zend_object* object, zend_long *count)
{
	Hmap* cobj = zobj_toc<Hmap>(object);
	htab_ptr look(cobj->data_);
	*count = look.size();
	return SUCCESS;
}


/*
HashTable*
Hmap::get_properties(zend_object* object)
{
	Hmap* cobj = zobj_toc<Hmap>(object);
	htab_ptr look(cobj->data_);
	return look;
}
*/
HashTable*
Hmap_php::get_properties_for(zend_object* object, zend_prop_purpose purpose)
{
	Hmap* cobj = zobj_toc<Hmap>(object);
	htab_rc look(cobj->data_);
	switch(purpose)
	{
	case ZEND_PROP_PURPOSE_DEBUG:
		{
			htab_rc temp_mgr;
			htab_rw  di(temp_mgr);

			

			cobj->debug_info(di);
			HashTable* result = (HashTable*) temp_mgr;
			htab_rc::try_addref(result);

			return result;
		}
	case ZEND_PROP_PURPOSE_ARRAY_CAST: 	
	case ZEND_PROP_PURPOSE_SERIALIZE:
	case ZEND_PROP_PURPOSE_VAR_EXPORT:
	case ZEND_PROP_PURPOSE_JSON:
	case ZEND_PROP_PURPOSE_GET_OBJECT_VARS:	
	case _ZEND_PROP_PURPOSE_NON_EXHAUSTIVE_ENUM:

		HashTable* ht = (HashTable*) look;
		//showarray("look", ht);
		return ht;
	}
	return nullptr;
}

obj_rc // static
Hmap::newFromArray(val_ptr htab)
{
	htab_rc hold;
	if (htab.isArray())
	{
		hold = htab.zarray(); // already owned
	}
	else {
		hold.adopt(htab_rc::empty_array());// take ownership
	}

	obj_rc result = Hmap::omg.new_zobj();
	Hmap* cobj = zobj_toc<Hmap>(result);
	cobj->construct(hold); // take ownership if not empty

	return result;
}

obj_rc  // static
Hmap::new_hmap()
{
	obj_rc result = Hmap::omg.new_zobj();
	Hmap* cobj = zobj_toc<Hmap>(result);
	htab_ptr nullarray;
	cobj->construct(nullarray);
	return result;
}

zval* 
Hmap_php::read_dimension(zend_object* obj, zval* offset, int type, zval* return_value)
{
	Hmap* cobj = zobj_toc<Hmap>(obj);
	htab_ptr look(cobj->data_);
	if (look.isNull())
	{
		return nullptr;
	}
	return look.get(offset);
}

void 
Hmap_php::write_dimension(zend_object* obj, zval* offset, zval* set_value)
{
	Hmap* cobj = zobj_toc<Hmap>(obj);
	htab_rw hw(cobj->data_);
	hw.set(offset, set_value);
}

int  
Hmap_php::has_dimension(zend_object* object, zval* offset, int check_empty)
{
	Hmap* cobj = zobj_toc<Hmap>(object);
	htab_ptr look(cobj->data_);
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
		if (val_ptr(check).ok())
		{
			return 1;
		}
	}
	return 1;
}

void  
Hmap_php::unset_dimension(zend_object* object, zval* unset)
{
	Hmap* cobj = zobj_toc<Hmap>(object);
	htab_rw hw(cobj->data_);
	hw.unset(unset);
}



Hmap::Hmap() : base_d () 
{
    data_ = htab_rc::empty_array();
}

Hmap::~Hmap() 
{}

void 
Hmap::construct(htab_ptr values)
{
	//showarray("Hmap::construct", values);
	if (values.size() > 0)
	{
		(htab_rc&) data_ = values; // takes ownership
		//showdata("construct ", data_);
	}
}

val_rc 
Hmap::getOrNot(str_ptr name, val_ptr ifnot)
{
	val_ptr result;

	htab_ptr look(data_);

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
Hmap::has(str_ptr name)
{
	htab_ptr look(data_);
	return look.ok() && look.has_key(name);
}

val_rc
Hmap::get(str_ptr name)
{	
	val_rc result;
	htab_ptr look(data_);
	if (look.ok())
	{
		result = look.get(name);
	}
	return result;
}

val_rc Hmap::get(val_ptr key)
{
	val_rc result;

	htab_ptr look(data_);
	if (look.ok())
	{
		result = look.get(key);
	}
	return result;
}

///zend_std_unset_property
void  
Hmap::unset(str_ptr name)
{
	htab_rw hw(data_);
	if (hw.ok())
	{
		hw.unset(name);
	}
}

void   
Hmap::set(str_ptr name, val_ptr value)
{
	htab_rw hw(data_);
	hw.set(name, value);
}


void Hmap::debug_info(htab_rw hw)
{
	// allow derived classes to override
	base_d::debug_info(hw);
	hw.set(HMAPit.data_key, data_);
}

htab_rc 
Hmap::subsetkey(str_ptr key)
{
	htab_rc result;
	htab_rw hw(result);
	val_rc value = get(key);
	hw.set(key, value);
	return result;
}

// keys as list values in data
htab_rc 
Hmap::subset(htab_ptr data)
{
	htab_rc result;
	htab_rw hw(result);

	htab_walk wk;

	auto key = wk.value();
	for(wk.start(data); wk.ok(); wk.next())
	{
		val_ptr value = this->get(key);
		hw.set(key, value);
	}
	return result;
}

void   
Hmap::addArray(htab_ptr data)
{
	for_key_value fkv;
	htab_rw hw(data_);

	for(fkv.start(data); fkv.ok(); fkv.next())
	{
		hw.set(fkv.key(), fkv.value());
	}
}

void
Hmap::assign(htab_ptr data)
{
	(htab_rc&) data_ = data;
}

zend_long 
Hmap::count() const
{
		return htab_ptr(data_).size();
}

htab_ptr 
Hmap::toArray()
{
	return htab_ptr(data_);
}

str_rc 
Hmap::unhive(str_ptr subj)
{
	preg sfind("#@([a-zA-Z][\\w\\d]*)#", preg::OFFSET_CAPTURE, true);

	int ct = sfind.matches(subj);
	if (ct > 0) {
		htab_ptr m = sfind.results();

		htab_ptr replace_list = m.get((int)0);
		htab_ptr keys_list = m.get(1);

		std::string_view original = subj.vstr();

		str_buf result;
		size_t ipos = 0;

		htab_ptr mydata(data_);

		for(int i = 0; i < ct; i++)
		{
			htab_ptr k1 = keys_list.get(i);
			val_ptr fkey = k1.get((int)0);
			//showmem("get key", fkey);
			
			val_rc rval = mydata.get(fkey);

			//showmem("replace value", rval);
			str_ptr replace_str = val_ptr(rval).zstr();

			htab_ptr f1 = replace_list.get(i);
			str_ptr slen_f1 = f1.get((int)0);
			val_ptr soffset_f1 = f1.get(1);

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

htab_rc 
Hmap::serialize()
{
	obj_ptr self(vobj());

	htab_rc result;

	HashTable* ht = Hmap::omg.handlers_.get_properties_for(self, ZEND_PROP_PURPOSE_SERIALIZE);

	result.adopt(ht);

	return result;
}

void 
Hmap::unserialize(htab_ptr htab)
{

	(htab_rc&)data_ = htab;

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

	htab_ptr arg1;

	if (data) {
		arg1 = val_ptr(data).zarray();
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
	val_ptr temp;

	if (ifnot) {
		temp = ifnot;
	}
	val_rc result = cobj->getOrNot(key, temp);
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

	val_rc temp = cobj->get(key);
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


ZEND_METHOD(Wcc_Hmap, getIterator)
{
	ZEND_PARSE_PARAMETERS_NONE();
	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

ZEND_METHOD(Wcc_Hmap, assign)
{
	zval* elist;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(elist)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Hmap>(ZEND_THIS);

	cobj->assign(elist);

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

	htab_ptr result = cobj->toArray();
	result.copy_zv(return_value);
}

ZEND_METHOD(Wcc_Hmap, unhive)
{
	zend_string* trans;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(trans)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Hmap>(ZEND_THIS);

	str_rc result = cobj->unhive(trans);
	result.move_zv(return_value);
}


ZEND_METHOD(Wcc_Hmap, offsetGet)
{
	zval* key;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Hmap>(ZEND_THIS);

	val_rc temp = cobj->get(val_ptr(key));
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
	cobj->set(val_ptr(key), val_ptr(value));
}

ZEND_METHOD(Wcc_Hmap, offsetExists)
{
	zval* key;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Hmap>(ZEND_THIS);

	bool temp = cobj->has(val_ptr(key));
	RETURN_BOOL(temp);
}


ZEND_METHOD(Wcc_Hmap, offsetUnset)
{
	zval* key;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ZVAL(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Hmap>(ZEND_THIS);

	cobj->unset(val_ptr(key));
}


ZEND_METHOD(Wcc_Hmap, count)
{
	ZEND_PARSE_PARAMETERS_START(0,0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Hmap>(ZEND_THIS);

	RETURN_LONG(cobj->count());
}

ZEND_METHOD(Wcc_Hmap, __serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Hmap* cobj = zval_toc<Hmap>(ZEND_THIS);

	htab_rc ret = cobj->serialize();
	ret.move_zv(return_value);
}

ZEND_METHOD(Wcc_Hmap, __unserialize)
{
	zval* data;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ARRAY(data)
	ZEND_PARSE_PARAMETERS_END();

	htab_ptr hw(Z_ARR_P(data));

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
/*
ZEND_METHOD(Wcc_Hmap, __wakeup)
{
		ZEND_PARSE_PARAMETERS_NONE();

}
*/

PHP_MINIT_FUNCTION(Wcc_Hmap_reg)
{
	//auto ce = register_class_Wcc_Hmap(zend_ce_arrayaccess, zend_ce_countable);
	//zend_standard_class_def
	auto ce = register_class_Wcc_Hmap(
		//zend_ce_iterator
		//zend_ce_arrayaccess, 
		//zend_ce_countable
		);

	Hmap::omg.classEntry(ce);

	//zend_printf("registered hmap\n");
	return SUCCESS;
}

//Wcc_Hmap.cpp
#endif