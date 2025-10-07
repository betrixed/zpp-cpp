#ifndef WCC_LOADER_CPP
#define WCC_LOADER_CPP

#ifndef WCC_LOADER_H
#include "loader.h"
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

thread_local obj_rc     gLoader;

class Loader_init : public state_init {
public:
	str_intern s_mustload;
	str_intern s_register;
	str_intern s_unregister;
	str_intern s_find;
	str_intern ns_sep;
	str_intern dir_sep;
	str_intern php_ext;



	void init() override;

	void init_req() override;
	void end_req() override;

};

Loader_init LDRi;

void Loader_init::init()
{
	s_mustload = "mustload";
	s_register = "spl_autoload_register";
	s_unregister = "spl_autoload_unregister";
	s_find = "find";
	ns_sep = "\\";
	dir_sep = "/";
	php_ext = "php";
}



void Loader_init::init_req()
{
	gLoader = Loader::omg.new_zobj();
}

void Loader_init::end_req()
{
	gLoader.init();
}

void Loader::setFinder(obj_ptr finder) 
{
	finder_ = finder;
	fdr_find_.set_fci(finder, LDRi.s_find);
}

void
Loader::call_spl(str_ptr fname)
{
	htab_rc cfn;
	//showstr("call method", fname);

	htab_rw hw(cfn);

	hw.push_back(this->self());
	hw.push_back(LDRi.s_mustload);

	fn_call_args<1> fn;
	fn.set_fname(fname);

	ZVAL_ARR(fn.argsptr(), cfn);
	fn.call_fn();


}

Loader::Loader() : base_d()
{
	isRegistered_ = false;
	throwNotFound_ = false;
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

val_rc //static
Loader::readPHP(str_ptr path)
{
	Loader* lob = Loader::cpp_global();
	return lob->require(path);
}

void 
Loader::destruct()
{
	unregLoader();
	finder_.init();
	extloader_.set_null();
}

val_rc 
Loader::require(str_ptr file)
{
	val_rc data;

	if (!extloader_.ok())
	{
		zend_throw_error(zend_ce_error, "No callable function installed");
		return data;
	}
	val_rc path(file);
	//showmem("ExtLoader", extloader_);
	//showmem("path", path);

	bool result = callable_fn(data, extloader_, 1, path);
	if (!result)
	{
		zend_printf("Callable failed\n");
		//data.set_null();
	}
	return data;
}

bool 
Loader::load(str_ptr class_name)
{
	str_rc path;

	if (finder_.ok())
	{
		ZVAL_STR(fdr_find_.argsptr(), class_name);
		path = fdr_find_.call_fn();
		if (!path.ok())
		{
			return false;
		}
	}
	else {

		str_rc rname = str_replace(LDRi.ns_sep, LDRi.dir_sep, class_name);
		str_buf buf;

		buf << basedir_ << LDRi.dir_sep << rname << '.' << LDRi.php_ext;

		path = buf.zstr();
	}
//callable_fn(val_rc& result, val_rc& callme, int argct = 0, zval* argv = nullptr);
	val_rc data = require(path);

	bool result = data.ok();
	return result;
}

bool 
Loader::must_load(str_ptr class_name)
{
	if (!load(class_name))
	{
		if (throwNotFound_)
		{
			zend_throw_error(zend_ce_error, "Loader: class not found - %s", class_name.data());
			return false;
		}
	}
	return true;
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
	zarg_rd args(execute_data);

	str_ptr path;

	args.zstring(path, args.need(1));

	val_rc result;

	if (!args.throw_errors())
	{
		result = Loader::readPHP(path);
	}
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Loader, instance)
{
	ZEND_PARSE_PARAMETERS_NONE();

	obj_ptr result = Loader::instance();

	result.return_zv(return_value);
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

	args.zbool(bval, args.need(1));

	if (!args.throw_errors())
	{
		Loader* lob = zval_toc<Loader>(ZEND_THIS);

		lob->setThrowNotFound(bval);	
	}
}
ZEND_METHOD(Wcc_Loader, setExtLoader)
{
	zarg_rd args(execute_data);

	val_ptr extfn = args.need(1);

	if (!args.throw_errors())
	{
		Loader* lob = zval_toc<Loader>(ZEND_THIS);

		lob->setExtLoader(extfn);	
	}
}

ZEND_METHOD(Wcc_Loader, setFinder)
{
	zarg_rd args(execute_data);

	obj_ptr finder;

	args.obj_ofclass(finder, args.need(1), Finder::omg.classEntry());
	if (!args.throw_errors())
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

	finder.return_zv(return_value);
}

ZEND_METHOD(Wcc_Loader, require)
{
	zarg_rd args(execute_data);

	str_ptr path;

	args.zstring(path, args.need(1));

	val_rc result;

	if (!args.throw_errors())
	{
		Loader* lob = zval_toc<Loader>(ZEND_THIS);

		result = lob->require(path);	
	}
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Loader, load)
{
	zarg_rd args(execute_data);

	str_ptr path;

	args.zstring(path, args.need(1));

	bool result = false;

	if (!args.throw_errors())
	{
		Loader* lob = zval_toc<Loader>(ZEND_THIS);

		result = lob->load(path);	
	}
	RETVAL_BOOL(result);
}

ZEND_METHOD(Wcc_Loader, mustload)
{
	zarg_rd args(execute_data);

	str_ptr path;

	args.zstring(path, args.need(1));

	bool result = false;

	if (!args.throw_errors())
	{
		Loader* lob = zval_toc<Loader>(ZEND_THIS);

		result = lob->must_load(path);	
	}
	RETVAL_BOOL(result);
}

ZEND_METHOD(Wcc_Loader, setBaseDir)
{
	zarg_rd args(execute_data);

	str_ptr path;

	args.zstring(path, args.need(1));

	if (!args.throw_errors())
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
	return SUCCESS;
}

#endif
