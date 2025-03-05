#ifndef route_match_h
#define route_match_h

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

#ifndef WCC_PAIR_H
#include "pair.h"
#endif

namespace wcc {

class Route;

class RouteMatch : public base_d {

protected:

	void set_tuple12(htab_read tg);
	void set_tuple14(htab_read tg);
	htab_mgr fetchArgs();

	Route*    testRoute(zval_user robj);

	Route*    firstMatch(zval_user wrap);
public:
	zobj_mgr route_;
	htab_mgr roles_;
	htab_mgr ob_args_;
	htab_mgr match_args_;
	htab_mgr errors_;

// string values
	zstr_mgr uri_;
	zstr_mgr module_name_;
	zstr_mgr ob_class_;
	zstr_mgr ob_method_;


	long verb_flag_;
	long ajax_flag_;

//  processing values
	zval_mgr target_; 
	zval_mgr result_;

	static base_obj_mgr<RouteMatch> omg;

	static zval_mgr call_method(zobj_user obj, zstr_user method, htab_read args);


	virtual void debug_info(htab_write di) override;

	zobj_user getMatch()
	{
		return route_;
	}

	bool find_route(RouteSet* routes);
	void error_context(Route* route);
	
	void setModuleName(zend_string* s)
	{
		module_name_ = s;
	}

	bool prepare_call();

	zval_mgr invoke(htab_read extra, zobj_user before, zobj_user after);
};


}; // namespace

#endif