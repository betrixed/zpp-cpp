#ifndef WCC_DEBUG_H
#define WCC_DEBUG_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

namespace wcc {

using namespace zpp;
	

class  DebugLog : public base_d {
protected:
	str_rc logfile_;
public:

	static base_obj_mgr<DebugLog> omg;

	static obj_rc instance();
	static obj_rc start(str_ptr msg);


	bool log_;
	bool echo_;

	void construct(str_ptr logpath);
	void line(str_ptr msg, int flags = PHP_FILE::APPEND);
};

}// namespace wcc
#endif