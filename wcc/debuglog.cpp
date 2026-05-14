#ifndef WCC_DEBUGLOG_CPP
#define WCC_DEBUGLOG_CPP

#ifndef WCC_DEBUGLOG_H
#include "debuglog.h"
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

static base_obj_mgr<DebugLog> DebugLog::omg;



DebugLogStatic DLSi;

void 
DebugLogStatic::init()
{
	filename_str = "filename";
	config_str = "config";
	temp_paths_str = "temp_folder_paths";
	logdir_str = "log_dir";
	tempdir_str = "temp_dir";
	outputs_str = "outputs";
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
	str_rc filename = buf.zstr();

	gDebug = DebugLog::omg.new_zobj();
	dg = zobj_toc<DebugLog>(gDebug);


	dg->construct(filename);
	DLSi.gInstance_ = gDebug;

	dg->line(msg, 0);

	return gDebug;

}

void
DebugLog::setOutputs(int flags)
{
	val_rc outputs((zend_long) flags);

	str_ptr(self_).property(DLSi.outputs_str, outputs);
}

void 
DebugLog::debug_info(htab_rw hw)
{
	base_d::debug_info(hw);
}

void 
DebugLog::construct(str_ptr logpath, int destflags);
{
	log_ = true;
	echo_ = false;


	obj_ptr self = self_;

	val_rc temparg(logpath);

	self.property(DLSi.filename_str, temparg);

	temparg = destflags;
	self.property(DLSi.outputs_str, temparg);

	filename_ = logpath;

}

void 
DebugLog::line(str_ptr msg, int flags)
{
	val_rc outputs = obj_ptr(self_).property(DLSi);

	outflags = outputs.zlong();

	bool trace = ((outflags & (TO_FILE|TO_CONSOLE)) != 0);
	if (trace)
	{
		bool dolog = ((outflags&TO_FILE)!=0);
		if (dolog)
		{
			datetime_obj  nowtime;
			str_rc tstr = nowtime.format(DTData.now_format);
			str_buf buf;
			buf << tstr << msg << endl;

			val_rc data(buf.zstr());
			file_put_contents(filename_, data, flags | FILE_LOCK);
		}
		bool docons = ((outflags&TO_CONSOLE)!=0);
		if (docons)
		{

		}
	}
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
