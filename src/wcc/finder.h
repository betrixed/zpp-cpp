#ifndef WCC_FINDER_H
#define WCC_FINDER_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif


namespace wcc {

using namespace zpp;

class Finder_init : public state_init 
{
public:

	str_intern nspaths_key;
	str_intern classes_key;
	str_intern folders_key;
	str_intern php_ext;
	str_intern dir_sep;
	str_intern dir_dot;
	str_intern dir_two;
	str_intern glob_all;


	void init() override;

};

extern Finder_init  FDit;

class  Finder : public base_d {
protected:
	htab_empty nsPaths_;
	htab_empty classes_;
	htab_empty folders_;
public:
	
	static base_obj_mgr<Finder> omg;

	static htab_rc dirList_dir(str_ptr path);

	static htab_rc dirList_fileExt(str_ptr path, htab_ptr extlist);

	static str_rc  path_ext(str_ptr path); 

	static str_rc  path_base(str_ptr path); 
	
	static str_rc  find_extension(str_ptr fpath, htab_ptr extlist);

	virtual void debug_info(htab_cow hw);

	virtual ~Finder();
	
	void addFolder(str_ptr fsdir);
	void addPath(str_ptr nsroot, str_ptr fspath);

	error_return addPathArray(htab_ptr pathsArray);
	void addClass(str_ptr cname, str_ptr fspath);
	void addClasses(htab_ptr classArray);

	// properties
	htab_ptr getNSPaths() const;

	htab_ptr getClassPaths() const;

	htab_ptr getFolders() const;

	//! return path if found or null
	str_rc findClass(str_ptr cname );

	str_rc findLeaf(str_ptr leafpath);

};

};
//wcc_finder.h
#endif