#ifndef ICACHE_CPP
#define ICACHE_CPP

#ifndef ICACHE_DATA_H
#include "icachedata.h"
#endif

#ifndef ICACHE_H
#include "icache.h"
#endif

#ifndef WCC_SERVICES_H
#include "services.h"
#endif

#ifndef ICACHE_ARGINFO_H
#define ICACHE_ARGINFO_H
extern "C" {
	#include "icache_arginfo.h"
};
#endif

namespace wcc {
	base_obj_mgr<ICache> ICache::omg;

using namespace zpp;

zobj_mgr 
ICache::make_cache( zval_user options, zval_user services)
{
	//showmem("options", options);
	//return zval_mgr();
    htab_read opt(options);
    zobj_mgr  result;

    zstr_mgr aclass = opt.get(IC_STR.class_key);

    if (aclass.isNull()) {
        zend_throw_exception(zend_ce_exception, "ICache::make_cache - adapter 'class' not specified", 100);
        return result;
    }

    result = class_data::create_object(aclass);
    zobj_user test(result);

    if (test.ok()) {
        
        test.call(STAB.construct_key, options, services);

        if (services.isObject())
        {
            zstr_user key(opt.get(IC_STR.service_key));
           
            if (key.size()) {
                  Services* svc = zval_toc<Services>(services);
                  zval_mgr temp(result);
                  svc->set(key, temp);
            }
        }
    }
    return result;
}

void ICache::__construct(zval_user options, zval_user services)
{
	if (services.isNull())
	{
		services_ = Services::instance();
	}
	else {
		services_ = services.zobject();
	}

	options_ = options.zarray();

	htab_read hread(options_);

	zval_user value;

	if (!hread.try_fetch(IC_STR.prefix_key, value))
	{
		prefix_ = zend_empty_string;
	}
	else {
		prefix_ = value.to_zstr();
	}

	zval_user temp;
	if (hread.try_fetch(IC_STR.expire_key, temp))
	{
		ttl_ = temp.zlong();
	}
	else {
		ttl_ = 86400;
	}
}

