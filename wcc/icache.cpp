#ifndef ICACHE_CPP
#define ICACHE_CPP

#ifndef ICACHE_DATA_H
#include "icachedata.h"
#endif

#ifndef ICACHE_H
#include "icache.h"
#endif


#ifndef ICACHE_ARGINFO
#define ICACHE_ARGINFO
extern "C" {
	#include "stub/icache_arginfo.h"
};
#endif

namespace wcc {

using namespace zpp;

	base_obj_mgr<ICache> ICache::omg;



obj_rc 
ICache::make_cache( val_ptr options, val_ptr services)
{
	//showmem("options", options);
	//return val_rc();
    htab_ptr opt(options);
    obj_rc  result;

    str_rc aclass = opt.get(IC_STR.class_key);

    if (aclass.isNull()) {
        zend_throw_exception(zend_ce_exception, "ICache::make_cache - adapter 'class' not specified", 100);
        return result;
    }

    result = class_data::create_object(aclass);
    obj_ptr test(result);

    if (test.ok()) {
        
        test.call(STAB.construct_key, options, services);

        if (services.isObject())
        {
            str_ptr key(opt.get(IC_STR.service_key));
           
            if (key.size()) {
                  Services* svc = zval_toc<Services>(services);
                 // val_rc temp(result);
                  svc->set(key, result);
            }
        }
    }
    return result;
}

void ICache::construct(val_ptr options, val_ptr services)
{
	if (services.isNull())
	{
		services_ = Services::instance();
	}
	else {
		services_ = services.zobject();
	}

	options_ = options.zarray();

	htab_ptr hread(options_);

	val_ptr value;

	if (!hread.try_fetch(IC_STR.prefix_key, value))
	{
		prefix_ = zend_empty_string;
	}
	else {
		prefix_ = value.to_zstr();
	}

	val_ptr temp;
	if (hread.try_fetch(IC_STR.expire_key, temp))
	{
		ttl_ = temp.zlong();
	}
	else {
		ttl_ = 86400;
	}
}



void ICache::debug_info(htab_rw s)
{
	base_d::debug_info(s);
	s.set(IC_STR.cached, cached_);
	s.set(IC_STR.svc_cache, svc_cache_);
	s.set(IC_STR.prefix_key, prefix_);
	s.set(IC_STR.options, options_);
	s.set(IC_STR.services, services_);
	s.set(IC_STR.ttl_key, (int)ttl_);
}

void ICache::addLocal(val_ptr pkg)
{
	htab_rw hw(cached_);

	ICacheData* icd = zval_toc<ICacheData>(pkg);
	hw.set(icd->getKey(), pkg);
}


bool ICache::clear()
{
	htab_rw(cached_).clear();
	return true;
}


bool ICache::clearPrefix(str_ptr prefix)
{
	htab_rc del_array;

	htab_rw  delkeys(del_array);

	htab_ptr  allkeys(cached_);
	htab_walk  htw;

	str_ptr spref(prefix);
	std::string_view starts_with = spref.vstr();

	auto key = htw.key();
	for( htw.start(allkeys) ; htw.ok() ; htw.next())
	{
		str_rc tempstr(key.to_zstr());

		std::string_view sv = str_ptr(tempstr).vstr();

		if (sv.rfind(starts_with, 0) == 0)
		{
			delkeys.push_back(key);
		}
	}
	if (delkeys.size() > 0)
	{
		deleteMultiple(del_array);
	}
	return true;
}


bool 
ICache::deleteKey(str_ptr key)
{
	return htab_rw(cached_).unset(key);
}

int 
ICache::deleteExpired()
{
	htab_rc hset = getExpired();
	htab_ptr  expired(hset);
	htab_rw cache(cached_);

	int result = expired.size();

	if (result > 0)
	{
		htab_walk htw;
		auto value = htw.value();

		for(htw.start(expired); htw.ok(); htw.next())
		{
			cache.unset(value);
		}
	}
	return result;
}


htab_rc
ICache::getExpired()
{
	htab_rc  result;
	htab_rw rtab(result);
	htab_ptr  cache(cached_);

	htab_walk htw;

	auto value = htw.value();
	auto key = htw.key();

	int now = time(nullptr);

	for(htw.start(cache); htw.ok(); htw.next())
	{
		if (value.isObject())
		{
			ICacheData* pkg = zval_toc<ICacheData>(value);
			if (pkg->getExpiry() < now)
			{
				rtab.push_back(key);
			}
		}
	}

	return result;
}

bool ICache::deleteMultiple(htab_ptr keys)
{
	bool result = true;

	htab_walk htw;
	auto key = htw.key();
	for(htw.start(keys); htw.ok(); htw.next())
	{
		if (!deleteKey(key))
		{
			result = false;
		}
	}
	return result;
}


val_rc 
ICache::get(str_ptr key, val_ptr noval)
{
	htab_ptr cache(cached_);
	val_ptr result = cache.get(key);
	if (result.isObject())
	{
		ICacheData* icd = zval_toc<ICacheData>(result);
		return icd->getData();
	}
	return noval;
}


val_rc 
ICache::getCached(str_ptr key)
{
	htab_ptr cache(cached_);

	return cache.get(key);
}


val_rc 
ICache::getData(str_ptr key)
{
	val_rc nullvalue;
	return get(key,nullvalue);
}

val_rc 
ICache::getMultiple(htab_ptr keys, val_ptr noval)
{
	val_rc result;
	val_rc nullvalue;

	htab_rw newtab(result);

	htab_walk iter;
	auto k = iter.key();

	val_rc nullval;
	for( iter.start(keys); iter.ok(); iter.next())
	{
		str_ptr zkey = k.zstr();
		if (zkey.ok())
		{
			
			val_rc  value = get(zkey, nullval);

			if (val_ptr(value).ok()) 
			{
				newtab.set(zkey, value);
			}
		}
	}
	return result;
}

val_rc 
ICache::getService(str_ptr key)
{
	val_rc result;
	val_ptr test;

	htab_rw svc(svc_cache_);

	if (svc.try_fetch(key,test))
	{
		result = test;
		return result;
	}

	Services* sobj = zobj_toc<Services>(services_);
	result = sobj->get(key);
	test = result;
	if (test.ok())
	{
		svc.set(key, result);
	}
	return result;
}


htab_rc 
ICache::getUnsaved()
{
	htab_rc rlist;
	
	htab_rw rtab(rlist);

	htab_walk htw;
	auto value = htw.value();
	
	for(htw.start(cached_); htw.ok(); htw.next())
	{
		if (value.isObject())
		{
			ICacheData* pkg = zval_toc<ICacheData>(value);
			if (!pkg->isSaved())
			{
				rtab.push_back(value);
			}
		}
	}
	return rlist;
}


val_rc 
ICache::getOption(str_ptr key)
{
	return htab_ptr(options_).get(key);
}

void 
ICache::setOption(str_ptr key, val_ptr value)
{
	htab_rw(options_).set(key, value);
}

bool 
ICache::set(str_ptr key, val_ptr data, zend_long ttl)
{
	obj_rc icd = setCached(key, data, ttl);
	return icd.ok();
}


// returns the ICacheData object
obj_rc
ICache::setCached(str_ptr key, val_ptr data, zend_long ttl)
{
	if (ttl<=0)
	{
		ttl = ttl_;
	}
	obj_rc icd = ICacheData::new_ICacheData(key, data, ttl);
	htab_rw(cached_).set(key, icd);
	return icd;	
}


bool 
ICache::setMultiple(val_ptr values, zend_long ttl)
{
	bool result = true;

	htab_ptr list_w(values);

	htab_walk walk;
	auto k=walk.key();
	auto v=walk.value();

	for(walk.start(list_w) ; walk.ok(); walk.next())
	{
		if (!set(k.zstr(), v, ttl))
		{
			result = false;
		}
	}
	return result;
}


void ICache::setServices(val_ptr svc)
{
	services_ = svc.zobject();
}

}; //namespace wcc


