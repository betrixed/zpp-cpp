#ifndef PERSISTENT_WCC_H
#define PERSISTENT_WCC_H


#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif


namespace wcc {
	using namespace zpp;

	class PersistInit : public state_init {
	public:

		htab_persist root_;

		obj_rc       persist_root_;

		void init() override;

	};

	extern PersistInit PIRoot;

	/**
	 * Class Persistent wraps a pointer to a permanent htab_persist
	 * C++ object, which wraps a persistent HashTable, and is even writable.
	 */
	extern PersistInit gPersist;



	class Persistent : public base_d {
		htab_persist*  persist_;
	public:
		
		static base_obj_mgr<Persistent> omg;

		Persistent();

		static obj_rc get_root();


		void init(htab_persist* p);

		void set(str_ptr key, val_ptr value);

		val_rc get(str_ptr key);


	#ifndef BASE_ZOBJPTR
		VIRTUAL_ZOBJPTR
	#endif
	};

}; //wcc namespace

#endif