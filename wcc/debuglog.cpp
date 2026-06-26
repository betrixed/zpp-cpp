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
	#  include "stub/debuglog_arginfo.h"
}
#endif
namespace wcc {

using namespace zpp;

base_obj_mgr<DebugLog> DebugLog::omg;



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

void //static 
DebugLog::setInstance(obj_ptr obj)
{
	DLSi.gInstance_ = obj;
}

DebugLog* //static 
DebugLog::cpp_global()
{
	obj_ptr obj = DLSi.gInstance_;
	return obj.ok() ? zobj_toc<DebugLog>(obj) 
					: (DebugLog*) nullptr;
}

void 
DebugLog::dump(str_ptr label, zval* anyval)
{
	dump(label.data(), anyval);
}

void DebugLog::showmem(const char* label, zval* mem)
{
	str_buf dump;
	dump_info di(dump);

	dump << label << ' ';
	di.setMaxLevel(5);
	di.di_showmem(mem);
	di.di_dump(mem);
	str_rc out = dump.zstr();

	line(out);

}
void 
DebugLog::dump(const char* label, zval* anyval)
{
	str_buf dump;

	dump_info di(dump);
	di.setMaxLevel(5);
	
	dump << label << ":" << endl;
	di.di_showmem(anyval);
	di.di_dump(anyval);

	str_rc out = dump.zstr();

	line(out);
}


void 
DebugLog::dump(const char* label, HashTable* arrayval)
{
	val_rc value(arrayval);

	dump(label, val_ptr(value));
}

void 
DebugLog::dump(const char* label, zend_string* strval)
{
	val_rc value(strval);

	dump(label, val_ptr(value));
}

void 
DebugLog::dump(const char* label, zend_object* objval)
{
	val_rc value(objval);
	dump(label, val_ptr(value));
}


obj_rc //static
DebugLog::start(const char* msg, int flags)
{
	str_rc temp(msg);

	return DebugLog::start(temp, flags);
}

obj_rc //static 
DebugLog::start(str_ptr msg, int destflags)
{

	DebugLog* dg  = nullptr;
	obj_rc result = DebugLog::instance();

	if (result.ok())
	{
		dg = zobj_toc<DebugLog>(result);

		dg->setOutputs(destflags);

		dg->line(msg,0);
		return result;
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

	result = DebugLog::omg.new_zobj();
	dg = zobj_toc<DebugLog>(result);

	dg->construct(filename, destflags);

	DLSi.gInstance_ = result;

	dg->line(msg, 0);

	return result;

}

int 
DebugLog::getOutputs()
{
	return outputs_;
}

void
DebugLog::setOutputs(int flags)
{
	outputs_ = flags;
}

void 
DebugLog::debug_info(htab_rw hw)
{
	base_d::debug_info(hw);
	hw.set(DLSi.outputs_str, (int) outputs_);
}

void 
DebugLog::construct(str_ptr logpath, int destflags)
{
	obj_ptr self = self_;

	val_rc temparg(logpath);

	self.property(DLSi.filename_str, temparg);

	outputs_ = destflags;

}

void 
DebugLog::line(const char* s)
{
	str_rc temp(s);
	this->line(temp, DebugLog::FILE_APPEND);
}
void 
DebugLog::line(str_ptr msg, int flags)
{
	obj_ptr self(self_);

	int outflags = outputs_;

	bool trace = ((outflags & (TO_FILE|TO_CONSOLE)) != 0);
	if (trace)
	{
		datetime_obj  nowtime;
		str_rc tstr = nowtime.format(DTData.now_format);
		str_buf buf;
		buf << tstr << ' ';
		buf << msg << endl;

		str_rc log = buf.zstr();

		bool dolog = ((outflags&TO_FILE)!=0);
		if (dolog)
		{
			
			str_rc filename = self.str_property(DLSi.filename_str);
			val_rc data(log);
			file_put_contents(filename, data, flags | FILE_LOCK);
		}
		bool docons = ((outflags&TO_CONSOLE)!=0);
		if (docons)
		{
			zend_write(log.data(), log.size());
		}
	}
}

}// end namespace wcc


using namespace zpp;
using namespace wcc;

ZEND_METHOD(Wcc_DebugLog, instance)
{
	ZEND_PARSE_PARAMETERS_NONE();

	obj_rc result = DebugLog::instance();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_DebugLog, start)
{
	zarg_rd args(execute_data);

	str_ptr msg = args.str(args.need(0));
	zend_long flags = 0;

	args.zlong_null(flags, args.option(1), DebugLog::TO_FILE);

	if (!args.throw_errors(__FUNCTION__))
	{
		obj_rc result = DebugLog::start(msg, flags);
		result.move_zv(return_value);	
	}
}

ZEND_METHOD(Wcc_DebugLog, __construct)
{
	zarg_rd args(execute_data);

	str_ptr path = args.str(args.need(0));
	zend_long flags = 0;

	args.zlong_null(flags, args.option(1), DebugLog::TO_FILE);

	if (!args.throw_errors(__FUNCTION__))
	{
		DebugLog* cobj = zval_toc<DebugLog>(ZEND_THIS);
		cobj->construct(path);
	}	
}

ZEND_METHOD(Wcc_DebugLog, line)
{
	zarg_rd args(execute_data);

	str_ptr msg = args.str(args.need(0));
	zend_long flags = 0;

	args.zlong_null(flags, args.option(1),  DebugLog::FILE_APPEND);

	if (!args.throw_errors(__FUNCTION__))
	{
		DebugLog* cobj = zval_toc<DebugLog>(ZEND_THIS);
		cobj->line(msg, flags);	
	}
}

ZEND_METHOD(Wcc_DebugLog, setOutputs)
{
	zarg_rd args(execute_data);

	zend_long flags;

	args.zlong(flags, args.need(0));

	if (!args.throw_errors(__FUNCTION__))
	{
		DebugLog* cobj = zval_toc<DebugLog>(ZEND_THIS);
		cobj->setOutputs(flags);	
	}
}


ZEND_METHOD(Wcc_DebugLog, getOutputs)
{
	if (zarg_rd::zero_args(execute_data, __FUNCTION__))
		return;

	DebugLog* cobj = zval_toc<DebugLog>(ZEND_THIS);
	RETURN_LONG(cobj->getOutputs());	
}

ZEND_METHOD(Wcc_DebugLog, setInstance)
{
	zarg_rd args(execute_data);

	obj_ptr obj;

	args.obj_ofclass(obj, args.need(0), DebugLog::omg.classEntry());

	if (!args.throw_errors(__FUNCTION__))
	{
		DebugLog::setInstance(obj);	
	}
}

ZEND_METHOD(Wcc_DebugLog, dump)
{
	zarg_rd args(execute_data);

	str_ptr label = args.str(args.need(0));
	zval* value = args.need(1);

	if (!args.throw_errors(__FUNCTION__))
	{
		DebugLog* cobj = zval_toc<DebugLog>(ZEND_THIS);
		cobj->dump(label, value);
	}
	
}


PHP_MINIT_FUNCTION(wcc_debuglog_reg)
{
	DebugLog::omg.classEntry(register_class_Wcc_DebugLog());

	STATE_INIT_ADD(DLSi)
	
	return SUCCESS;
}

#endif
