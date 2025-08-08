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
	void indexItem(val_ptr obj);
	void indexRouteKey(obj_ptr obj);
	void copy_target(Route* r);
	
public:
	htab_rc fixed_;
	htab_rc vary_;
	str_rc file_;
	htab_rc nameIndex_;

	// default modifications for route targets
	str_rc method_sfx_;

	str_rc prefix_;
	str_rc module_name_;

	val_rc notFound_;

	static str_ptr rex_url();

	static base_obj_mgr<RouteSet> omg;

	RouteSet();
	
	virtual ~RouteSet();

	virtual void debug_info(htab_rw di);

	Route* match(RouteMatch* rm);

	bool compile(Route* route);
	
	void prefix(str_ptr start);
	void module(str_ptr name);
	void methodSfx(str_ptr name);
	void notFound(val_ptr target);
	
	htab_rc serialize();
	void     unserialize(htab_rd htab);

	void  addRouteList(htab_rd list, str_ptr prefix, str_ptr module);
	void  addRoute(obj_ptr route);

	obj_rc getRoute(str_ptr name);
	str_rc routeUrl(str_ptr name, htab_rd params);
	
	void setFile(str_ptr name) {
		file_ = name;
	}

	str_ptr getFile() const {
		return file_;
	}

	htab_rd getFixed() {
		return fixed_;
	}

	htab_rd getVary() {
		return vary_;
	}
	static void  appendRoute(htab_rw array, str_ptr key, Route* route);

	VIRTUAL_ZOBJPTR

};


};




#endif