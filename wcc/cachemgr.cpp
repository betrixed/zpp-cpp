#ifndef WCC_CACHEMGR_CPP
#define WCC_CACHEMGR_CPP

#ifndef WCC_CACHEMGR_H
#include "cachemgr.h"
#endif

#ifndef WCC_FINDER_H
#include "finder.h"
#endif

#ifndef REFLECT_CACHE_H
#include "reflect_cache.h"
#endif

#ifndef TOML_PHP_H
#include "toml/toml_php.h"
#endif

#ifndef WCC_SERVICES_H
#include "services.h"
#endif

#ifndef WCC_LOADER_H
#include "loader.h"
#endif

#ifndef CACHEMGR_ARGINFO_H
#define CACHEMGR_ARGINFO_H

extern "C" {
	#include "stub/cachemgr_arginfo.h"
};

#endif

namespace wcc {


base_obj_mgr<CacheMgr> CacheMgr::omg;

CacheMgr_init Cache_i;



void CacheMgr_init::init()
{
	defaults_str = "defaults";
	fast_cache = "fast_cache";
	del_expired = "delete_expired";
	expiry_key = "expiry_key";
	expired_check = "expired_check";

	default_cache = "default_cache";
	file_cache = "file_cache";
	cache_mgr = "cache_mgr";

	s_cache_obj = "cache_obj";
	s_cache_defaults = "cache_defaults";
	s_default_cache = "default_cache";
	s_loader = "loader",

	s_set = "set";
	s_get = "get";
	s_delete = "delete";
	s_getcached = "getcached";
	s_setoption = "setoption";

	s_flushcached = "flushcached";
	s_delete_expired = "deleteexpired";
	s_clear_str = "clear";
	defer_write = "defer_write";

	php_ext = "php";
	toml_ext = "toml";
	xml_ext = "xml";
	// The loader filesystem is case-sensitive
	xmlread_c = "\\Wcc\\XmlRead::fromFile";
}

void CacheMgr::construct(htab_ptr cfg)
{
	xml_call_.set_fname(Cache_i.xmlread_c);
	this->init(cfg);
}


val_rc //static
CacheMgr::callStatic(str_ptr name, htab_ptr args)
{
	str_rc cache_name = name.uncamel();

	obj_rc cache_mgr = Services::service(Cache_i.cache_mgr);

	CacheMgr* cm = zobj_toc<CacheMgr>(cache_mgr);
	
	return cm->readCache(args.get(int(0)), cache_name);
}

void CacheMgr::debug_info(htab_rw di)
{
	di.set(Cache_i.s_cache_obj, cache_obj_);

	di.set(Cache_i.s_cache_defaults, cache_defaults_);

	di.set(Cache_i.fast_cache, fast_cache_);

	di.set(Cache_i.s_default_cache, default_cache_);

	di.set(Cache_i.expiry_key, expiry_key_);

	di.set(Cache_i.del_expired, delete_expired_);

}

Loader* CacheMgr::getLoader()
{
	if (!loader_.ok())
	{
		Services* svc = Services::cpp_global();

		loader_ = svc->get(Cache_i.s_loader);
	}
	return zobj_toc<Loader>(loader_);
}

void CacheMgr::init(htab_ptr cfg)
{
	//showdata("CacheMgr init", cfg);

	cache_defaults_ = cfg.get(Cache_i.defaults_str);
	fast_cache_ = cfg.get(Cache_i.fast_cache);
	if (!fast_cache_.ok())
	{
		fast_cache_ = Cache_i.file_cache;
	}
	val_ptr expiry = cfg.get(Cache_i.del_expired);
	if (expiry.isLong())
	{
		delete_expired_ = expiry.zlong();
	}
	else {
		delete_expired_ = 60*10;
	}
	expiry_key_ = cfg.get(Cache_i.expiry_key);
	if (!expiry_key_.ok()) 
	{
		expiry_key_ = Cache_i.expired_check;
	}
	default_cache_ = cfg.get(Cache_i.default_cache);
	if (!default_cache_.ok())
	{
		default_cache_ = Cache_i.file_cache;
	}
}

void CacheMgr::destruct()
{
	cache_obj_.init();
}

void CacheMgr::clearAll()
{
	htab_walk wk;

	fn_call fnclear;

	fnclear.set_fname(Cache_i.s_clear_str);

	auto cache = wk.value();
	for(wk.start(cache_obj_); wk.ok(); wk.next())
	{
		obj_ptr obj = cache.zobject();
		fnclear.set_obj(obj);
		fnclear.call_fn();
	}
}


void CacheMgr::createCache(str_ptr svckey, str_ptr classname, htab_ptr options)
{

	Services* svc = Services::cpp_global();
	if (svc->has(svckey)) {
		zend_throw_error(zend_ce_error, "createCache: service key %s exists", svckey.data());
		return;
	}

	
	//showdata("config_args a", cache_defaults_);
	htab_rc config_args = cache_defaults_;
	//showdata("config_args b", config_args);

	htab_rw config(config_args);

	//showdata("config_write c", config_args);
	config.merge(options);

	//showdata("config_args", config_args);
	
	htab_rc args_cache;
	htab_rw acwrite(args_cache);

	acwrite.push_back(config_args);
	acwrite.push_back(svc->self());

	//showdata("args_cache", args_cache);

	obj_rc obj = ReflectCache::staticInstanceArgs(classname, args_cache);

	htab_rw hw(cache_obj_);
	hw.set(svckey, obj);
	svc->set(svckey, obj);
}

void CacheMgr::deleteExpired()
{
	if (fast_cache_.ok())
	{
		obj_ptr fc = cache_obj_.get(fast_cache_);

		if (fc.ok())
		{
			val_rc key(expiry_key_);
			
			val_rc check = fc.call(Cache_i.s_get, key);
			if (!check.ok())
			{
				htab_walk wk;

				fn_call delete_expired;

				delete_expired.set_fname(Cache_i.s_delete_expired);

				auto cache = wk.value();

				for(wk.start(cache_obj_); wk.ok(); wk.next())
				{
					obj_ptr obj = cache.zobject();
					delete_expired.set_obj(obj);
					delete_expired.call_fn();
				}

				val_rc ttl(delete_expired_);

				fc.call(Cache_i.s_set, ttl, ttl);
			}
		}
	}
}

obj_ptr 
CacheMgr::getCache(str_ptr svckey)
{
	obj_ptr cache = cache_obj_.get(svckey);
	if (!cache.ok())
	{
		cache = cache_obj_.get(default_cache_);
	}
	return cache;
}

str_ptr 
CacheMgr::getCacheClass(str_ptr svckey)
{
	str_ptr result;

	obj_ptr cache = cache_obj_.get(svckey);
	if (cache.ok())
	{
		result = cache.className();
	}
	else {
		zend_throw_error(zend_ce_error, "getCacheClass: key %s not found", svckey.data());
	}
	return result;

}

htab_rc
CacheMgr::getCacheKeys()
{
	return htab_rc::getKeys(cache_obj_);
}

val_rc 
CacheMgr::readCache(str_ptr filename, str_ptr cachename)
{
	val_rc result;

	if (!file_exists(filename)) {
		zend_throw_error(zend_ce_error, "File '%s' not found: ", filename.data());
		return result;
	}

	obj_ptr cache = cache_obj_.get(cachename);
	if (!cache.ok())
	{
		cache = cache_obj_.get(default_cache_);
		if (!cache.ok())
		{
			zend_throw_error(zend_ce_error, "ICache '%s' not found: ", default_cache_.data());
			return result;
		}
	}
	val_rc key = filename;
	obj_rc pkg = cache.call(Cache_i.s_getcached, key);
	if (pkg.ok())
	{
		long mtime = filemtime(filename);
		ICacheData* icd = zobj_toc<ICacheData>(pkg);
		if (mtime > icd->getStored() + 3) 
		{
			cache.call(Cache_i.s_delete, key);
		}
		else {
			return icd->getData();
		}
	}
	// data missing or not current
	val_rc data = readFile( filename );
	//showmem("read data", data);

	if (data.ok())
	{
		if (!cache.call(Cache_i.s_set, key, data))
		{
			zend_throw_error(zend_ce_error, "Failed to set data from %s", filename.data());
			return result;
		}
	}
	else {
		zend_throw_error(zend_ce_error, "Failed to read data from %s", filename.data());
		return result;
	}
	result = std::move(data);
	return result;
}

void 
CacheMgr::flush_caches()
{
	htab_walk wk;
	auto cache = wk.value();
	for(wk.start(cache_obj_); wk.ok(); wk.next())
	{
		obj_ptr obj = cache.zobject();

		obj.call(Cache_i.s_flushcached);

		val_rc arg1(Cache_i.defer_write);
		val_rc arg2(false);
		obj.call(Cache_i.s_setoption, arg1, arg2);
	}
}


val_rc //static
CacheMgr::readFile(str_ptr filename, str_ptr ext)
{
	str_rc filetype;
	val_rc result;

	//showstr("readFile ", filename);

	if (!ext.ok())
	{
		
		filetype = Finder::path_ext(filename);
		//filetype = FTAB.pathinfo.call(filename, PathInfo::EXTENSION);
		//showstr("pathinfo", filetype);
	}
	else {
		filetype = ext;
	}

	if (!filename.ok() || !filetype.ok())
	{
		zend_throw_error(zend_ce_error,"CacheMgr::readFile bad filename");
		return result;
	}
	

	if (zs_cmp_ci(filetype,Cache_i.xml_ext)==0)
	{

		obj_rc cache_mgr = Services::service(Cache_i.cache_mgr);
		CacheMgr* cm = zobj_toc<CacheMgr>(cache_mgr);
		auto& fn = cm->xml_call_;

		zval* p = fn.argsptr();
		ZVAL_STR(p, filename);
		result = fn.call_fn();

	}
	else if (zs_cmp_ci(filetype, Cache_i.php_ext)==0)
	{
		//showstr("match php ", filename);
		result = Loader::readPHP(filename);
	}
	else if (zs_cmp_ci(filetype, Cache_i.toml_ext)==0)
	{
		result = val_rc(Toml::decodeFile(filename));
	}
	else {
		zend_throw_error(zend_ce_error,"Unmatched file extension %s", filetype.data());
	}
	
	return result;
}

};
// end namespace wcc;