using namespace wcc;
using namespace zpp;

ZEND_METHOD(Wcc_ICache, __construct)
{
	zval* options = nullptr;
	zval* services_obj = nullptr;

	zend_class_entry* services_ce = Services::omg.classEntry();


	ZEND_PARSE_PARAMETERS_START(0, 2)
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY(options)
	Z_PARAM_OBJECT_OF_CLASS(services_obj, services_ce)
	ZEND_PARSE_PARAMETERS_END();

	val_rc options_z;
	val_rc services_z;

	if (!options)
	{
		options_z.empty_array();
		options = options_z;
	}

	if (!services_obj) {
		services_z = Services::instance();
		services_obj = services_z;
	}

	auto cobj = zval_toc<ICache>(ZEND_THIS);
	cobj->construct(options,services_obj);
}

ZEND_METHOD(Wcc_ICache, make_cache)
{
    zval* options = nullptr;
    zval* services_obj = nullptr;
    zend_class_entry* services_ce = Services::omg.classEntry();

    ZEND_PARSE_PARAMETERS_START(1, 2)
    Z_PARAM_ARRAY(options)
    Z_PARAM_OPTIONAL
    Z_PARAM_OBJECT_OF_CLASS(services_obj, services_ce)
    ZEND_PARSE_PARAMETERS_END();   

    obj_rc cache = ICache::make_cache(options, services_obj);
    cache.move_zv(return_value);
}


