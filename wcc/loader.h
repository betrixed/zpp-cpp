#ifndef WCC_LOADER_H
#define WCC_LOADER_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

namespace wcc {
	using namespace zpp;

	class Loader : public base_d 
	{
	protected:
		obj_rc  finder_;
		val_rc  extloader_;
		str_rc  basedir_;
		bool    throwNotFound_;
		bool    isRegistered_;

		fn_call_args<1> fdr_find_;

		void call_spl(str_ptr fname);
	public:

		static base_obj_mgr<Loader> omg;

		static obj_ptr instance();
		
		static val_rc  readPHP(str_ptr file);

		static Loader* cpp_global();

		void debug_info(htab_rw di) override;
		
		Loader();

		void destruct();

		void setThrowNotFound(bool val) { throwNotFound_ = val; }

		void setExtLoader(val_ptr exload);

    	void setFinder(obj_ptr finder);

    	obj_ptr getFinder() { return finder_; }

    	void setBaseDir(str_ptr dir);
   
    	str_ptr getBaseDir() { return basedir_; }

    	val_rc require(str_ptr file);

    	bool load(str_ptr class_name);

    	bool must_load(str_ptr class_name);

    	void regLoader();

    	void unregLoader();

	};

}; // namespace wcc
#endif