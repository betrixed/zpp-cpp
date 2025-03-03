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


zval_mgr 
ICache::make_cache( zval_user options, zval_user services)
{
	//showmem("options", options);
	//return zval_mgr();
    htab_ptr opt(options);
    zval_mgr result;

    zstr_mgr aclass = opt[wis->classkey];

    if (aclass.isNull()) {
        zend_throw_exception(zend_ce_exception, "ICache::make_cache - adapter 'class' not specified", 100);
        return result;
    }

    result = class_data.create_object(aclass);
    zobj_user test(result);

    if (test.ok()) {

        zval_mgr arg_opt(options);
        zval_mgr arg_svc(services);
        
        test.call(STAB.construct_key, arg_opt, arg_svc);

        if (arg_svc.isObject())
        {
            zstr_own key = opt[wis->service];
           
            if (key.size()) {
                  Wcc_Services* svc = zval_toc<Wcc_Services>(services.ptr());
                  svc->set(key, result);
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

	options_ = options.cow_array();

	zval_own  value;

	if (!options_.try_fetch("prefix", value))
	{
		prefix_ = zend_empty_string;
	}
	else {
		prefix_ = value.to_zstr();
	}

	zval_own temp;
	if (options_.try_fetch("expire", temp))
	{
		ttl_ = temp.zlong();
	}
	else {
		ttl_ = 86400;
	}
}


void ICache::debug_info(HashTable *ht)
{
	htab_ptr s(ht);

	s.set("cached", cached_);
	s.set("svc_cache", svc_cache_);
	s.set("prefix", prefix_);
	s.set("options", options_);
	s.set("services", services_);
	s.set("ttl", ttl_);
}

void ICache::addLocal(zval_user pkg)
{
	htab_ptr cached(cached_);

	ICacheData* icd = zval_toc<ICacheData>(pkg.ptr());

	cached.set(icd->getKey(), pkg);
}


bool ICache::clear()
{
	cached_.clear();
	return true;
}


bool ICache::clearPrefix(zstr_user prefix)
{
	zval_own del_array;

	del_array.init_array();
	htab_ptr  delkeys(del_array);

	htab_ptr  allkeys(cached_);
	htab_walk  htw;

	zstr_ptr spref(prefix);
	std::string_view starts_with = spref.vstr();

	auto& key = htw.key();
	for( htw.start(allkeys) ; htw.ok() ; htw.next())
	{
		zstr_ptr tempstr(key.to_zstr());
		std::string_view sv = tempstr.vstr();
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
	return cached_.unset(key);
}

int 
ICache::deleteExpired()
{
	htab_own expired = getExpired();

	int result = expired.size();

	if (result > 0)
	{
		htab_walk htw;
		auto& value = htw.value();

		for(htw.start(expired); htw.ok(); htw.next())
		{
			cached_.unset(value);
		}
	}
	return result;
}


htab_own ICache::getExpired()
{
	int result = 0;

	htab_own rtab;

	htab_walk htw;
	auto& value = htw.value();
	auto& key = htw.key();

	int now = time(nullptr);

	for(htw.start(cached_); htw.ok(); htw.next())
	{
		if (value.isObject())
		{
			ICacheData* pkg = zval_toc<ICacheData>(value.ptr());
			if (pkg->getExpiry() < now)
			{
				rtab.push_back(key);
			}
		}
	}

	return rtab;

}

bool ICache::deleteMultiple(zval_user keys)
{
	bool result = true;
	htab_ptr dlist(keys);

	htab_walk htw;
	auto& key = htw.key();
	for(htw.start(dlist); htw.ok(); htw.next())
	{
		if (!deleteKey(key.zstr()))
		{
			result = false;
		}
	}
	return result;
}


zval_own ICache::get(zstr_user key, zval_user noval)
{


	htab_ptr cache(cached_);
	zval_user result = cache[key];
	if (result.isObject())
	{
		ICacheData* icd = zval_toc<ICacheData>(result.ptr());
		return icd->getData();
	}
	return noval;
}


zval_own ICache::getCached(zstr_user key)
{
	zval_own result;

	htab_ptr cache(cached_);

	if (cache.try_fetch(key,result))
	{
		return result;
	}
	return result;	
}


zval_own ICache::getData(zstr_user key)
{
	zval_own nullvalue;
	return get(key,nullvalue);
}


zval_own ICache::getMultiple(zval_user keys, zval_user noval)
{
	zval_own result;
	zval_own nullvalue;

	htab_ptr newtab(result);

	htab_ptr ktab(keys);

	htab_walk iter;
	auto& k = iter.key();

	for( iter.start(ktab); iter.ok(); iter.next())
	{
		zstr_user zkey = k.zstr();
		zval_own temp;
		zval* z = get(zkey, temp);
		if (z) {
			newtab.set(zkey, z);
		}
		else {
			newtab.set(zkey, noval);
		}
	}
	return result;
}



zval_own ICache::getService(zstr_user key)
{
	zval_own result;

	if (svc_cache_.try_fetch(key,result))
	{
		return result;
	}

	Wcc_Services* sobj = zobj_toc<Wcc_Services>(services_.ptr());
	result = sobj->get(key);

	if (!result.isNull())
	{
		svc_cache_.set(key, result);
	}
	return std::move(result);
}


zval_own ICache::getUnsaved()
{
	htab_own rtab;

	htab_walk htw;
	auto& value = htw.value();

	for(htw.start(cached_); htw.ok(); htw.next())
	{
		if (value.isObject())
		{
			ICacheData* pkg = zval_toc<ICacheData>(value.ptr());
			if (!pkg->isSaved())
			{
				rtab.push_back(value);
			}
		}
	}
	return rtab;
}


zval_own 
ICache::getOption(zstr_user key)
{
	zval_own result;

	options_.try_fetch(key, result);

	return result;
}

void 
ICache::setOption(zstr_user key, zval_user value)
{
	options_.set(key, value.ptr());
}

bool ICache::set(zstr_user key, zval_user data, zend_long ttl)
{
	zval_own icd = ICacheData::new_ICacheData(key, data, ttl);
	cached_.set(key, icd);

	return true;
}


zval_own ICache::setCached(zstr_user key, zval_user data, zend_long ttl)
{
	if (ttl<=0)
	{
		ttl = ttl_;
	}
	zval_own icd = ICacheData::new_ICacheData(key, data, ttl);
	cached_.set(key, icd);
	return icd;	
}


bool ICache::setMultiple(zval_user values, zend_long ttl)
{
	bool result = true;

	htab_ptr list_w(values);

	htab_walk walk;
	auto& k=walk.key();
	auto& v=walk.value();

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
	services_ = svc;
}

}; //namespace wcc

//@@@@@@@@@@@ @@@@@@@@@@@@@@@@@@@@@ @@@@@@@@@@@@@@ @@@@@@@@@@@@@@@

ZEND_METHOD(Wcc_ICache, __construct)
{
	zval* options = nullptr;
	zval* services_obj = nullptr;

	zend_class_entry* services_ce = services_mgr.classEntry();


	ZEND_PARSE_PARAMETERS_START(0, 2)
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY(options)
	Z_PARAM_OBJECT_OF_CLASS(services_obj, services_ce)
	ZEND_PARSE_PARAMETERS_END();

	zval_own options_z;
	zval_own services_z;

	if (!options)
	{
		options_z.init_array();
		options = options_z.ptr();
	}

	if (!services_obj) {
		services_z = Wcc_Services::instance();
		services_obj = services_z.ptr();
	}


	auto cobj = zval_toc<Wcc_ICache>(ZEND_THIS);
	cobj->__construct(options,services_obj);
}

ZEND_METHOD(Wcc_ICache, make_cache)
{
    zval* options = nullptr;
    zval* services_obj = nullptr;
    zend_class_entry* services_ce = services_mgr.classEntry();

    ZEND_PARSE_PARAMETERS_START(1, 2)
    Z_PARAM_ARRAY(options)
    Z_PARAM_OPTIONAL
    Z_PARAM_OBJECT_OF_CLASS(services_obj, services_ce)
    ZEND_PARSE_PARAMETERS_END();   

    zval_own cache = Wcc_ICache::make_cache(options, services_obj);
    cache.move_zv(return_value);
}


ZEND_METHOD(Wcc_ICache, addLocal)
{
	zend_class_entry* cobj_ce = icachedata_mgr.classEntry();
	zval* pkg;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_OBJECT_OF_CLASS(pkg, cobj_ce)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_ICache>(ZEND_THIS);
	zval_own data(pkg);
	cobj->addLocal(data);
}

ZEND_METHOD(Wcc_ICache, clear)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_ICache>(ZEND_THIS);
	RETURN_BOOL(cobj->clear());
}

ZEND_METHOD(Wcc_ICache, clearPrefix)
{
	zend_string* key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_ICache>(ZEND_THIS);
	RETURN_BOOL(cobj->clearPrefix(key));
}

ZEND_METHOD(Wcc_ICache, delete)
{
	zend_string* key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_ICache>(ZEND_THIS);
	RETURN_BOOL(cobj->deleteKey(key));
}

ZEND_METHOD(Wcc_ICache, deleteExpired)
{
	ZEND_PARSE_PARAMETERS_NONE();

	auto cobj = zval_toc<Wcc_ICache>(ZEND_THIS);
	RETURN_LONG(cobj->deleteExpired());
}
ZEND_METHOD(Wcc_ICache, deleteMultiple)
{
	zval* keys;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ARRAY(keys)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_ICache>(ZEND_THIS);
	zval_own w_keys(keys);

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

	zval_own   defval(noval);

	auto cobj = zval_toc<Wcc_ICache>(ZEND_THIS);
	zval_own   result = cobj->get(key, defval);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_ICache, getData)
{
	zend_string* key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_ICache>(ZEND_THIS);
	zval_own noval;

	zval_own result = cobj->get(key,noval);
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

	zval_own w_keys(keys);
	zval_own w_noval(noval);

	auto cobj = zval_toc<Wcc_ICache>(ZEND_THIS);
	zval_own result = cobj->getMultiple(w_keys, w_noval);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_ICache, getOption)
{
	zend_string* key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_ICache>(ZEND_THIS);
	zval_own result = cobj->getOption(key);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_ICache, getPrefix)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_ICache>(ZEND_THIS);
	zval_own result = cobj->getPrefix();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_ICache, getService)
{
	zend_string* key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_ICache>(ZEND_THIS);
	zval_own result = cobj->getService(key);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_ICache, getTTL)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_ICache>(ZEND_THIS);
	zend_long ttl = cobj->getTTL();
	RETURN_LONG(ttl);
}

