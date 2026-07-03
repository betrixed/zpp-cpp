#ifndef WCC_LOADER_CPP
#define WCC_LOADER_CPP

#ifndef WCC_LOADER_H
#include "loader.h"
#endif

#ifndef WCC_DEBUGLOG_H
#include "debuglog.h"
#endif

#ifndef WCC_SERVICES_H
#include "services.h"
#endif

#ifndef LOADER_ARGINFO_H
#define LOADER_ARGINFO_H
extern "C" {
	#include "stub/loader_arginfo.h"
}
#endif

namespace wcc {

using namespace zpp;

base_obj_mgr<Loader> Loader::omg;

thread_local obj_rc     gLoader = obj_rc();

class Loader_init : public state_init {
public:
	str_intern s_mustload;
	str_intern s_register;
	str_intern s_unregister;
	str_intern s_find;
	str_intern ns_sep;
	str_intern dir_sep;
	str_intern php_ext;

	str_intern finder_str;
	str_intern loader_str;
	str_intern invoke_fn;

	str_intern extloader_str;
	str_intern basedir_str;
	str_intern isreg_str;
	str_intern throwon_err;

	void init() override;

	void init_req() override;
	void end_req() override;

};

Loader_init LDRi;


void Loader_init::init()
{
	//zend_printf("Loader_init::init\n");
	s_mustload = "mustload";
	s_register = "spl_autoload_register";
	s_unregister = "spl_autoload_unregister";
	s_find = "find";
	ns_sep = "\\";
	dir_sep = "/";
	php_ext = "php";
	finder_str = "finder";
	loader_str = "loader";
	invoke_fn = "__invoke";

	extloader_str = "extloader";
	basedir_str = "basedir";
	isreg_str = "registered";
	throwon_err = "throwerr";
}

void Loader_init::init_req()
{
	//zend_printf("Loader_init::init_req\n");
	gLoader = Loader::omg.new_zobj();
}

void Loader_init::end_req()
{
	//zend_printf("Loader_init::end_req\n");
	gLoader.init();
}

void Loader::setFinder(obj_ptr finder) 
{
	finder_ = finder;
	fdr_find_.set_fci(LDRi.s_find, finder);
}

htab_ptr
Loader::getLoaded()
{
	return loaded_;
}

htab_ptr
Loader::getRequired()
{
	return required_;
}

void
Loader::call_spl(str_ptr fname)
{
	htab_rc cfn;
	//showstr("call method", fname);

	htab_rw hw(cfn);

	hw.push_back(this->self());
	hw.push_back(LDRi.s_mustload);

	fn_call  spl(fname);
	fn_params<1> fn(spl);

	val_ptr::array_bind(fn.argsptr(), cfn);
	fn.call_fn();
}

Loader::Loader() : base_d()
{
	isRegistered_ = false;
	throwNotFound_ = false;
	record_ = false;
}

void Loader::debug_info(htab_rw di)
{
	di.set(LDRi.finder_str, finder_);
	di.set(LDRi.extloader_str, extloader_);
	di.set(LDRi.basedir_str, basedir_);
	di.set(LDRi.throwon_err, throwNotFound_);
	di.set(LDRi.isreg_str, isRegistered_);
}

obj_ptr // static
Loader::instance()
{
	return gLoader;
}

Loader* 
Loader::cpp_global()
{
	return zobj_toc<Loader>(gLoader);
}

void
Loader::setRecord(bool val)
{
	record_ = val;
}

val_return //static
Loader::readPHP(str_ptr path)
{
	Loader* lob = Loader::cpp_global();
	val_return result = lob->require(path);
	//showmem("readPHP result", result);
	return result;
}

static void php_path(str_ptr php_root, const char* s, htab_rw data)
{
	str_rc ns(s);
	str_buf buf;

	buf << php_root << "/" << ns;
	str_rc value = buf.zstr();
	data.set(ns, value);
}

void 
Loader::setBaseDir(str_ptr dir) 
{ 
	basedir_ = dir; 

	obj_rc finder = Finder::omg.new_zobj();
	Finder* fob = zobj_toc<Finder>(finder);

	htab_rc paths_data;
	htab_rw paths(paths_data);

	php_path(basedir_, "Wcc", paths);
	php_path(basedir_, "Wc", paths);
	php_path(basedir_, "Wcd", paths);

	fob->addPathArray(paths_data);

	this->setFinder(finder);

	this->regLoader();
	
	Services* sobj = Services::cpp_global();

	obj_ptr self(this->self());

	sobj->set(LDRi.finder_str, finder);
	sobj->set(LDRi.loader_str, self);

	sobj->setObject(finder);
	sobj->setObject(self);
}


void 
Loader::destruct()
{
	unregLoader();
	finder_.init();
	extloader_.set_null();
}

/*
bool extload_fn(
    val_rc& result,
    val_rc& callme, 
    int argct, 
    zval* argv)
{
val_rc fname = {0};
val_ptr::string_bind(&fname, LDRi.invoke_fn);


//zend_result _call_user_function_impl(zval *object, zval *function_name, 
    //zval *retval_ptr, uint32_t param_count, zval params[], HashTable *named_params)
    zval fname = {0};
    val_ptr::string_bind(&fname, LDRi.invoke_fn);

    if (_call_user_function_impl( 
        callme, //object
        &fname,
        result,  // result storage
        argct, 
        argv,
        (HashTable*) nullptr // 
        ) != SUCCESS)
    {
        // TODO: exception message callme toString ??
        zend_throw_error(zend_ce_error, "Invalid callable");
        return false;
    }
    return true;
}
	*/
val_return 
Loader::require(str_ptr file)
{
	val_return result;

	if (!extloader_.ok())
	{
		result.error() << "Loader callable not set";
		return result;
	}

	if (!file_exists(file))
	{
		result.error() << "Loader::require " << file << " not found";
		return result;
	}
	zval path = {0};
	val_ptr::string_bind(&path, file);
	obj_ptr fn = extloader_.zobject();

	result.value_ = fn.call(str_ptr(LDRi.invoke_fn), &path);

	DebugLog* log = DebugLog::cpp_global();
	//bool ok = extload_fn(result.value_, extloader_, 1, &path);
	//zend_printf("Loader::require %s ", file.data());
	//showmem("by Extloader", result.value_);

	if (log)
	{
		log->dump("require", (zval*)(result.value_));
	}
	if (!result.value_.ok() && throwNotFound_)
	{	// Load function may throw anyway.

		result.error() << "Loader callable failed for " << file;
		return result;
	}
	
	if (record_) 
	{
		htab_rw wr(this->required_);
		wr.push_back(&path);
	}
	return result;
}


bool_return
Loader::load(str_ptr class_name)
{
	bool_return result;

	str_rc path;

	if (finder_.ok())
	{
		fn_params<1> fn(fdr_find_);

		val_ptr::string_bind(fn.argsptr(), class_name);

		path = fn.str();

		if (!path.ok())
		{
			if (throwNotFound_)
			{
				result.error() << "Class " << class_name << " not found";
			}
			result.value_ = false;
			return result;
		}
	}
	else {

		str_rc rname = str_replace(LDRi.ns_sep, LDRi.dir_sep, class_name);
		str_buf buf;

		buf << basedir_ << LDRi.dir_sep << rname << '.' << LDRi.php_ext;

		path = buf.zstr();
	}
//callable_fn(val_rc& result, val_rc& callme, int argct = 0, zval* argv = nullptr);
	val_return data = require(path);

	if (data.has_errors())
	{
		result.value_ = false;
		result = data.move_error();
	}

	return result;
}

bool_return
Loader::must_load(str_ptr class_name)
{
	//showstr("must_load: ", class_name);
	return load(class_name);
}

void 
Loader::setExtLoader(val_ptr exload) 
{ 
	extloader_ = exload; 
	//this->regLoader();
}

void 
Loader::regLoader()
{
	call_spl(LDRi.s_register);
	isRegistered_ = true;
}

void 
Loader::unregLoader()
{
	if (isRegistered_)
	{
		isRegistered_ = false;
		call_spl(LDRi.s_unregister);
	}
}

}; // namespace

