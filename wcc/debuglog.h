#ifndef WCC_DEBUGLOG_H
#define WCC_DEBUGLOG_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

namespace wcc {

using namespace zpp;
	

class DebugLogStatic : public state_init {
public:
	obj_rc  gInstance_;
	str_intern filename_str;

	str_intern config_str;
	str_intern temp_paths_str;
	str_intern logdir_str;
	str_intern tempdir_str;
	str_intern outputs_str;
	
	void init() override; // start module;

	void init_req() override;
	void end_req() override;

};


extern DebugLogStatic DLSi;

class  DebugLog : public base_d {
public:

	enum {
	 	 TO_FILE = 1,
	     TO_CONSOLE = 2
	};

	enum {
		FILE_APPEND = PHP_FILE::APPEND,
		FILE_LOCK = PHP_FILE::EXCL_LOCK
	}

	void debug_info(htab_rw hw) override;

	static base_obj_mgr<DebugLog> omg;

	static obj_rc instance();
	static obj_rc start(str_ptr msg, int flags=TO_FILE);


	void construct(str_ptr logpath, int flags=TO_FILE);
	void line(str_ptr msg, int flags = PHP_FILE::APPEND);
	void setOutputs(int flags = TO_CONSOLE);
};



}// namespace wcc
#endif