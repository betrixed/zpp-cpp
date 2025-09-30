#ifndef WCC_CACHEMGR_CPP
#define WCC_CACHEMGR_CPP

#ifndef WCC_CACHEMGR_H
#include "cachemgr.h"
#endif

#ifndef REFLECT_CACHE_H
#include "reflect_cache.h"
#endif

#ifndef TOML_PHP_H
#include "toml/toml_php.h"
#endif

#ifndef XML_READ_H
#include "wcc/xmlread.h"
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
	
	s_set = "set";
	s_get = "get";
	s_delete = "delete";
	s_getcached = "getcached";
	s_setoption = "setoption";
	s_writecached = "writecached";
	s_delete_expired = "deleteexpired";
	s_clear_str = "clear";
	defer_write = "defer_write";

	php_ext = "php";
	toml_ext = "toml";
	xml_ext = "xml";
}

void CacheMgr::construct(htab_ptr cfg)
{
	this->init(cfg);
}


val_rc //static
CacheMgr::callStatic(str_ptr name, htab_ptr args)
{
	return val_rc();
}

void CacheMgr::init(htab_ptr cfg)
{
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
	for_key_value wk;

	fn_call fnclear;

	fnclear.set_fname(Cache_i.s_clear_str);

	for(wk.start(cache_obj_); wk.ok(); wk.next())
	{
		obj_rc obj = wk.value();
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
	htab_rc config_args = cache_defaults_;
	htab_rw config(config_args);
	config.merge(options);

	htab_rc args_cache;
	htab_rw args(args_cache);

	args.push_back(config_args);
	args.push_back(svc->self());

	obj_rc obj = ReflectCache::staticInstanceArgs(classname, args_cache);

	htab_rw cache_obj(cache_obj_);
	cache_obj.set(svckey, obj);
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
				for_key_value wk;

				fn_call delete_expired;

				delete_expired.set_fname(Cache_i.s_delete_expired);

				for(wk.start(cache_obj_); wk.ok(); wk.next())
				{
					obj_ptr obj = wk.value();
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
		long mtime = FTAB.filemtime.call(filename);
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

	if (data.ok())
	{
		if (!cache.call(Cache_i.s_set, data))
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
CacheMgr::write_caches()
{
	for_key_value wk;

	for(wk.start(cache_obj_); wk.ok(); wk.next())
	{
		obj_ptr obj(wk.value());
		obj.call(Cache_i.s_writecached);

		val_rc arg1(Cache_i.defer_write);
		val_rc arg2(false);
		obj.call(Cache_i.s_setoption, arg1, arg2);
	}
}


val_rc //static
CacheMgr::readFile(str_ptr filename, str_ptr ext)
{
	str_rc filetype;

	if (!ext.ok())
	{
		filetype = FTAB.pathinfo.call(filename, PathInfo::EXTENSION);
	}
	else {
		filetype = ext;
	}
	if (zs_cmp_ci(filetype,Cache_i.xml_ext)==0)
	{
		return readXml(filename);
	}
	if (zs_cmp_ci(filetype,Cache_i.php_ext)==0)
	{
		return readPhp(filename);
	}
	if (zs_cmp_ci(filetype, Cache_i.toml_ext)==0)
	{
		return readToml(filename);
	}
	zend_throw_error(zend_ce_error,"Unmatched file extension %s", filetype.data());
	return val_rc();
}

val_rc  //static
CacheMgr::readPhp(str_ptr filename)
{
	return FTAB.simple_loader.call(filename);
}

val_rc  //static
CacheMgr::readToml(str_ptr filename)
{
	val_rc result(Toml::decodeFile(filename));
	return result;
}
val_rc  //static
CacheMgr::readXml(str_ptr filename)
{
	val_rc result(Wcc_XmlRead::fromFile(filename));
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

	args.zarray(cfg, args.need(1));

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

	args.zstring(fname, args.need(1));
	args.zarray(params, args.need(2));

	if (!args.throw_errors())
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

	args.zstring(svckey, args.need(1));
	args.zstring(classname, args.need(2));
	args.zarray(options, args.need(3));

	if (!args.throw_errors())
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
	args.zstring(svckey, args.need(1));

	if (!args.throw_errors())
	{
		CacheMgr*  cobj = zval_toc<CacheMgr>(ZEND_THIS);
		result = cobj->getCache(svckey);
	}
	result.return_zv(return_value);
}

ZEND_METHOD(Wcc_CacheMgr, getCacheClass)
{
	str_rc 		result;
	str_ptr 	svckey;

	zarg_rd args(execute_data);

	args.zstring(svckey, args.need(1));

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

	args.zstring(filename, args.need(1));
	args.zstring(svckey, args.need(2));

	if (!args.throw_errors())
	{
		CacheMgr*  cobj = zval_toc<CacheMgr>(ZEND_THIS);
		result = cobj->readCache(filename, svckey);
	}
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_CacheMgr, write_caches)
{
	ZEND_PARSE_PARAMETERS_NONE();

	CacheMgr*  cobj = zval_toc<CacheMgr>(ZEND_THIS);
	cobj->write_caches();
}

ZEND_METHOD(Wcc_CacheMgr, readFile)
{
	zarg_rd args(execute_data);

	str_ptr filename;
	str_ptr ext;
	val_rc result;

	args.zstring(filename, args.need(1));
	args.zstring_null(ext, args.option(2));
	if (!args.throw_errors())
	{
		result = CacheMgr::readFile(filename, ext);
		result.move_zv(return_value);
	}
}

ZEND_METHOD(Wcc_CacheMgr, readPhp)
{
	zarg_rd args(execute_data);

	str_ptr filename;
	val_rc result;

	args.zstring(filename, args.need(1));

	if (!args.throw_errors())
	{
		result = CacheMgr::readPhp(filename);
		result.move_zv(return_value);
	}
}

ZEND_METHOD(Wcc_CacheMgr, readToml)
{
	zarg_rd args(execute_data);

	str_ptr filename;
	val_rc result;

	args.zstring(filename, args.need(1));

	if (!args.throw_errors())
	{
		result = CacheMgr::readToml(filename);
		result.move_zv(return_value);
	}
}

ZEND_METHOD(Wcc_CacheMgr, readXml)
{
	zarg_rd args(execute_data);

	str_ptr filename;
	val_rc result;

	args.zstring(filename, args.need(1));

	if (!args.throw_errors())
	{
		result = CacheMgr::readXml(filename);
		result.move_zv(return_value);
	}
}

PHP_MINIT_FUNCTION(Wcc_CacheMgr_reg)
{
	CacheMgr::omg.classEntry(register_class_Wcc_CacheMgr());

	return SUCCESS;
}

#endif