ZEND_METHOD(Wcc_ICache, addLocal)
{
	zend_class_entry* cobj_ce = ICacheData::omg.classEntry();
	zval* pkg;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_OBJECT_OF_CLASS(pkg, cobj_ce)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<ICache>(ZEND_THIS);
	val_rc data(pkg);
	cobj->addLocal(data);
}

ZEND_METHOD(Wcc_ICache, clear)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<ICache>(ZEND_THIS);
	RETURN_BOOL(cobj->clear());
}

ZEND_METHOD(Wcc_ICache, clearPrefix)
{
	zend_string* key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<ICache>(ZEND_THIS);
	RETURN_BOOL(cobj->clearPrefix(key));
}

ZEND_METHOD(Wcc_ICache, delete)
{
	zend_string* key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<ICache>(ZEND_THIS);
	RETURN_BOOL(cobj->deleteKey(key));
}

ZEND_METHOD(Wcc_ICache, deleteExpired)
{
	ZEND_PARSE_PARAMETERS_NONE();

	auto cobj = zval_toc<ICache>(ZEND_THIS);
	RETURN_LONG(cobj->deleteExpired());
}
ZEND_METHOD(Wcc_ICache, deleteMultiple)
{
	zval* keys;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ARRAY(keys)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<ICache>(ZEND_THIS);
	val_rc w_keys(keys);

	RETURN_BOOL(cobj->deleteMultiple(w_keys));
}

ZEND_METHOD(Wcc_ICache, get)
{
	zend_string* key;
	zval*        noval = nullptr;

	ZEND_PARSE_PARAMETERS_START(1, 2)
	Z_PARAM_STR(key)
	Z_PARAM_OPTIONAL
	Z_PARAM_ZVAL(noval)
	ZEND_PARSE_PARAMETERS_END();

	val_rc   defval(noval);

	auto cobj = zval_toc<ICache>(ZEND_THIS);
	val_rc   result = cobj->get(key, defval);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_ICache, getData)
{
	zend_string* key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<ICache>(ZEND_THIS);
	val_rc noval;

	val_rc result = cobj->get(key,noval);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_ICache, getMultiple)
{
	zval* keys;
	zval* noval = nullptr;

	ZEND_PARSE_PARAMETERS_START(1, 2)
	Z_PARAM_ARRAY(keys)
	Z_PARAM_OPTIONAL
	Z_PARAM_ZVAL(noval)
	ZEND_PARSE_PARAMETERS_END();

	val_rc w_keys(keys);
	val_rc w_noval(noval);

	auto cobj = zval_toc<ICache>(ZEND_THIS);
	val_rc result = cobj->getMultiple(w_keys, w_noval);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_ICache, getOption)
{
	zend_string* key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<ICache>(ZEND_THIS);
	val_rc result = cobj->getOption(key);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_ICache, getPrefix)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<ICache>(ZEND_THIS);
	str_ptr result = cobj->getPrefix();
	result.copy_zv(return_value);
}