using namespace wcc;
using namespace zpp;

ZEND_METHOD(Wcc_CacheMgr, __construct)
{
	zarg_rd args(execute_data);

	htab_ptr cfg;

	args.zarray(cfg, args.need(0));

	if (!args.throw_errors())
	{
		CacheMgr*  cobj = zval_toc<CacheMgr>(ZEND_THIS);
		cobj->construct(cfg);
	}
}

ZEND_METHOD(Wcc_CacheMgr, __destruct)
{
	ZEND_PARSE_PARAMETERS_NONE();

	CacheMgr*  cobj = zval_toc<CacheMgr>(ZEND_THIS);
	cobj->destruct();
}

ZEND_METHOD(Wcc_CacheMgr, __callStatic)
{
	zarg_rd args(execute_data);

	str_ptr 	fname;
	htab_ptr 	params;
	val_rc      result;

	args.zstring(fname, args.need(0));
	args.zarray(params, args.need(1));

	if (!args.throw_errors(__FUNCTION__))
	{
		result = CacheMgr::callStatic(fname, params);
	}
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_CacheMgr, clearAll)
{
	ZEND_PARSE_PARAMETERS_NONE();

	CacheMgr*  cobj = zval_toc<CacheMgr>(ZEND_THIS);
	cobj->clearAll();
}

