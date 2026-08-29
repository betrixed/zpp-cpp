#ifndef WCC_SESSION_ADAPT_SYSCACHE_H
#define WCC_SESSION_ADAPT_SYSCACHE_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

namespace wcc {

	class ICache;
	
	class SysCache : public base_d {
	protected: 
		obj_rc  cache_;
		zend_long expires_;

		ICache* cache_ptr();
	public:
		static base_obj_mgr<SysCache> omg;

		void debug_info(htab_rw di) override;

		void construct(obj_rc cache);

		zend_long getExpires();

		bool close();

		bool destroy(str_ptr id);

		int  gc(int max_lifetime);

		bool open(str_ptr path, str_ptr name);

		str_rc read(str_ptr id);

		bool   write(str_ptr id, str_ptr data);

	};
}

#endif