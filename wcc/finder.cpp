#ifndef WCC_FINDER_CPP
#define WCC_FINDER_CPP


#ifndef WCC_FINDER_H
#include "finder.h"
#endif

#include <filesystem>

namespace wcc {

base_obj_mgr<Finder> Finder::omg;

class Finder_init : public state_init 
{
public:
	Finder_init() : state_init() {}

	zstr_intern nspaths_key;
	zstr_intern classes_key;
	zstr_intern folders_key;
	zstr_intern php_ext;

	char dir_sep;

	void init() override 
	{
		nspaths_key = "nspaths";
		classes_key = "classes";
		folders_key = "folders";
		dir_sep = '/';
		php_ext = ".php";
	}

};

Finder_init  FDit;

using namespace zpp;

Finder::~Finder()
{
	//zend_printf("~Finder ");
	//showarray("nsPaths_", nsPaths_);
}

void 
Finder::debug_info(htab_write d)
{

	d.set(FDit.nspaths_key, nsPaths_);
	d.set(FDit.classes_key, classes_);
	d.set(FDit.folders_key, folders_);
	//showarray("Finder::debug_info", d);
}

void 
Finder::addFolder(zstr_user fsdir)
{
	htab_write(folders_).push_back(fsdir);
}

void 
Finder::addPath(zstr_user nsroot, zstr_user fspath)
{

	htab_write(nsPaths_).set(nsroot, fspath);
}

void 
Finder::addPathArray(htab_read pathsArray)
{
	for_key_value wk;
	htab_write hw(nsPaths_);

	for(wk.start(pathsArray); wk.ok(); wk.next())
	{
		hw.set(wk.key(), wk.value());
	}

	//showarray("addPathArray", nsPaths_);
}

void 
Finder::addClass(zstr_user cname, zstr_user fspath)
{
	htab_write(classes_).set(cname, fspath);
}

// properties
htab_read 
Finder::getNSPaths() const
{
	return nsPaths_;
}

htab_read 
Finder::getClassPaths() const
{
	return classes_;
}

htab_read 
Finder::getFolders() const
{
	return folders_;
}

zstr_mgr 
Finder::find(zstr_user cname)
{
	zstr_mgr result;
	zstr_mgr test_path;

	result = htab_read(classes_).get(cname);
	if (result.size()) {
		return result;
	}

	size_t epos = cname.size();

	std::string_view vcname = cname.vstr();
	zstr_mgr filepath(FDit.php_ext);

	zstr_buffer buf;

	//bool nsFound = false;

	htab_read ns_array(nsPaths_);

	while(true)
	{
		size_t pos = vcname.rfind('\\', epos);

		if (pos == std::string_view::npos) 
		{
			{
				buf << vcname.substr(0, epos) << filepath;
				filepath = buf.zstr();

				for_key_value fwk;

				for(fwk.start(folders_); fwk.ok(); fwk.next())
				{
					buf << fwk.value() << FDit.dir_sep << filepath;

					test_path = buf.zstr();

					//showstr("test path", test);
					if (std::filesystem::exists(test_path.vstr())) 
					{
						result = std::move(test_path);
						return result;
					}
				}
			}
			break;
		}
		
		buf <<  FDit.dir_sep << vcname.substr(pos+1, epos-pos) << filepath;
		filepath = buf.zstr();

		//showstr("filepath", filepath);

		epos = pos - 1;

		zstr_user nspath = ns_array.get(vcname.substr(0,pos));

		if (!nspath.isNull()) 
		{
			//showmem("nspath", nspath);
			buf << nspath << filepath;
			test_path = buf.zstr();
			
			//showstr("test path", test);
			if (std::filesystem::exists(test_path.vstr())) {
				//showstr("exists", test);
				result = std::move(test_path);
				break;
			}
		}
	}
	return result;
}

}; //namespace wcc

using namespace wcc;

ZEND_METHOD(Wcc_Finder, addFolder)
{
	zend_string* fspath;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(fspath)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Finder>(ZEND_THIS);

	cobj->addFolder(fspath);

}

ZEND_METHOD(Wcc_Finder, addPath)
{
	zend_string* nsroot;
	zend_string* fspath;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_STR(nsroot)
	Z_PARAM_STR(fspath)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Finder>(ZEND_THIS);

	cobj->addPath(nsroot, fspath);
}

ZEND_METHOD(Wcc_Finder, addPathArray)
{
	zval* paths;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(paths)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Finder>(ZEND_THIS);

	cobj->addPathArray(paths);
}

ZEND_METHOD(Wcc_Finder, addClass)
{
	zend_string* cname;
	zend_string* fspath;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_STR(cname)
	Z_PARAM_STR(fspath)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Finder>(ZEND_THIS);

	cobj->addClass(cname, fspath);	
}

ZEND_METHOD(Wcc_Finder, getNSPaths)
{
	ZEND_PARSE_PARAMETERS_START(0,0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Finder>(ZEND_THIS);
	htab_read result = cobj->getNSPaths();

	result.return_zv(return_value);
}

ZEND_METHOD(Wcc_Finder, getClassPaths)
{
	ZEND_PARSE_PARAMETERS_START(0,0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Finder>(ZEND_THIS);
	htab_read result = cobj->getClassPaths();

	result.return_zv(return_value);	
}

ZEND_METHOD(Wcc_Finder, getFolders)
{
	ZEND_PARSE_PARAMETERS_START(0,0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Finder>(ZEND_THIS);
	htab_read result = cobj->getFolders();

	result.return_zv(return_value);	
}

ZEND_METHOD(Wcc_Finder, find)
{
	zend_string* cname;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(cname)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Finder>(ZEND_THIS);

	zstr_mgr result = cobj->find(cname);
	result.move_zv(return_value);
}

PHP_MINIT_FUNCTION(Wcc_Finder_reg)
{
	auto ce = register_class_Wcc_Finder();

	Finder::omg.classEntry(ce);

	return SUCCESS;
}

//wcc_finder.cpp
#endif