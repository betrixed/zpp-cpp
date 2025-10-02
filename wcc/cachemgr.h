#ifndef WCC_CACHEMGR_H
#define WCC_CACHEMGR_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

#ifndef XML_READ_H
#include "wcc/xmlread.h"
#endif

namespace wcc {

using namespace zpp;

class CacheMgr_init : public state_init {
public:
	str_intern defaults_str;
	str_intern fast_cache;
	str_intern del_expired;
	str_intern expiry_key;
	str_intern expired_check;

	str_intern default_cache;
	str_intern file_cache;
	str_intern defer_write;

	str_intern s_set;
	str_intern s_get;
	str_intern s_clear_str;
	
	str_intern s_delete;
	str_intern s_getcached;
	str_intern s_setoption;
	str_intern s_writecached;
	str_intern s_delete_expired;
	
	str_intern s_simple_loader;
	
	str_intern cache_mgr;
	
	str_intern php_ext;
	str_intern toml_ext;
	str_intern xml_ext;

	str_intern s_cache_obj;
	str_intern s_cache_defaults;
	str_intern s_default_cache;


	void init() override;

};

extern CacheMgr_init Cache_i;


class  CacheMgr : public base_d {
protected:
	htab_rc cache_obj_;
	htab_rc cache_defaults_;

	str_rc  fast_cache_;
	str_rc  default_cache_;

	str_rc  expiry_key_;
	int     delete_expired_;

public:
	static base_obj_mgr<CacheMgr> omg;

	void debug_info(htab_rw di) override;

	void construct(htab_ptr cfg);

	void init(htab_ptr cfg);

	void destruct();
	

	void clearAll();
	void createCache(str_ptr svckey, str_ptr classname, htab_ptr options);
	void deleteExpired();

	static val_rc  callStatic(str_ptr method, htab_ptr params);

	obj_ptr getCache(str_ptr svckey);

	str_ptr getCacheClass(str_ptr svckey);

	htab_rc getCacheKeys();

	val_rc readCache(str_ptr filename, str_ptr cachename);

	static val_rc readFile(str_ptr filename, str_ptr ext = str_ptr());
	static val_rc readPhp(str_ptr filename);
	static val_rc readToml(str_ptr filename);
	static val_rc readXml(str_ptr filename);

	void write_caches();

}; // CacheMgr


}; //namespace wcc

#endif