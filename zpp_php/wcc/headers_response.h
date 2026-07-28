#ifndef HEADERS_RESPONSE_H
#define HEADERS_RESPONSE_H

// Not used anymore, excised from global_response

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

#ifndef WCC_HMAP_H
#include "hmap.h";
#endif

namespace wcc {

using namespace zpp;
	/**
	 * Simple functions to cache
	 * writes to headers for Http response.
	 */ 
	class Headers;

	class Headers_mgr : public base_obj_mgr<Headers>
	{
	protected:
		//typedef base_obj_mgr<T> mydef;

		virtual void init_class_fn() 
		{
			mydef::init_class_fn();

			HmapIterator::setup_handlers(mydef::handlers_);
			HmapIterator::setup_class(mydef::class_entry_);
		}
	};
	
	class Headers : public Hmap 
	{
	public:
		static Headers_mgr omg;
		
		virtual void debug_info(htab_rw ht);
		 
		void reset();
		bool send();
		void setRaw(str_ptr zs);
	};

};

//headers_response.h
#endif
