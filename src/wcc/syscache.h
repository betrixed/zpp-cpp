#ifndef WCC_SESSION_ADAPT_SYSCACHE_H
#define WCC_SESSION_ADAPT_SYSCACHE_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

namespace wcc {

	// cache session data.
	// calculate end time from last update.
	class SysCacheInit : public state_init 
	{
	public:
		str_intern cache_key_str;
		str_intern cache_str;
		str_intern expires_str;
		str_intern default_cache_key;

		void init() override;

	};

	extern SysCacheInit SCinit;

	class ICache;
	
	class SysCache : public base_d {
	protected: 
		obj_rc  cache_;
		zend_long 	  expires_;
		zend_long     pkg_stored_; // pkg last update.

		ICache* cache_ptr();
	public:

		enum {
			DEFAULT_EXPIRY = 40*60
		};

		static base_obj_mgr<SysCache> omg;

		void debug_info(htab_cow di) override;

		error_return construct(htab_ptr options = htab_ptr::empty_array());

		// Note next 2, are not part of SessionHandler Interface.
		// getExpires is more or less a copy of ICacheData getTTL();
		zend_long getExpires();

		// Caculate session end time if no further updates.
		zend_long getEndTime();

		bool close();

		bool destroy(str_ptr id);

		int  gc(int max_lifetime);

		bool open(str_ptr path, str_ptr name);

		str_rc read(str_ptr id);

		bool   write(str_ptr id, str_ptr data);

	};
}

#endif