#ifndef ROUTE_MATCH_H
#define ROUTE_MATCH_H

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

	void set_tuple12(htab_rd tg);
	void set_tuple14(htab_rd tg);
	htab_rc fetchArgs();

	obj_ptr    testRoute(obj_ptr robj);

	obj_ptr    firstMatch(val_ptr wrap);
public:
	obj_rc route_;
	htab_rc roles_;
	htab_rc ob_args_;
	htab_rc match_args_;
	htab_rc errors_;

// string values
	str_rc uri_;
	str_rc module_name_;
	str_rc ob_class_;
	str_rc ob_method_;


	long verb_flag_;
	long ajax_flag_;

//  processing values
	val_rc target_; 
	val_rc result_;

	static base_obj_mgr<RouteMatch> omg;

	static val_rc call_method(obj_ptr obj, str_ptr method, htab_rd args);

	//public function __construct(string $uri, int $verb_flag, mixed $ajax_flag);
	void construct(str_ptr uri, int verbs, int ajax);

	//public function setCallInfo(string $obclass, string $obmethod, array $args) : void;

	void setCallInfo(str_ptr obclass, str_ptr obmethod, val_ptr args);

	virtual void debug_info(htab_rw di);

	obj_ptr getMatch() const
	{
		return route_;
	}

	void setRoute(obj_ptr ro)
	{
		route_ = ro;
	}
	
	htab_rd getErrors() const
	{
		return errors_;
	}

	str_ptr getUri() const
	{
		return uri_;
	}

	htab_rd getRoles() const
	{
		return roles_;
	}

	htab_rd getObjArgs() const
	{
		return ob_args_;
	}

	bool find_route(RouteSet* routes);
	void error_context(Route* route);
	
	void setModuleName(zend_string* s)
	{
		module_name_ = s;
	}
	str_ptr getModuleName() const
	{
		return module_name_;
	}

	str_ptr getObjClass() const
	{
		return ob_class_;
	}

	str_ptr getObjMethod() const
	{
		return ob_method_;
	}

	bool prepare_call();

	val_rc call(htab_rd extra, obj_ptr before, obj_ptr after);

#ifndef BASE_ZOBJPTR
	VIRTUAL_ZOBJPTR
#endif
};


}; // namespace

#endif