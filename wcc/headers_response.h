#ifndef HEADERS_RESPONSE_H
#define HEADERS_RESPONSE_H


#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

#ifndef WCC_HMAP_H
#include "hmap.h";
#endif

namespace wcc {

	/**
	 * Simple functions to cache
	 * writes to headers for Http response.
	 */ 
	class Headers : public Hmap 
	{
	public:
		static Hmap_mgr<Headers> omg;
		
		virtual void debug_info(htab_write ht);
		 
		void reset();
		bool send();
		void setRaw(zstr_user zs);
	};

};

//headers_response.h
#endif
