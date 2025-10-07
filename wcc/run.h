#ifndef WCC_RUNSA_H
#define WCC_RUNSA_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

namespace wcc {
	using namespace zpp;

	class Run : public base_d {
	protected:
		void config_init(str_ptr bootstrap);
		void temp_folders();
		obj_ptr setup_world();
		void load_boot(str_ptr php_root);
		void setup_cryptic();
		
	public:

		static base_obj_mgr<Run> omg;

		bool class_load(str_ptr class_name, str_ptr php_root);

		

		void construct();
		void destruct();
		void execute(str_ptr bootstrap);
		void shutdown();

	};
};

#endif