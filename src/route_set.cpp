#ifndef WCC_ROUTESET_CPP
#define WCC_ROUTESET_CPP

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif


#include "Zend/zend_smart_str.h"

#include "route_set.h"

#ifndef WCC_ROUTE_H
#include "route.h"
#endif

#ifndef ROUTE_MATCH_H
#include "route_match.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "ext/pcre/php_pcre.h"

#ifdef __cplusplus
};

#endif

namespace wcc {
 	base_obj_mgr<RouteSet> RouteSet::omg;



RouteSet::RouteSet() 
{	

}

RouteSet::~RouteSet() 
{	
}

htab_mgr RouteSet::serialize()
{
	htab_mgr result;

	htab_write hw(result);

	//showarray("fixed 1", fixed_);
	hw.set(route_data.cc_fixed, fixed_);
	//showarray("fixed 2", fixed_);

	hw.set(route_data.cc_vary, vary_);

	hw.set(route_data.cc_file, file_);

	return result;
}

void     
RouteSet::unserialize(htab_read hr)
{
	fixed_ = hr.get(route_data.cc_fixed);
	vary_ = hr.get(route_data.cc_vary);
	file_ = hr.get(route_data.cc_file);
}

void  RouteSet::addRoute(zobj_user obj)
{
	Route* ro = zobj_toc<Route>(obj);

	if (ro->hasParams())
	{
		appendRoute(vary_,  ro->compiled_, ro);
	}
	else {
		if (ro->pattern_.size() > 0) {
			appendRoute(fixed_ , ro->pattern_, ro);
		}
	}
}

void 
RouteSet::indexRouteKey(zobj_user ro)
{
	Route* route = zobj_toc<Route>(ro);
	zstr_user  key = route->id_;
	if (key.size())
	{	
		htab_write(nameIndex_).set(key, ro);
	}	
}

void 
RouteSet::indexItem(zval_user obj)
{
	if (obj.isArray())
	{
		htab_walk wk;
		auto ro = wk.value();
		for( wk.start(obj.zarray()); wk.ok(); wk.next())
		{
			indexRouteKey(ro);
		}
	}
	else if (obj.isObject())
	{
		indexRouteKey(obj);
	}
}

void
RouteSet::indexRoutes()
{
	htab_walk wk;
	auto robj = wk.value();
	for( wk.start(fixed_); wk.ok(); wk.next())
	{
		indexItem(robj);
	}
	for( wk.start(vary_); wk.ok(); wk.next())
	{
		indexItem(robj);
	}
}

zobj_mgr
RouteSet::getRoute(zstr_user name)
{

	zobj_mgr result;
	htab_read idx(nameIndex_);

	if (idx.size()==0) 
	{
		this->indexRoutes();
	}
	// may have created HashTable*
	idx = nameIndex_;

	if (idx.size() > 0)
	{
		result = idx.get((zend_string*) name);
	}
	return result;
}


void  RouteSet::addRouteList(zval_user list)
{
	//showmem(" list: ", list);
	htab_walk walk;

	int loopct = 0;

	auto route = walk.value();
	
	for(walk.start(list.zarray()); walk.ok(); walk.next())
	{
		//zend_printf("rlist iterate ");
		//showmem(" route: ", (zval*) route);
		loopct++;
		if (route.isObject())
		{
			zobj_user obj = route.zobject();
			if (!Route::omg.myType(obj))
				continue;
			this->addRoute(route);
		}

	}
	return;
}

void RouteSet::appendRoute(
	htab_write rarr, 
	zstr_user key, 
	Route* myroute)
{
	zval_mgr store(myroute->zobj());

	zval_user  zip;

	if (!rarr.try_fetch(key, zip))
	{
		// first time, store as object
		rarr.set(key, store);
		return;
	}
	
	if (zip.isObject())
	{
		// Already one stored as object, convert to list of 2 objects
		htab_mgr sublist;

		htab_write hw(sublist);

		hw.push_back(zip); 
		hw.push_back(store);

		store = sublist;

		rarr.set(key,store);

	}
	else if (zip.isArray())
	{
		// Add to list of objects, add object
		htab_write sublist(zip);
		sublist.push_back(store);
	}

	return;
}

void RouteSet::debug_info(htab_write hw)
{
	hw.set(route_data.cc_fixed, fixed_);

	hw.set(route_data.cc_vary, vary_);

	hw.set(route_data.cc_file, file_);

}

zstr_mgr 
RouteSet::routeUrl(zstr_user name, htab_read params)
{
	zstr_mgr result;

	zobj_mgr route = getRoute(name);

	if (!route.isNull())
	{
		result = zobj_toc<Route>(route)->routeUrl(params);
	}
	return result;
}

}; // end namespace wcc - @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

