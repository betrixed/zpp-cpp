#ifndef WCC_FINDER_H
#define WCC_FINDER_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif


namespace wcc {

class  Finder : public base_d {
protected:
	htab_empty nsPaths_;
	htab_empty classes_;
	htab_empty folders_;
public:
	
	static base_obj_mgr<Finder> omg;

	virtual void debug_info(htab_rw hw);

	virtual ~Finder();
	
	void addFolder(str_ptr fsdir);
	void addPath(str_ptr nsroot, str_ptr fspath);

	void addPathArray(htab_ptr pathsArray);
	void addClass(str_ptr cname, str_ptr fspath);
	void addClasses(htab_ptr classArray);

	// properties
	htab_ptr getNSPaths() const;

	htab_ptr getClassPaths() const;

	htab_ptr getFolders() const;

	//! return path if found or null
	str_rc find(str_ptr cname );

};

};
//wcc_finder.h
#endif