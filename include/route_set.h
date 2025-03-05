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

public:
	htab_mgr fixed_;
	htab_mgr vary_;
	zstr_mgr file_;
	htab_mgr nameIndex_;

	static base_obj_mgr<RouteSet> omg;

	RouteSet();
	
	virtual ~RouteSet();

	virtual void debug_info(htab_write di);

	Route* match(RouteMatch* rm);
	
	htab_mgr serialize();
	void     unserialize(htab_read htab);

	void  addRouteList(zval_user list);
	void  addRoute(zval_user route);

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
};


};


#define ARG_S  "_arg"
#define FUN_S  "_fun"
#define FUNX_S "_fnx"
#define HITS_S "_hit"
#define MOD_S  "_mod"
#define NSP_S  "_nsp"
#define OBJ_S  "_obj"
#define OBJX_S "_obx"
#define ROLE_S "_rol"

#endif