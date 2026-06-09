#ifndef WCC_SESSION_ADAPT_SYSCACHE_CPP
#define WCC_SESSION_ADAPT_SYSCACHE_CPP

#ifndef WCC_SESSION_ADAPT_SYSCACHE_H
#  include "syscache.h"
#endif

#ifndef ICACHE_H
#  include "icache.h"
#endif

namespace wcc {

using namespace zpp;

class SyscInit : public state_init {
public:
	str_intern cache_str;
	str_intern expires_str;
	str_intern cache_key_str;

	void init() override;
};

SyscInit  SYSC;

void SyscInit::init()
{
	cache_str = "cache";
	expires_str = "expires";
	cache_key_str = "cache_key";
}


base_obj_mgr<SysCache> SysCache::omg;


void 
SysCache::debug_info(htab_rw di)
{
	di.set(SYSC.cache_str, cache_);
	di.set(SYSC.expires_str, (zend_long)expires_);
}

ICache* 
SysCache::cache_ptr()
{
	return zobj_toc<ICache>(cache_);
}

void 
SysCache::construct(htab_ptr options)
{
	expires_ = 0;
	str_rc cache_key = options.get(SYSC.cache_key_str);
	val_rc cache = Services::service(cache_key);
	val_ptr test(cache);
	if (test.isObject() && test.instanceof(ICache::omg.classEntry()))
	{
		cache_ = test.zobject();
	}
	else {
		error_return msg;

		msg.error() << "Cache object not found: name " << cache_key;
		msg.throw_errors();
	}
}

int 
SysCache::getExpires()
{
	return expires_;
}

bool 
SysCache::close()
{
	return true;
}

bool 
SysCache::destroy(str_ptr id)
{
	ICache* ic = cache_ptr();
	ic->deleteKey(id);
}

int  
SysCache::gc(int max_lifetime)
{
	ICache* ic = cache_ptr();
	int ttl = ic->getTTL();
	if (ttl != max_lifetime)
	{
		ic->setTTL(max_lifetime);
		return max_lifetime;
	}
	return ttl;
}

bool 
SysCache::open(str_ptr path, str_ptr name)
{
	return true;
}

str_rc 
SysCache::read(str_ptr id)
{
	str_rc result;

	ICache* ic = cache_ptr();
	val_rc pkg_obj = ic->getCached(id);
	if (!pkg_obj.isObject())
	{
		result = str_ptr::empty_str();
		return result;
	}
	ICacheData* pkg = zval_toc<ICacheData>(pkg_obj);
	expires_ = pkg->getExpiry();

	int ttl = pkg->getTTL();

	if (ttl != ic->getTTL())
	{
		// update pkg ttl instead??
		ic->setTTL(ttl);
	}

	result = pkg->getData();

	return result;
}

bool   
SysCache::write(str_ptr id, str_ptr data)
{
	ICache* ic = cache_ptr();
	obj_rc pkg_obj = ic->setCached(id, data);
	if (pkg_obj.ok())
	{
		ICacheData* pkg = zval_toc<ICacheData>(pkg_obj);
		expires_ = pkg->getTTL() + time();
		return true;
	}
	return false;

}


}//wcc

#endif