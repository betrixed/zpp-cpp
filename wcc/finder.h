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

	virtual void debug_info(htab_write hw) const;

	void addFolder(zstr_user fsdir);
	void addPath(zstr_user nsroot, zstr_user fspath);

	void addPathArray(htab_read pathsArray);
	void addClass(zstr_user cname, zstr_user fspath);

	// properties
	htab_read getNSPaths() const;

	htab_read getClassPaths() const;

	htab_read getFolders() const;

	//! return path if found or null
	zstr_mgr find(zstr_user cname );

};

};
//wcc_finder.h
#endif