ZEND_METHOD(Wcc_ICache, getUnsaved)
{
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_ICache>(ZEND_THIS);
	zval_own result = cobj->getUnsaved();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_ICache, getCached)
{
	zend_string* key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_ICache>(ZEND_THIS);
	zval_own result = cobj->getService(key);
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

	zval_own w_value(value);

	auto cobj = zval_toc<Wcc_ICache>(ZEND_THIS);
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

	zval_own w_value(value);

	auto cobj = zval_toc<Wcc_ICache>(ZEND_THIS);
	zval_own result = cobj->setCached(key,w_value, ttl);
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

	zval_own w_values(values);

	auto cobj = zval_toc<Wcc_ICache>(ZEND_THIS);
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

	auto cobj = zval_toc<Wcc_ICache>(ZEND_THIS);
	cobj->setOption(key, value);
}

ZEND_METHOD(Wcc_ICache, setServices)
{
	zend_class_entry* svc_ce = services_mgr.classEntry();
	zval*     svc;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_OBJECT_OF_CLASS(svc, svc_ce)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_ICache>(ZEND_THIS);
	zval_own data(svc);
	cobj->setServices(data);	
}

ZEND_METHOD(Wcc_ICache, setTTL)
{
	zend_long     ttl = 0;

	ZEND_PARSE_PARAMETERS_START(2, 3)
	Z_PARAM_LONG(ttl)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_ICache>(ZEND_THIS);
	cobj->setTTL(ttl);
}



PHP_MINIT_FUNCTION(Wcc_ICache_reg)
{
	auto ce = register_class_Wcc_ICache();

	icache_mgr.classEntry(ce);

	return SUCCESS;
}

//icache.cpp
#endif