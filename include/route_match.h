#ifndef route_match_h
#define route_match_h

#ifndef WC_BASE_H
#include "wc_base.h"
#endif

#ifndef WCC_PAIR_H
#include "pair.h"
#endif

namespace wcc {

class Route;

class RouteMatch : public base_d {

protected:

	void set_tuple12(htab_ptr tg);
	void set_tuple14(htab_ptr tg);
	htab_own fetchArgs();
	Route*    testRoute(zend_object* robj);

	Route*    firstMatch(zval_own& wrap);
public:
	zobj_own route_;
	htab_own roles_;
	htab_own ob_args_;
	htab_own match_args_;
	htab_own errors_;

// string values
	zstr_own uri_;
	zstr_own module_name_;
	zstr_own ob_class_;
	zstr_own ob_method_;


	long verb_flag_;
	long ajax_flag_;

//  processing values
	zval_own target_; 
	zval_own result_;

	static base_obj_mgr<RouteMatch> omg;

	static zval_own call_method(zobj_ptr obj, zstr_ptr method, htab_ptr args);


	virtual void debug_info(HashTable *ht) override;

	

	const zobj_own& getMatch()
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

	

	zval_own invoke(htab_ptr extra, zobj_ptr before, zobj_ptr after);
};


}; // namespace

#endif