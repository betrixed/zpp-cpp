#ifndef WCC_DEBUGLOG_H
#define WCC_DEBUGLOG_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

namespace wcc {

using namespace zpp;
	

class DebugLogStatic : public state_init {
public:
    obj_rc  	gInstance_;
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
protected: 
		int outputs_;
public:

	enum {
	 	 TO_FILE = 1,
	     TO_CONSOLE = 2,
	     FILE_APPEND = PHP_FILE::APPEND,
		 FILE_LOCK = PHP_FILE::EXCL_LOCK
	};


	

	static base_obj_mgr<DebugLog> omg;

	static DebugLog* cpp_global();

	static obj_rc instance();
	static void setInstance(obj_ptr obj);
	
	static obj_rc start(str_ptr msg, int flags=TO_FILE);
	static obj_rc start(const char* msg, int flags=TO_FILE);
	void debug_info(htab_rw hw) override;
	void construct(str_ptr logpath, int flags=TO_FILE);
	void line(str_ptr msg, int flags = PHP_FILE::APPEND);
	void line(const char* s);

	void dump(str_ptr label, zval* anyval, int maxlevel=1);
	void dump(const char* label, zval* anyval, int maxlevel=1);
	void dump(const char* label, HashTable* arrayval, int maxlevel=1);
	void dump(const char* label, zend_string* strval, int maxlevel=1);
	void dump(const char* label, zend_object*  objval, int maxlevel=1);
	void showmem(const char* label, zval* mem, int maxlevel=1);
	
	void setOutputs(int flags = TO_CONSOLE);
	int  getOutputs();
};



}// namespace wcc
#endif