PHP_METHOD(Wcc_RouteSet, __serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RouteSet* cobj = zval_toc<RouteSet>(ZEND_THIS);

	htab_mgr ret = cobj->serialize();
	ret.move_zv(return_value);
}

PHP_METHOD(Wcc_RouteSet, __unserialize)
{
	zval* data;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ARRAY(data)
	ZEND_PARSE_PARAMETERS_END();

	htab_read hw(Z_ARR_P(data));

	RouteSet*   cobj = zval_toc<RouteSet>(ZEND_THIS);

	cobj->unserialize(hw);
	return;
}

PHP_METHOD(Wcc_RouteSet, setFile) 
{
	zend_string* name;
	 
	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	RouteSet* cobj = zval_toc<RouteSet>(ZEND_THIS);
	cobj->setFile(name);
}

PHP_METHOD(Wcc_RouteSet, getFile)
{
	ZEND_PARSE_PARAMETERS_NONE();
	
	RouteSet* cobj = zval_toc<RouteSet>(ZEND_THIS);
	zstr_user file = cobj->getFile();

	file.return_zv(return_value);
}

PHP_METHOD(Wcc_RouteSet, getFixed)
{
	ZEND_PARSE_PARAMETERS_NONE();
	
	RouteSet* cobj = zval_toc<RouteSet>(ZEND_THIS);
	htab_read fixed =cobj->getFixed();

	fixed.return_zv(return_value);
}

PHP_METHOD(Wcc_RouteSet, getVary)
{
	ZEND_PARSE_PARAMETERS_NONE();
	
	RouteSet* cobj = zval_toc<RouteSet>(ZEND_THIS);

	htab_read vary = cobj->getVary();

	vary.return_zv(return_value);
}

PHP_METHOD(Wcc_RouteSet, addRouteList)
{
	zval 	*list;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY(list)
	ZEND_PARSE_PARAMETERS_END();

	RouteSet* cobj = zval_toc<RouteSet>(ZEND_THIS);

	//showmem("addRoutes", ZEND_THIS);

	cobj->addRouteList(list);

}

PHP_METHOD(Wcc_RouteSet, addRoute)
{
	zval 	*robj;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(robj, Route::omg.classEntry())
	ZEND_PARSE_PARAMETERS_END();

	RouteSet* cobj = zval_toc<RouteSet>(ZEND_THIS);

	//showmem("addRoutes", ZEND_THIS);


	cobj->addRoute(robj);

}

PHP_METHOD(Wcc_RouteSet, getRoute)
{
	zend_string 	*name;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	RouteSet* cobj = zval_toc<RouteSet>(ZEND_THIS);

	//showmem("addRoutes", ZEND_THIS);
	zobj_mgr robj = cobj->getRoute(name);

	robj.move_zv(return_value);

}

PHP_METHOD(Wcc_RouteSet, routeUrl)
{
	zend_string*	name;
	zval*           params = nullptr;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(name)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY(params)
	ZEND_PARSE_PARAMETERS_END();

	RouteSet* cobj = zval_toc<RouteSet>(ZEND_THIS);

	//showmem("addRoutes", ZEND_THIS);
	htab_read plist;
	if (params) {
		plist = params;
	}
	zstr_mgr robj = cobj->routeUrl(name, plist);

	robj.move_zv(return_value);

}


PHP_MINIT_FUNCTION(wcc_routeset_d)
{
	//WcR_ce = wcc_class_reg("Wc\\Route", class_Wcc_Route_methods);
	auto ce = register_class_Wcc_RouteSet();

	RouteSet::omg.classEntry(ce);

	class_data cval(ce);

	cval.add_constant("ARG_S", route_data.ARG_S);


	cval.add_constant("FUN_S", route_data.FUN_S);
	cval.add_constant("FUNX_S", route_data.FUNX_S);
	cval.add_constant("HITS_S", route_data.HITS_S);
	cval.add_constant("MOD_S", route_data.MOD_S);
	cval.add_constant("NSP_S", route_data.NSP_S);
	cval.add_constant("OBJ_S", route_data.OBJ_S);
	cval.add_constant("OBJX_S", route_data.OBJX_S);
	cval.add_constant("ROLE_S", route_data.ROLE_S);

	cval.add_constant("KEY", "@key");
	cval.add_constant("ROUTES", "routes");
	cval.add_constant("NO_PREFIX", "noprefix");
	

	return SUCCESS;

}

#endif