using namespace wcc;
using namespace zpp;

ZEND_METHOD(Wcc_Loader, readPHP)
{
	val_return result;

	zarg_rd args(execute_data);

	str_ptr path;

	args.zstring(path, args.need(0));

	if (!args.throw_errors(__FUNCTION__))
	{
		result = Loader::readPHP(path);
		if (!result.throw_errors(__FUNCTION__))
		{
			result.value_.move_zv(return_value);
		}
	}
}

ZEND_METHOD(Wcc_Loader, instance)
{
	ZEND_PARSE_PARAMETERS_NONE();

	obj_ptr result = Loader::instance();

	result.copy_zv(return_value);
}

ZEND_METHOD(Wcc_Loader, __destruct)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Loader* lob = zval_toc<Loader>(ZEND_THIS);

	lob->destruct();	
}

ZEND_METHOD(Wcc_Loader, setThrowNotFound)
{
	zarg_rd args(execute_data);

	bool bval = true;

	args.zbool(bval, args.need(0));

	if (!args.throw_errors(__FUNCTION__))
	{
		Loader* lob = zval_toc<Loader>(ZEND_THIS);

		lob->setThrowNotFound(bval);	
	}
}
ZEND_METHOD(Wcc_Loader, setExtLoader)
{
	zarg_rd args(execute_data);

	val_ptr extfn = args.need(0);

	if (!args.throw_errors(__FUNCTION__))
	{
		Loader* lob = zval_toc<Loader>(ZEND_THIS);

		lob->setExtLoader(extfn);	
	}
}