ZEND_METHOD(Wcc_ICache, getService)
{
	zend_string* key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<ICache>(ZEND_THIS);
	val_rc result = cobj->getService(key);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_ICache, getTTL)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<ICache>(ZEND_THIS);
	zend_long ttl = cobj->getTTL();
	RETURN_LONG(ttl);
}

ZEND_METHOD(Wcc_ICache, getUnsaved)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<ICache>(ZEND_THIS);
	htab_rc result = cobj->getUnsaved();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_ICache, getCached)
{
	zend_string* key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<ICache>(ZEND_THIS);
	val_rc result = cobj->getCached(key);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_ICache, set)
{
	zend_string* key;
	zval*        value;
	zend_long    ttl = 0;

	ZEND_PARSE_PARAMETERS_START(2, 3)
	Z_PARAM_STR(key)
	Z_PARAM_ZVAL(value)
	Z_PARAM_OPTIONAL
	Z_PARAM_LONG(ttl)
	ZEND_PARSE_PARAMETERS_END();

	val_rc w_value(value);

	auto cobj = zval_toc<ICache>(ZEND_THIS);
	bool result = cobj->set(key,w_value, ttl);
	RETURN_BOOL(result);
}

ZEND_METHOD(Wcc_ICache, setCached)
{
	zend_string* key;
	zval*        value;
	zend_long     ttl = 0;

	ZEND_PARSE_PARAMETERS_START(2, 3)
	Z_PARAM_STR(key)
	Z_PARAM_ZVAL(value)
	Z_PARAM_OPTIONAL
	Z_PARAM_LONG(ttl)
	ZEND_PARSE_PARAMETERS_END();

	val_rc w_value(value);

	auto cobj = zval_toc<ICache>(ZEND_THIS);
	obj_rc result = cobj->setCached(key,w_value, ttl);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_ICache, setMultiple)
{
	zval*        values;
	zend_long     ttl = 0;

	ZEND_PARSE_PARAMETERS_START(2, 3)
	Z_PARAM_ZVAL(values)
	Z_PARAM_OPTIONAL
	Z_PARAM_LONG(ttl)
	ZEND_PARSE_PARAMETERS_END();

	val_rc w_values(values);

	auto cobj = zval_toc<ICache>(ZEND_THIS);
	bool result = cobj->setMultiple(w_values, ttl);
	RETURN_BOOL(result);
}

ZEND_METHOD(Wcc_ICache, setOption)
{
	zend_string* key;
	zval*        value;

	ZEND_PARSE_PARAMETERS_START(2, 2)
	Z_PARAM_STR(key)
	Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<ICache>(ZEND_THIS);
	cobj->setOption(key, value);
}

ZEND_METHOD(Wcc_ICache, setServices)
{
	zend_class_entry* svc_ce = Services::omg.classEntry();
	zval*     svc;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_OBJECT_OF_CLASS(svc, svc_ce)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<ICache>(ZEND_THIS);
	val_rc data(svc);
	cobj->setServices(data);	
}

ZEND_METHOD(Wcc_ICache, setTTL)
{
	zend_long     ttl = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_LONG(ttl)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<ICache>(ZEND_THIS);
	cobj->setTTL(ttl);
}



PHP_MINIT_FUNCTION(Wcc_ICache_reg)
{
	auto ce = register_class_Wcc_ICache();

	ICache::omg.classEntry(ce);

	return SUCCESS;
}

//icache.cpp
#endif