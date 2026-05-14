#ifndef WCC_DEBUG_CPP
#define WCC_DEBUG_CPP

#ifndef WCC_DEBUG_H
#include "debug.h"
#endif

#ifndef WCC_SERVICES_H
#include "services.h"
#endif

#ifndef DEBUG_ARGINFO_H
extern "C" {
	#  include "stub/debug_arginfo.h"
}
#endif
namespace wcc {

using namespace zpp;

class DebugLogStatic : public state_init {
public:
	obj_rc  gInstance_;

	str_intern config_str;
	str_intern temp_paths_str;
	str_intern logdir_str;
	str_intern tempdir_str;

	void init() override; // start module;

	void init_req() override;
	void end_req() override;

};


DebugLogStatic DLSi;

void 
DebugLogStatic::init()
{
	config_str = "config";
	temp_paths_str = "temp_folder_paths";
	logdir_str = "log_dir";
	tempdir_str = "temp_dir";
}

void 
DebugLogStatic::init_req()
{
}

void 
DebugLogStatic::end_req()
{
	gInstance_.init();
}

obj_rc //static 
DebugLog::instance()
{
	return DLSi.gInstance_;
}

obj_rc //static 
DebugLog::start(str_ptr msg)
{
	obj_rc gDebug = DLSi.gInstance_;
	DebugLog* dg  = nullptr;

	if (gDebug.ok())
	{
		dg = zobj_toc<DebugLog>(gDebug);
		dg->line(msg,0);
		return gDebug;
	}

	obj_rc cfg = Services::service(DLSi.config_str);
	str_rc log_dir;

	htab_rc folders = cfg.array_property(DLSi.temp_paths_str);
	if (folders.size())
	{
		log_dir = folders.get(DLSi.logdir_str);
	}
	if (!log_dir.size())
	{
		log_dir = cfg.str_property(DLSi.tempdir_str);
	}
	str_buf buf;
	buf << log_dir << "/debuglog.txt";

	gDebug = DebugLog::omg.new_zobj();
	dg = zobj_toc<DebugLog>(gDebug);

	dg->construct(buf.zstr());
	DLSi.gInstance_ = gDebug;

	dg->line(msg, 0);

	return gDebug;

}

bool log_;
bool echo_;

void 
DebugLog::construct(str_ptr logpath)
{
	log_ = true;
	echo_ = false;
}

void 
DebugLog::line(str_ptr msg, int flags)
{
	bool trace = (echo_ || log_);
	if (trace)
	{
		if (log_)
		{

		}
	}

	datetime_obj  nowtime;
	str_rc tstr = nowtime.format(DTData.now_format);

	str_buf buf;
	buf << tstr << msg << endl;

	val_rc data(buf.zstr());
	file_put_contents(logfile_, data, PHP_FILE::EXC_LOCK);
}

}// end namespace wcc


using namespace zpp;
using namespace wcc;

ZEND_METHOD(Wcc_Debug, instance)
{
	ZEND_PARSE_PARAMETERS_NONE();

	DebugLog* cobj = zval_toc<DebugLog>(ZEND_THIS);
	obj_rc result = cobj->instance();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Debug, start)
{
	zarg_rd args(execute_data);

	str_ptr msg = args.str(args.need(0));

	if (!args.throw_errors())
	{
		DebugLog* cobj = zval_toc<DebugLog>(ZEND_THIS);
		obj_rc result = cobj->start(msg);
		result.move_zv(return_value);	
	}
}

ZEND_METHOD(Wcc_Debug, __construct)
{
	zarg_rd args(execute_data);

	str_ptr path = args.str(args.need(0));

	if (!args.throw_errors())
	{
		DebugLog* cobj = zval_toc<DebugLog>(ZEND_THIS);
		cobj->construct(path);
	}	
}

ZEND_METHOD(Wcc_Debug, line)
{
	zarg_rd args(execute_data);

	str_ptr msg = args.str(args.need(0));
	zend_long flags = 0;

	args.zlong_null(flags, args.option(1), 0);

	if (!args.throw_errors())
	{
		DebugLog* cobj = zval_toc<DebugLog>(ZEND_THIS);
		cobj->line(msg, flags);	
	}
}

PHP_MINIT_FUNCTION(wcc_debug_reg)
{
	DebugLog::omg.classEntry(register_class_Wcc_Debug());

	STATE_INIT_ADD(DLSi)
	
	return SUCCESS;
}

#endif