ZEND_METHOD(Wcc_Loader, setFinder)
{
	zarg_rd args(execute_data);

	obj_ptr finder;

	args.obj_ofclass(finder, args.need(0), Finder::omg.classEntry());
	if (!args.throw_errors(__FUNCTION__))
	{
		Loader* lob = zval_toc<Loader>(ZEND_THIS);

		lob->setFinder(finder);	
	}

}
ZEND_METHOD(Wcc_Loader, getFinder)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Loader* lob = zval_toc<Loader>(ZEND_THIS);

	obj_ptr finder = lob->getFinder();

	finder.copy_zv(return_value);
}

ZEND_METHOD(Wcc_Loader, getLoaded)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Loader* lob = zval_toc<Loader>(ZEND_THIS);

	htab_ptr loaded = lob->getLoaded();

	loaded.copy_zv(return_value);
}

ZEND_METHOD(Wcc_Loader, getRequired)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Loader* lob = zval_toc<Loader>(ZEND_THIS);

	htab_ptr required = lob->getRequired();

	required.copy_zv(return_value);
}

ZEND_METHOD(Wcc_Loader, require)
{
	val_return result;

	zarg_rd args(execute_data);

	str_ptr path;

	args.zstring(path, args.need(0));

	if (!args.throw_errors(__FUNCTION__))
	{
		Loader* lob = zval_toc<Loader>(ZEND_THIS);

		result = lob->require(path);

		if (!result.throw_errors(__FUNCTION__))
		{
			result.value_.move_zv(return_value);
		}	
	}
	
}

ZEND_METHOD(Wcc_Loader, load)
{
	bool_return result;

	zarg_rd args(execute_data);

	str_ptr path;

	args.zstring(path, args.need(0));

	if (!args.throw_errors(__FUNCTION__))
	{
		Loader* lob = zval_toc<Loader>(ZEND_THIS);

		result = lob->load(path);	

		if (!result.throw_errors(__FUNCTION__))
		{
			RETVAL_BOOL(result.value_);
		}
	}
}

ZEND_METHOD(Wcc_Loader, mustload)
{
	zarg_rd args(execute_data);

	str_ptr path;

	args.zstring(path, args.need(0));

	bool_return result;

	if (!args.throw_errors(__FUNCTION__))
	{
		Loader* lob = zval_toc<Loader>(ZEND_THIS);

		result = lob->must_load(path);	

		if (result.throw_errors(__FUNCTION__))
		{
			result.value_ = false;
		}
	}
	RETVAL_BOOL(result.value_);
}

ZEND_METHOD(Wcc_Loader, setRecord)
{
	zarg_rd args(execute_data);

	bool  value = true;

	args.zbool(value, args.need(0));

	if (!args.throw_errors(__FUNCTION__))
	{
		Loader* lob = zval_toc<Loader>(ZEND_THIS);
		lob->setRecord(value);
	}
}

ZEND_METHOD(Wcc_Loader, setBaseDir)
{
	zarg_rd args(execute_data);

	str_ptr path;

	args.zstring(path, args.need(0));

	if (!args.throw_errors(__FUNCTION__))
	{
		Loader* lob = zval_toc<Loader>(ZEND_THIS);

		lob->setBaseDir(path);	
	}

}

ZEND_METHOD(Wcc_Loader, getBaseDir)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Loader* lob = zval_toc<Loader>(ZEND_THIS);

	str_rc path = lob->getBaseDir();

	path.move_zv(return_value);
}

ZEND_METHOD(Wcc_Loader, regLoader)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Loader* lob = zval_toc<Loader>(ZEND_THIS);

	lob->regLoader();	
}
ZEND_METHOD(Wcc_Loader, unregLoader)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Loader* lob = zval_toc<Loader>(ZEND_THIS);

	lob->unregLoader();
}

PHP_MINIT_FUNCTION(wcc_loader_reg)
{
	Loader::omg.classEntry(register_class_Wcc_Loader());

	STATE_INIT_ADD(LDRi)
	
	return SUCCESS;
}

#endif
