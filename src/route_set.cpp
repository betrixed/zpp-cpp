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

#include "route_match.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "ext/pcre/php_pcre.h"

#ifdef __cplusplus
};

#endif

namespace wcc {
 	base_obj_mgr<RouteSet> RouteSet::omg;


static const char* cc_fixed = "fixed";
static const char* cc_vary = "vary";
static const char* cc_file = "file";


RouteSet::RouteSet() 
{	

}

RouteSet::~RouteSet() 
{	
}

htab_own RouteSet::serialize()
{
	htab_own hw;

	//showarray("fixed 1", fixed_);
	hw.set(route_data.cc_fixed, fixed_);
	//showarray("fixed 2", fixed_);

	hw.set(route_data.cc_vary, vary_);

	hw.set(route_data.cc_file, file_);

	return hw;
}

void     
RouteSet::unserialize(htab_ptr hw)
{
	zval_own temp;

	if (hw.try_fetch(route_data.cc_fixed, temp))
	{
		fixed_ = std::move(temp);
	}
	if (hw.try_fetch(route_data.cc_vary, temp))
	{
		vary_ = std::move(temp);
	}
	if (hw.try_fetch(route_data.cc_file, temp))
	{
		file_ = temp.zstr();
	}

	//fixed_.show_data("fixed all");
	//vary_.show_data("vary all");
}

void  RouteSet::addRoute(Route* ro)
{
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

void RouteSet::indexRouteKey(Route* ro)
{
	zstr_ptr  key = ro->id_;
	if (key.size())
	{
		nameIndex_.set(key, ro->zobj());
	}	
}

void RouteSet::indexItem(const zval_own &obj)
{
	if (obj.isArray())
	{
		htab_walk wk;
		auto& ro = wk.value();
		for( wk.start(obj.zarray()); wk.ok(); wk.next())
		{
			indexRouteKey(zval_toc<Route>(ro));
		}
	}
	else if (obj.isObject())
	{
		indexRouteKey(zval_toc<Route>(obj));
	}
}

void
RouteSet::indexRoutes()
{
	htab_walk wk;
	auto& robj = wk.value();
	for( wk.start(fixed_); wk.ok(); wk.next())
	{
		indexItem(robj);
	}
	for( wk.start(vary_); wk.ok(); wk.next())
	{
		indexItem(robj);
	}
}

zobj_ptr
RouteSet::getRoute(zstr_ptr name)
{

	zobj_ptr result;
	if (nameIndex_.size()==0) 
	{
		this->indexRoutes();
	}
	if (nameIndex_.size() > 0)
	{
		result = nameIndex_.get(name);
	}
	return result;
}


void  RouteSet::addRouteList(zval* list)
{
	//showmem(" list: ", list);
	zval_ptr wzp(list);

	htab_walk walk;
	int loopct = 0;

	auto& route = walk.value();
	
	for(walk.start(wzp.zarray()); walk.ok(); walk.next())
	{
		//zend_printf("rlist iterate ");
		//showmem(" route: ", (zval*) route);
		loopct++;
		if (route.isObject())
		{
			zend_object* obj = route.zobject();
			if (!route_mgr.myType(obj))
				continue;
			this->addRoute( zobj_toc<Route>(obj) );
		}

	}
	return;
}

void RouteSet::appendRoute(
	htab_own& rarr, 
	zstr_own& key, 
	Route* myroute)
{
	zval_ptr  zip;

	zval_own robj(myroute->zobj());

	if (!rarr.try_fetch(key, zip))
	{
		rarr.set(key, robj);
		return;
	}
	
	if (zip.isObject())
	{
		htab_own sublist;

		sublist.push_back(zip); 
		sublist.push_back(robj);

		zval_own temp(sublist);

		rarr.set(key,temp);

	}
	else if (zip.isArray())
	{
		htab_ptr sublist(zip);
		sublist.push_back(robj);
	}

	return;
}

void RouteSet::debug_info(HashTable *ht)
{
	htab_ptr hw(ht);

	hw.set(cc_fixed, fixed_);

	hw.set(cc_vary, vary_);

	hw.set(cc_file, file_);

}

zstr_own 
RouteSet::routeUrl(zstr_ptr name, htab_ptr params)
{
	zstr_own result;

	zobj_ptr route = getRoute(name);

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

	htab_own ret = cobj->serialize();
	ret.move_zv(return_value);
}

PHP_METHOD(Wcc_RouteSet, __unserialize)
{
	zval* data;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ARRAY(data)
	ZEND_PARSE_PARAMETERS_END();

	htab_ptr hw(Z_ARR_P(data));

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

	cobj->file_.set(name);

	return;
}

PHP_METHOD(Wcc_RouteSet, getFile)
{
	ZEND_PARSE_PARAMETERS_NONE();
	
	RouteSet* cobj = zval_toc<RouteSet>(ZEND_THIS);

	cobj->file_.return_zv(return_value);
}

PHP_METHOD(Wcc_RouteSet, getFixed)
{
	ZEND_PARSE_PARAMETERS_NONE();
	
	RouteSet* cobj = zval_toc<RouteSet>(ZEND_THIS);

	cobj->fixed_.return_zv(return_value);
}

PHP_METHOD(Wcc_RouteSet, getVary)
{
	ZEND_PARSE_PARAMETERS_NONE();
	
	RouteSet* cobj = zval_toc<RouteSet>(ZEND_THIS);

	cobj->vary_.return_zv(return_value);
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
		Z_PARAM_OBJECT_OF_CLASS(robj, route_mgr.classEntry())
	ZEND_PARSE_PARAMETERS_END();

	RouteSet* cobj = zval_toc<RouteSet>(ZEND_THIS);

	//showmem("addRoutes", ZEND_THIS);

	cobj->addRoute(zval_toc<Route>(robj));

}

PHP_METHOD(Wcc_RouteSet, getRoute)
{
	zend_string 	*name;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	RouteSet* cobj = zval_toc<RouteSet>(ZEND_THIS);

	//showmem("addRoutes", ZEND_THIS);
	zobj_ptr robj = cobj->getRoute(name);

	robj.return_zv(return_value);

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
	htab_ptr plist;
	if (params) {
		plist = params;
	}
	zstr_own robj = cobj->routeUrl(name, plist);

	robj.return_zv(return_value);

}


PHP_MINIT_FUNCTION(wcc_routeset_d)
{
	//WcR_ce = wcc_class_reg("Wc\\Route", class_Wcc_Route_methods);
	auto ce = register_class_Wcc_RouteSet();

	RouteSet::omg.classEntry(ce);

	class_init cval(ce);

	cval.add_constant("ARG_S", ARG_S);


	cval.add_constant("FUN_S", FUN_S);
	cval.add_constant("FUNX_S", FUNX_S);
	cval.add_constant("HITS_S", HITS_S);
	cval.add_constant("MOD_S", MOD_S);
	cval.add_constant("NSP_S", NSP_S);
	cval.add_constant("OBJ_S", OBJ_S);
	cval.add_constant("OBJX_S", OBJX_S);
	cval.add_constant("ROLE_S", ROLE_S);

	cval.add_constant("KEY", "@key");
	cval.add_constant("ROUTES", "routes");
	cval.add_constant("NO_PREFIX", "noprefix");
	

	return SUCCESS;

}

#endif