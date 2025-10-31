#ifndef WCC_FINDER_CPP
#define WCC_FINDER_CPP


#ifndef WCC_FINDER_H
#include "finder.h"
#endif

#ifndef WCC_FINDER_ARGINFO
#define WCC_FINDER_ARGINFO
extern "C" {
	#include "stub/finder_arginfo.h"
}

#endif

#include <filesystem>


namespace wcc {

using namespace zpp;

base_obj_mgr<Finder> Finder::omg;

class Finder_init : public state_init 
{
public:
	Finder_init() : state_init() {}

	str_intern nspaths_key;
	str_intern classes_key;
	str_intern folders_key;
	str_intern php_ext;
	str_intern dir_sep;
	str_intern dir_dot;
	str_intern dir_two;



	void init() override 
	{
		nspaths_key = "nspaths";
		classes_key = "classes";
		folders_key = "folders";
		dir_sep = "/";
		php_ext = ".php";
		dir_dot = ".";
		dir_two = "..";
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
Finder::debug_info(htab_rw d)
{

	d.set(FDit.nspaths_key, nsPaths_);
	d.set(FDit.classes_key, classes_);
	d.set(FDit.folders_key, folders_);
	//showarray("Finder::debug_info", d);
}

void 
Finder::addFolder(str_ptr fsdir)
{
	htab_rw(folders_).push_back(fsdir);
}

void 
Finder::addPath(str_ptr nsroot, str_ptr fspath)
{

	htab_rw(nsPaths_).set(nsroot, fspath);
}

void 
Finder::addPathArray(htab_ptr pathsArray)
{
	htab_rw hw(nsPaths_);

	for_key_value wk;
	for(wk.start(pathsArray); wk.ok(); wk.next())
	{
		hw.set(wk.key(), wk.value());
	}

	//showarray("addPathArray", nsPaths_);
}

void 
Finder::addClasses(htab_ptr classArray)
{
	
	htab_rw hw(classes_);

	for_key_value wk;
	for(wk.start(classArray); wk.ok(); wk.next())
	{
		hw.set(wk.key(), wk.value());
	}
}

void 
Finder::addClass(str_ptr cname, str_ptr fspath)
{
	htab_rw(classes_).set(cname, fspath);
}

// properties
htab_ptr 
Finder::getNSPaths() const
{
	return nsPaths_;
}

htab_ptr 
Finder::getClassPaths() const
{
	return classes_;
}

htab_ptr 
Finder::getFolders() const
{
	return folders_;
}

str_rc 
Finder::find(str_ptr cname)
{
	str_rc result;
	str_rc test_path;
	str_rc file_name;
	str_rc ns_key;
	str_rc sub_path;

	//showstr("Find class", cname);
	result = htab_ptr(classes_).get(cname);
	if (result.size()) {
		return result;
	}

	size_t cname_len = cname.size();
	int epos = cname_len;
	int ipos = epos + 1;

	std::string_view vcname = cname.vstr();
	str_rc filepath(FDit.php_ext);

	str_buf buf;

	//bool nsFound = false;

	htab_ptr ns_array(nsPaths_);
	bool nsFound = false;

	while(true)
	{	
		epos = ipos-2; // search for next backslash going backwards
		if (epos < 0)
		{
			break;
		}
		size_t bs_pos = vcname.rfind('\\', epos);
		bool found = (bs_pos != std::string_view::npos);
		ipos = found ? (int) bs_pos+1 : 0; // character position after bs, or 0

		//zend_printf(" ipos %d epos %d\n", epos,  ipos);
		if (!nsFound)
		{
			nsFound = found;
			// first time loop
			buf << cname.substr(ipos) << FDit.php_ext;
			file_name = buf.zstr(); 
			sub_path =  FDit.dir_sep;
			//showstr("first sub_path", sub_path);
		}
		else {
			int seglen = epos - ipos + 1;

			
			std::string_view seg = vcname.substr(ipos, seglen);

			buf << FDit.dir_sep << seg << sub_path;
			sub_path = buf.zstr();
			//showstr("2nd sub_path", sub_path);
		}
		
		if (nsFound) {
			size_t keylen = found ? bs_pos : 0;
			if (keylen) 
			{
				ns_key = cname.substr(0,keylen);
				//showstr("ns_key", ns_key);
				str_ptr ns_path = ns_array.get(ns_key);

				if (ns_path.ok())
				{
					// but what about the
					buf << ns_path << sub_path << file_name;
					test_path = buf.zstr();
					
					//showstr("test path", test_path);
					if (std::filesystem::exists(test_path.vstr())) {
						result = std::move(test_path);
						break;
					}
				}
			}
			else {

				return result;
			}
			// loop again
		}
		else {
			// ipos == 0, root class file_name is in a folder?
			htab_walk fwk;
			auto folder = fwk.value();
			for(fwk.start(folders_); fwk.ok(); fwk.next())
			{
				buf << folder.zstr() << FDit.dir_sep << filepath;
				test_path = buf.zstr();

				if (std::filesystem::exists(test_path.vstr())) 
				{
					result = std::move(test_path);
					return result;
				}
			}
		}
	}
	return result;
}



str_rc  
Finder::path_base(str_ptr path)
{
	str_rc result;

	auto psize = path.size();

	if (psize)
	{
		zend_string* fname = php_basename(path.data(), psize, nullptr, 0);
		result.adopt(fname);
	}

	return result;
}

str_rc  
Finder::path_ext(str_ptr path)
{
	str_rc result;

	if (path.size())
	{
		str_rc base = path_base(path);

		int pos = base.rfind('.');

		if (pos >= 0)
		{
			result = base.substr(pos+1);
		}
	}
	return result;
}

htab_rc
Finder::dirList_dir(str_ptr path)
{
	htab_rc result = htab_ptr::empty_array();

	if (!is_dir(path))
	{
		return result;
	}

	val_rc dh = opendir(path);

	if (dh.isFalse()) 
	{
		return result;
	}

	htab_rw list(result);
	while(true)
	{
		val_rc entry = readdir(dh);
		if (entry.isFalse())
		{
			break;
		}
		str_rc value = entry.zstr();
		if (!zs_cmp(value, FDit.dir_dot) || !zs_cmp(value, FDit.dir_two))
		{
			continue;
		}
		str_buf jpath;

		jpath << path << FDit.dir_sep << value;

		str_rc tdir = jpath.zstr();
		if (is_dir(tdir))
		{
			list.set(value, tdir);
		}
	}
	closedir(dh);

	return result;
}

// return list of files with extensions in found order
htab_rc //static 
Finder::dirList_fileExt(str_ptr path, htab_ptr extlist)
{
	htab_rc result = htab_ptr::empty_array();

	if (!is_dir(path))
	{
		return result;
	}

	val_rc dh = opendir(path);


	if (dh.isFalse()) 
	{
		return result;
	}

	htab_rw list(result);
	while(true)
	{
		val_rc entry = readdir(dh);
		if (entry.isFalse())
		{
			break;
		}

		str_ptr fname = entry.zstr();

		if (extlist.size())
		{
			str_rc extname = path_ext(fname);
			extname.lowercase();

			if (extlist.value_index(extname) < 0)
			{
				continue;
			}
			list.push_back(entry);
		}
	}
	closedir(dh);
	return result;
}
}; //namespace wcc

using namespace wcc;
using namespace zpp;

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

ZEND_METHOD(Wcc_Finder, addClasses)
{
	zval* clist;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(clist)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Finder>(ZEND_THIS);

	cobj->addClasses(clist);	
}

ZEND_METHOD(Wcc_Finder, getNSPaths)
{
	ZEND_PARSE_PARAMETERS_START(0,0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Finder>(ZEND_THIS);
	htab_ptr result = cobj->getNSPaths();

	result.return_zv(return_value);
}

ZEND_METHOD(Wcc_Finder, getClassPaths)
{
	ZEND_PARSE_PARAMETERS_START(0,0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Finder>(ZEND_THIS);
	htab_ptr result = cobj->getClassPaths();

	result.return_zv(return_value);	
}

ZEND_METHOD(Wcc_Finder, getFolders)
{
	ZEND_PARSE_PARAMETERS_START(0,0)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Finder>(ZEND_THIS);
	htab_ptr result = cobj->getFolders();

	result.return_zv(return_value);	
}

ZEND_METHOD(Wcc_Finder, find)
{
	zend_string* cname;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(cname)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Finder>(ZEND_THIS);

	str_rc result = cobj->find(cname);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Finder, dirList_dir)
{
	zarg_rd args(execute_data);

	str_ptr path;
	htab_rc result;

	args.zstring(path, args.need(0));

	if (!args.throw_errors())
	{
		result = Finder::dirList_dir(path);
	}
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Finder, dirList_fileExt)
{
	zarg_rd args(execute_data);

	str_ptr path;
	htab_ptr extlist;

	htab_rc result;

	args.zstring(path, args.need(0));
	args.zarray_null(extlist, args.option(1));

	if (!args.throw_errors())
	{
		result = Finder::dirList_fileExt(path,extlist);
	}
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