	zstr_intern expiry_key;
	zstr_intern expire_key;
	zstr_intern prefix_key;
	zstr_intern key_key;
	zstr_intern data_key;
	zstr_intern ttl_key;
	zstr_intern stored_key;
	zstr_intern saved;
	zstr_intern class_key;
	zstr_intern service_key;



void ICache::debug_info(htab_write s)
{
	s.set(IC_STR.cached, cached_);
	s.set(IC_STR.svc_cache, svc_cache_);
	s.set(IC_STR.prefix_key, prefix_);
	s.set(IC_STR.options, options_);
	s.set(IC_STR.services, services_);
}

void ICache::addLocal(zval_user pkg)
{
	htab_write hw(cached_);

	ICacheData* icd = zval_toc<ICacheData>(pkg);

	hw.set(icd->getKey(), pkg);
}


bool ICache::clear()
{
	htab_write(cached_).clear();
	return true;
}


bool ICache::clearPrefix(zstr_user prefix)
{
	htab_mgr del_array;

	htab_write  delkeys(del_array);

	htab_read  allkeys(cached_);
	htab_walk  htw;

	zstr_user spref(prefix);
	std::string_view starts_with = spref.vstr();

	auto key = htw.key();
	for( htw.start(allkeys) ; htw.ok() ; htw.next())
	{
		zstr_mgr tempstr(key.to_zstr());

		std::string_view sv = zstr_user(tempstr).vstr();

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
ICache::deleteKey(zstr_user key)
{
	return htab_write(cached_).unset(key);
}

int 
ICache::deleteExpired()
{
	htab_mgr hset = getExpired();
	htab_read  expired(hset);
	htab_write cache(cached_);

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


htab_mgr
ICache::getExpired()
{
	htab_mgr  result;
	htab_write rtab(result);
	htab_read  cache(cached_);

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

bool ICache::deleteMultiple(htab_read keys)
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


zval_mgr 
ICache::get(zstr_user key, zval_user noval)
{
	htab_read cache(cached_);
	zval_user result = cache.get(key);
	if (result.isObject())
	{
		ICacheData* icd = zval_toc<ICacheData>(result);
		return icd->getData();
	}
	return noval;
}


zval_mgr 
ICache::getCached(zstr_user key)
{
	htab_read cache(cached_);

	return cache.get(key);
}


zval_mgr 
ICache::getData(zstr_user key)
{
	zval_mgr nullvalue;
	return get(key,nullvalue);
}

zval_mgr 
ICache::getMultiple(htab_read keys, zval_user noval)
{
	zval_mgr result;
	zval_mgr nullvalue;

	htab_write newtab(result);

	htab_walk iter;
	auto k = iter.key();

	zval_mgr nullval;
	for( iter.start(keys); iter.ok(); iter.next())
	{
		zstr_user zkey = k.zstr();
		if (zkey.ok())
		{
			
			zval_mgr  value = get(zkey, nullval);

			if (zval_user(value).ok()) 
			{
				newtab.set(zkey, value);
			}
		}
	}
	return result;
}

zval_mgr 
ICache::getService(zstr_user key)
{
	zval_mgr result;
	zval_user test;

	htab_write svc(svc_cache_);

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


htab_mgr 
ICache::getUnsaved()
{
	htab_mgr rlist;
	
	htab_write rtab(rlist);

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


zval_mgr 
ICache::getOption(zstr_user key)
{
	return htab_read(options_).get(key);
}

void 
ICache::setOption(zstr_user key, zval_user value)
{
	htab_write(options_).set(key, value);
}

bool 
ICache::set(zstr_user key, zval_user data, zend_long ttl)
{
	zobj_mgr icd = ICacheData::new_ICacheData(key, data, ttl);
	htab_write(cached_).set(key, icd);

	return true;
}


zobj_mgr
ICache::setCached(zstr_user key, zval_user data, zend_long ttl)
{
	if (ttl<=0)
	{
		ttl = ttl_;
	}
	zobj_mgr icd = ICacheData::new_ICacheData(key, data, ttl);
	htab_write(cached_).set(key, icd);
	return icd;	
}


bool 
ICache::setMultiple(zval_user values, zend_long ttl)
{
	bool result = true;

	htab_read list_w(values);

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


void ICache::setServices(zval_user svc)
{
	services_ = svc.zobject();
}

}; //namespace wcc

//@@@@@@@@@@@ @@@@@@@@@@@@@@@@@@@@@ @@@@@@@@@@@@@@ @@@@@@@@@@@@@@@

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

	zval_mgr options_z;
	zval_mgr services_z;

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
	cobj->__construct(options,services_obj);
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

    zobj_mgr cache = ICache::make_cache(options, services_obj);
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
	zval_mgr data(pkg);
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
	zval_mgr w_keys(keys);

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

	zval_mgr   defval(noval);

	auto cobj = zval_toc<ICache>(ZEND_THIS);
	zval_mgr   result = cobj->get(key, defval);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_ICache, getData)
{
	zend_string* key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<ICache>(ZEND_THIS);
	zval_mgr noval;

	zval_mgr result = cobj->get(key,noval);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_ICache, getMultiple)
{
	zval* keys;
	zval* noval;

	ZEND_PARSE_PARAMETERS_START(1, 2)
	Z_PARAM_ARRAY(keys)
	Z_PARAM_OPTIONAL
	Z_PARAM_ZVAL(noval)
	ZEND_PARSE_PARAMETERS_END();

	zval_mgr w_keys(keys);
	zval_mgr w_noval(noval);

	auto cobj = zval_toc<ICache>(ZEND_THIS);
	zval_mgr result = cobj->getMultiple(w_keys, w_noval);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_ICache, getOption)
{
	zend_string* key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<ICache>(ZEND_THIS);
	zval_mgr result = cobj->getOption(key);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_ICache, getPrefix)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<ICache>(ZEND_THIS);
	zstr_user result = cobj->getPrefix();
	result.return_zv(return_value);
}

ZEND_METHOD(Wcc_ICache, getService)
{
	zend_string* key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<ICache>(ZEND_THIS);
	zval_mgr result = cobj->getService(key);
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
	htab_mgr result = cobj->getUnsaved();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_ICache, getCached)
{
	zend_string* key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<ICache>(ZEND_THIS);
	zval_mgr result = cobj->getService(key);
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

	zval_mgr w_value(value);

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

	zval_mgr w_value(value);

	auto cobj = zval_toc<ICache>(ZEND_THIS);
	zobj_mgr result = cobj->setCached(key,w_value, ttl);
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

	zval_mgr w_values(values);

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
	zval_mgr data(svc);
	cobj->setServices(data);	
}

ZEND_METHOD(Wcc_ICache, setTTL)
{
	zend_long     ttl = 0;

	ZEND_PARSE_PARAMETERS_START(2, 3)
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