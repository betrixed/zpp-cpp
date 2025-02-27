#ifndef WCC_ROUTESET_H
#define WCC_ROUTESET_H


#ifndef WC_BASE_H
#include "wc_base.h"
#endif

#ifndef WCC_ROUTE_H
#include "route.h"
#endif

namespace wcc {

class  RouteMatch;

class   RouteSet : public base_d {
protected:

	void indexRoutes();
	void indexItem(const zval_own &obj);
	void indexRouteKey(Route* ro);

public:
	htab_own fixed_;
	htab_own vary_;
	zstr_own file_;
	htab_own nameIndex_;

	static base_obj_mgr<RouteSet> omg;

	RouteSet();
	
	virtual ~RouteSet();

	virtual void debug_info(HashTable* ht);

	Route* match(RouteMatch* rm);
	
	htab_own serialize();
	void     unserialize(htab_ptr htab);

	void  addRouteList(zval* list);
	void  addRoute(Route* route);

	zobj_ptr getRoute(zstr_ptr name);
	zstr_own routeUrl(zstr_ptr name, htab_ptr params);
	
	static void  appendRoute(htab_own& array, zstr_own& key, Route* route);
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