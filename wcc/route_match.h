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

	void set_tuple12(htab_read tg);
	void set_tuple14(htab_read tg);
	htab_mgr fetchArgs();

	zobj_user    testRoute(zobj_user robj);

	zobj_user    firstMatch(zval_user wrap);
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

	//public function __construct(string $uri, int $verb_flag, mixed $ajax_flag);
	void construct(zstr_user uri, int verbs, int ajax);

	//public function setCallInfo(string $obclass, string $obmethod, array $args) : void;

	void setCallInfo(zstr_user obclass, zstr_user obmethod, zval_user args);

	virtual void debug_info(htab_write di);

	zobj_user getMatch() const
	{
		return route_;
	}

	void setRoute(zobj_user ro)
	{
		route_ = ro;
	}
	
	htab_read getErrors() const
	{
		return errors_;
	}

	zstr_user getUri() const
	{
		return uri_;
	}

	htab_read getRoles() const
	{
		return roles_;
	}

	htab_read getObjArgs() const
	{
		return ob_args_;
	}

	bool find_route(RouteSet* routes);
	void error_context(Route* route);
	
	void setModuleName(zend_string* s)
	{
		module_name_ = s;
	}
	zstr_user getModuleName() const
	{
		return module_name_;
	}

	zstr_user getObjClass() const
	{
		return ob_class_;
	}

	zstr_user getObjMethod() const
	{
		return ob_method_;
	}

	bool prepare_call();

	zval_mgr call(htab_read extra, zobj_user before, zobj_user after);

#ifndef BASE_ZOBJPTR
	VIRTUAL_ZOBJPTR
#endif
};


}; // namespace

#endif