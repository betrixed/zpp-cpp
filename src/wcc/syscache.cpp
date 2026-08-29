#ifndef WCC_SESSION_ADAPT_SYSCACHE_CPP
#define WCC_SESSION_ADAPT_SYSCACHE_CPP

#ifndef WCC_SESSION_ADAPT_SYSCACHE_H
#  include "syscache.h"
#endif

#ifndef ICACHE_H
#  include "icache.h"
#endif

#ifndef ICACHEDATA_H
#  include "icachedata.h"
#endif

#ifndef SYSCACHE_ARGINFO_H
#define SYSCACHE_ARGINFO_H
extern "C" {
	#  include "stub/syscache_arginfo.h"
}
#endif
namespace wcc {

using namespace zpp;

class SyscInit : public state_init {
public:
	str_intern cache_str;
	str_intern expires_str;


	void init() override;
};

SyscInit  SYSC;

void SyscInit::init()
{
	cache_str = "cache";
	expires_str = "expires";
}


base_obj_mgr<SysCache> SysCache::omg;


void 
SysCache::debug_info(htab_rw di)
{
	di.set(SYSC.cache_str, cache_);
	di.set(SYSC.expires_str, (int)expires_);
}

ICache* 
SysCache::cache_ptr()
{
	return zobj_toc<ICache>(cache_);
}

void 
SysCache::construct(obj_rc cache)
{
	expires_ = 0;
	cache_ = cache;
}

zend_long 
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
	return true;
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

	val_rc mixed = pkg->getData();
	result = std::move(mixed);

	return result;
}

bool   
SysCache::write(str_ptr id, str_ptr data)
{
	ICache* ic = cache_ptr();
	val_rc vdata(data);
	obj_rc pkg_obj = ic->setCached(id, vdata);
	if (pkg_obj.ok())
	{
		ICacheData* pkg = zobj_toc<ICacheData>(pkg_obj);
		expires_ = pkg->getTTL() + time(nullptr);
		return true;
	}
	return false;

}


}//wcc

using namespace zpp;
using namespace wcc;


ZEND_METHOD(Wcc_Session_Adapt_SysCache, __construct)
{
	zarg_rd args(execute_data);

	obj_ptr cache = args.obj_class(args.need(0), ICache::omg.classEntry());

	if (!args.throw_errors())
	{
		SysCache* cobj = zval_toc<SysCache>(ZEND_THIS);
		cobj->construct(cache);
	}

}

ZEND_METHOD(Wcc_Session_Adapt_SysCache, close)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	SysCache* cobj = zval_toc<SysCache>(ZEND_THIS);
	bool result = cobj->close();
	RETURN_BOOL(result);
}

ZEND_METHOD(Wcc_Session_Adapt_SysCache, destroy)
{
	zarg_rd args(execute_data);
	str_ptr id = args.str(args.need(0));
	if (!args.throw_errors())
	{
		SysCache* cobj = zval_toc<SysCache>(ZEND_THIS);
		bool result = cobj->destroy(id);
		RETURN_BOOL(result);
	}
}

ZEND_METHOD(Wcc_Session_Adapt_SysCache, gc)
{
	zarg_rd args(execute_data);
	zend_long lifetime = 0;
	args.zlong(lifetime, args.need(0));
	if (!args.throw_errors())
	{
		SysCache* cobj = zval_toc<SysCache>(ZEND_THIS);
		zend_long result = cobj->gc(lifetime);
		RETURN_LONG(result);
	}
}

ZEND_METHOD(Wcc_Session_Adapt_SysCache, getExpires)
{
	if (!zarg_rd::zero_args(execute_data, __FUNCTION__))
	{
		return;
	}
	SysCache* cobj = zval_toc<SysCache>(ZEND_THIS);
	zend_long result = cobj->getExpires();
	RETURN_LONG(result);
}

ZEND_METHOD(Wcc_Session_Adapt_SysCache, open)
{
	zarg_rd args(execute_data);
	str_ptr path = args.str(args.need(0));
	str_ptr name = args.str(args.need(1));
	
	if (!args.throw_errors())
	{
		SysCache* cobj = zval_toc<SysCache>(ZEND_THIS);
		bool result = cobj->open(path,name);
		RETURN_BOOL(result);
	}
}

ZEND_METHOD(Wcc_Session_Adapt_SysCache, read)
{
	zarg_rd args(execute_data);
	str_ptr id = args.str(args.need(0));
	if (!args.throw_errors())
	{
		SysCache* cobj = zval_toc<SysCache>(ZEND_THIS);
		str_rc result = cobj->read(id);
		if (result.ok())
		{
			result.move_zv(return_value);
		}
		else {
			RETURN_BOOL(false);
		}
	}
}

ZEND_METHOD(Wcc_Session_Adapt_SysCache, write)
{
	zarg_rd args(execute_data);
	str_ptr id = args.str(args.need(0));
	str_ptr data = args.str(args.need(1));
	
	if (!args.throw_errors())
	{
		SysCache* cobj = zval_toc<SysCache>(ZEND_THIS);
		bool result = cobj->write(id,data);
		RETURN_BOOL(result);
	}
}


ZEND_MINIT_FUNCTION(wcc_syscache_reg)
{
	auto ce = register_class_Wcc_Session_Adapt_SysCache(php_session_iface_entry);
	SysCache::omg.classEntry(ce);
	STATE_INIT_ADD(SYSC);
	return SUCCESS;
}

#endif