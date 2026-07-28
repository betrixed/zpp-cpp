#ifndef WCC_RUNSA_H
#define WCC_RUNSA_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

namespace wcc {
	using namespace zpp;

	class Run : public base_d {
	protected:
		error_return  config_init(str_ptr bootstrap);
		error_return setup_cryptic();
		
		void temp_folders();
		obj_ptr setup_world();
		
		
	public:

		static base_obj_mgr<Run> omg;

		void debug_info(htab_rw di) override;

		void construct();
		void destruct();
		error_return execute(str_ptr bootstrap);
		void shutdown();

	};

class Run_init : public state_init {
public:
	// property names
	str_intern start_time;
	str_intern target;
	str_intern target_const;
	str_intern site_leaf;
	str_intern bootstrap;

	str_intern site_leaf_const;
	str_intern site_const;

	str_intern wc_leaf;
	str_intern vendor_leaf;
	str_intern vendor_str;
	str_intern phproot;

	str_intern config_dir;
	str_intern temp_dir;
	str_intern init_cwd;
	str_intern dos_str;

	str_intern page_hits;
	str_intern user_name;
	str_intern view_data;
	str_intern finder;
	str_intern finder_class;

	str_intern s_services;
	str_intern run_str;
	str_intern phpstats_class;
	str_intern phpstats_str;
	str_intern default_str;

	str_intern sapi_str;
	str_intern cli_str;
	str_intern begin_str;
	str_intern stats_str;

	str_intern is_web;
	str_intern php_str;
	str_intern config_str;
	str_intern web_str;
	str_intern assets_str;

	str_intern web_dir;
	str_intern theme_str;
	str_intern br_eol;
	str_intern br_html;
	str_intern br_eol_str;

	str_intern gallery_str;
	str_intern site_dir;

	str_intern ns_sep;
	str_intern dir_sep;
	str_intern addPathArray;
	str_intern cache_mgr;
	str_intern file_cache;

	str_intern namespaces_str;
	str_intern modules_str;
	str_intern error_log;
	str_intern temp_folder_names;
	str_intern temp_folder_paths;

	str_intern app_class;
	str_intern s_prepare;
	str_intern site_str;
	str_intern cryptic_str;

	str_intern user_session;
	str_intern is_ended;

	void init() override;
};

extern Run_init Run_i;
};

#endif