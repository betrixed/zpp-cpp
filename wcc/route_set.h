#ifndef WCC_ROUTESET_H
#define WCC_ROUTESET_H


#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

#ifndef WCC_ROUTE_H
#include "route.h"
#endif

namespace wcc {

class  RouteMatch;

class   RouteSet : public base_d {
protected:

	void indexRoutes();
	void indexItem(zval_user obj);
	void indexRouteKey(zobj_user obj);
	void copy_target(Route* r);
	
public:
	htab_mgr fixed_;
	htab_mgr vary_;
	zstr_mgr file_;
	htab_mgr nameIndex_;

	// default modifications for route targets
	zstr_mgr method_sfx_;

	zstr_mgr prefix_;
	zstr_mgr module_name_;

	zval_mgr notFound_;

	static zstr_user rex_url();

	static base_obj_mgr<RouteSet> omg;

	RouteSet();
	
	virtual ~RouteSet();

	virtual void debug_info(htab_write di);

	Route* match(RouteMatch* rm);

	bool compile(Route* route);
	
	void prefix(zstr_user start);
	void module(zstr_user name);
	void methodSfx(zstr_user name);
	void notFound(zval_user target);
	
	htab_mgr serialize();
	void     unserialize(htab_read htab);

	void  addRouteList(htab_read list, zstr_user prefix, zstr_user module);
	void  addRoute(zobj_user route);

	zobj_mgr getRoute(zstr_user name);
	zstr_mgr routeUrl(zstr_user name, htab_read params);
	
	void setFile(zstr_user name) {
		file_ = name;
	}

	zstr_user getFile() const {
		return file_;
	}

	htab_read getFixed() {
		return fixed_;
	}

	htab_read getVary() {
		return vary_;
	}
	static void  appendRoute(htab_write array, zstr_user key, Route* route);

	VIRTUAL_ZOBJPTR

};


};




#endif