ZEND_METHOD(Wcc_CacheMgr, createCache)
{
	zarg_rd args(execute_data);

	str_ptr 	svckey;
	str_ptr     classname;
	htab_ptr 	options;

	args.zstring(svckey, args.need(0));
	args.zstring(classname, args.need(1));
	args.zarray(options, args.need(2));

	if (!args.throw_errors(__FUNCTION__))
	{
		CacheMgr*  cobj = zval_toc<CacheMgr>(ZEND_THIS);
		cobj->createCache(svckey, classname, options);
	}
}

ZEND_METHOD(Wcc_CacheMgr, deleteExpired)
{
	ZEND_PARSE_PARAMETERS_NONE();

	CacheMgr*  cobj = zval_toc<CacheMgr>(ZEND_THIS);
	cobj->deleteExpired();
}

ZEND_METHOD(Wcc_CacheMgr, getCache)
{
	obj_ptr result;
	zarg_rd args(execute_data);

	str_ptr 	svckey;
	args.zstring(svckey, args.need(0));

	if (!args.throw_errors())
	{
		CacheMgr*  cobj = zval_toc<CacheMgr>(ZEND_THIS);
		result = cobj->getCache(svckey);
	}
	result.copy_zv(return_value);
}

ZEND_METHOD(Wcc_CacheMgr, getCacheKeys)
{
	zarg_rd::more_args(execute_data, __FUNCTION__, 0);
	CacheMgr*  cobj = zval_toc<CacheMgr>(ZEND_THIS);
	htab_rc result = cobj->getCacheKeys();

	result.move_zv(return_value);

}

ZEND_METHOD(Wcc_CacheMgr, getCacheClass)
{
	str_rc 		result;
	str_ptr 	svckey;

	zarg_rd args(execute_data);

	args.zstring(svckey, args.need(0));

	if (!args.throw_errors())
	{
		CacheMgr*  cobj = zval_toc<CacheMgr>(ZEND_THIS);
		result = cobj->getCacheClass(svckey);
	}
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_CacheMgr, readCache)
{
	zarg_rd args(execute_data);

	str_ptr 	filename;
	str_ptr     svckey;
	val_rc      result;

	args.zstring(filename, args.need(0));
	args.zstring(svckey, args.need(1));

	if (!args.throw_errors(__FUNCTION__))
	{
		CacheMgr*  cobj = zval_toc<CacheMgr>(ZEND_THIS);
		result = cobj->readCache(filename, svckey);
	}
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_CacheMgr, flush_caches)
{
	ZEND_PARSE_PARAMETERS_NONE();

	CacheMgr*  cobj = zval_toc<CacheMgr>(ZEND_THIS);
	cobj->flush_caches();
}

ZEND_METHOD(Wcc_CacheMgr, readFile)
{
	zarg_rd args(execute_data);

	str_ptr filename;
	str_ptr ext;
	val_rc result;

	args.zstring(filename, args.need(0));
	args.zstring_null(ext, args.option(1));

	if (!args.throw_errors())
	{
		result = CacheMgr::readFile(filename, ext);
	}
	result.move_zv(return_value);
}



PHP_MINIT_FUNCTION(Wcc_CacheMgr_reg)
{
	CacheMgr::omg.classEntry(register_class_Wcc_CacheMgr());

	return SUCCESS;
}

#endif