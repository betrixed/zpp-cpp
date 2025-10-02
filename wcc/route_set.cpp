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


extern "C" {
	#include "ext/pcre/php_pcre.h"
}


#ifndef WCC_ROUTESET_ARGINFO
#define WCC_ROUTESET_ARGINFO
extern "C" {
	#include "stub/routeset_arginfo.h"	
}
#endif



namespace wcc {
 	base_obj_mgr<RouteSet> RouteSet::omg;

class RouteSetData : public state_init 
{
public:
	str_intern module_name;
	str_intern prefix;
	str_intern method_sfx;
	str_intern fallback;
	str_intern route_set;
	
	str_intern none_tag;
	str_intern verb_tag;
	str_intern forward_slash;

	str_intern rex_url;

	str_intern cc_fixed;
	str_intern cc_vary;
	str_intern cc_file;

	RouteSetData() : state_init() {}

	virtual void init()
	{
		module_name = "module_name";
		prefix = "prefix";
		method_sfx = "method_sfx";
		fallback = "notFound";
		route_set = "route_set";
		none_tag = "<none>";
		verb_tag = "<verb>";
		forward_slash = "/";

		cc_fixed = "fixed";
		cc_vary = "vary";
		cc_file = "file";

		rex_url = 
			R"x(/\/?()x" R"x(:?[\w\d])x" R"x([_\-\.\w\d]*)x" 
			R"x(|{\w)x" R"x([_\-\.\w\d]*)x" R"x(}|\s+$)x"
			R"x()/mi)x";
	}
};

RouteSetData radata;

RouteSet::RouteSet() 
{	

}

RouteSet::~RouteSet() 
{	
}

str_ptr 
RouteSet::rex_url()
{
	return radata.rex_url;
}

void RouteSet::module(str_ptr name)
{
	if(name.size())
	{
		if (zs_cmp_ci(name,radata.none_tag) != 0)
		{
			module_name_ = name;
		}
		else {
			module_name_.init();
		}
	}
}
void RouteSet::notFound(val_ptr target)
{
	notFound_ = target;
}

void RouteSet::prefix(str_ptr prefix)
{
	if (prefix.size())
	{
		if (zs_cmp_ci(prefix, radata.none_tag) != 0) 
		{
			prefix_ = prefix;
		}
		else {
			prefix_.init();
		}
	}
}

htab_rc RouteSet::serialize()
{
	htab_rc result;

	htab_rw hw(result);

	//showarray("fixed 1", fixed_);
	hw.set(radata.cc_fixed, fixed_);
	//showarray("fixed 2", fixed_);

	hw.set(radata.cc_vary, vary_);

	hw.set(radata.cc_file, file_);

	return result;
}

void     
RouteSet::unserialize(htab_ptr hr)
{
	fixed_ = hr.get(radata.cc_fixed);
	vary_ = hr.get(radata.cc_vary);
	file_ = hr.get(radata.cc_file);
}

void  RouteSet::addRoute(obj_ptr obj)
{
	//zend_printf("set::addRoute\n");
	Route* ro = zobj_toc<Route>(obj);
	val_rc zobj(obj);
	//dump_info::msg_dump("addRoute",zobj);

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
RouteSet::indexRouteKey(obj_ptr ro)
{
	Route* route = zobj_toc<Route>(ro);
	str_ptr  key = route->id_;
	if (key.size())
	{	
		htab_rw(nameIndex_).set(key, ro);
	}	
}

void 
RouteSet::indexItem(val_ptr obj)
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

obj_rc
RouteSet::getRoute(str_ptr name)
{

	obj_rc result;
	htab_ptr idx(nameIndex_);

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


void  RouteSet::addRouteList(htab_ptr list, str_ptr prefix, str_ptr module)
{
	this->prefix(prefix);

	this->module(module);

	//showarray(" list: ", list);
	htab_walk walk;

	int loopct = 0;

	auto route = walk.value();
	

	for(walk.start(list); walk.ok(); walk.next())
	{
		//zend_printf("rlist iterate ");
		//showmem(" route: ", (zval*) route);
		loopct++;
		if (route.isObject())
		{
			//showmem("add route", route);
			obj_ptr obj = route.zobject();
			

			if (!Route::omg.myType(obj))
				continue;

			Route* cobj = zobj_toc<Route>(obj);

			str_ptr compiled = cobj->getCompiled();

			bool ok = compiled.size();

			if (!ok) {
				ok = compile(cobj);
			}
			if (ok)
			{
				this->addRoute(obj);
			}
		}

	}
	return;
}

void RouteSet::appendRoute(
	htab_rw rarr, 
	str_ptr key, 
	Route* myroute)
{
	val_rc store(myroute);

	val_ptr  zip;

	if (!rarr.try_fetch(key, zip))
	{
		// first time, store as object
		rarr.set(key, store);
		return;
	}
	
	if (zip.isObject())
	{
		// Already one stored as object, convert to list of 2 objects
		htab_rc sublist;

		htab_rw hw(sublist);

		hw.push_back(zip); 
		hw.push_back(store);

		store = sublist;

		rarr.set(key,store);

	}
	else if (zip.isArray())
	{
		// Add to list of objects, add object
		htab_rw sublist(zip);
		sublist.push_back(store);
	}

	return;
}

void RouteSet::debug_info(htab_rw hw)
{
	hw.set(radata.cc_fixed, fixed_);

	hw.set(radata.cc_vary, vary_);

	hw.set(radata.cc_file, file_);

	hw.set(radata.module_name, module_name_);

	hw.set(radata.prefix, prefix_);

	hw.set(radata.method_sfx, method_sfx_);

	hw.set(radata.fallback, notFound_);
}

void RouteSet::methodSfx(str_ptr name)
{
	method_sfx_ = name;
}

str_rc 
RouteSet::routeUrl(str_ptr name, htab_ptr params)
{
	str_rc result;

	obj_rc route = getRoute(name);

	if (!route.isNull())
	{
		result = zobj_toc<Route>(route)->routeUrl(params);
	}
	return result;
}


void
RouteSet::copy_target(Route* route)
{
	val_ptr target = route->getTarget();

	//showstr("route name", route->id_);
	
	if (target.isObject()) 
	{
		obj_rc tobj(target.zobject());
		obj_ptr init_tobj(tobj);

		if (init_tobj.instanceof( Target::omg.classEntry() )) 
		{			
			bool modifyTarget = false;

			Target* tcobj = zobj_toc<Target>(init_tobj);

			str_ptr test = tcobj->getModule();
			str_rc target_module;

			if (!test.size())
			{
				target_module = this->module_name_;
				modifyTarget = true;
			}
			else {
				target_module = test;
			}

			test =  tcobj->getFunc();
			str_rc target_method;
			if (test.size())
			{
				//? else TODO: throw exception?
				target_method = test;
			}
			str_rc suffix;
			test = this->method_sfx_;
			// Use of "<none>" to signify no suffix
			if ( test.size() 
				  && (zs_cmp_ci(test, radata.none_tag)==0)) 
			{
				suffix = str_empty();
			}
			else {
				suffix = test;
			}

			test = suffix;
			if (test.size()) 
			{
				//replace "<verb>" tag with actual verb for suffix
				if (zs_cmp_ci(test, radata.verb_tag)==0) 
				{
					suffix = Route::getVerb(route->getVerbs());
					test = suffix;
				}
				str_ptr fn_stub(target_method);
				if (test.size() && !fn_stub.ends_with(test))
				{
					str_buf fbuf(fn_stub);
					fbuf << test;
					target_method = fbuf.zstr();
					modifyTarget = true;
				}
			}

			if (modifyTarget)
			{
				obj_rc ntobj = tcobj->copy();
				Target* nt = zobj_toc<Target>(ntobj);
				nt->setModule(target_module);
				nt->setFunc(target_method);

				val_rc new_target(ntobj);

				route->setTarget(new_target);
			}

		}


	}
	//showmem("old target", target_val);
}

bool 
RouteSet::compile(Route* route)
{
	htab_rc captures;
	int   pr2;

	str_rc url(route->getPattern());
	str_ptr rex(RouteSet::rex_url());

	bool slashBegins = false;
	
	str_rc init_pattern;
	if ((url.size()==1) && (url.data()[0] == '/'))
	{
		init_pattern = radata.forward_slash;

		slashBegins = true;
		pr2 = 0;
	}
	else {
		//showstr("rex", rex);
		preg   urlseg(rex, 0, true); // flags=0, global=true
		pr2 = urlseg.matches(url);
		if (pr2) {
			captures = urlseg.captures();
		}
		
	}
	
	// try and prevent bad double-// without pattern reset
	str_ptr prefix(prefix_);

	if (prefix.size())
	{
		bool preslash = (prefix.data()[0] == '/');
		str_buf buf(init_pattern);

		if (!slashBegins) {

			if (!preslash)
			{
				buf << '/';
			}
			buf << prefix;
		}
		else {
			if (preslash) {
				//zend_printf("preslash \n");
				str_rc temp = prefix.substr(1);
				buf << temp;
			}
			else {
				buf << prefix;
			}
		}
		init_pattern = buf.zstr();
		//showstr("init_pattern", init_pattern);
	}
	//str_out sink;
	//sink << pr2 << " pattern vstr" << pattern.vstr() << '\n';

	str_buf  pattern(init_pattern);
	str_buf  compiled(init_pattern);

	//zend_printf("pattern buf %s len=%d\n", pattern.data(), pattern.size());

	str_rc name;
	str_rc blob;
	htab_rc params_tab;
	htab_rw params(params_tab);

	while (pr2 > 0) 
	{ 

		htab_ptr m2(captures);

		if (m2.size() < 2)
		{
			break;
		}
		htab_ptr segs = m2.get((int) 1);
		htab_walk wk;
		auto value = wk.value();

		int   param_ix = 1;

		for(wk.start(segs); wk.ok(); wk.next())
		{
			str_rc useg = value.zstr();
			useg.trim();
			
			if (!useg.size())
			{
				zend_throw_error(zend_ce_error,"Route has bad URL segment %s",url.data());
				return false;
				continue;
				//?? ERROR return false
			}

			useg = useg.to_lower();


			int firstchar = useg.data()[0];

			if (firstchar == ':')
			{
				name = useg.substr(1);
				str_buf bb;
				bb << '{' << name << '}'; 
				blob = bb.zstr();
			}
			else if (firstchar == '{')
			{
				name = useg.substr(1,-1);
				blob = useg;
			}
			else {
				name = useg;
				blob.init();
				pattern << '/' << useg;
				compiled << '/' << useg;
			}

			if (blob.size()) 
			{
				params.set(name, param_ix);
				param_ix++;

				pattern << '/' << blob;
				compiled << "/([^/]*)";
			}
		}
		break;
	}
	auto pcount = params.size();
	str_rc cpattern;
	str_rc rpattern = pattern.zstr();

	if (pcount > 0)
	{
		cpattern = compiled.zstr();
		compiled << "#^" << cpattern << "$#";
		cpattern = compiled.zstr();
		route->setParams(params);
	}
	else {
		cpattern = rpattern;
	}

	route->setPattern(rpattern);
	route->setCompiled(cpattern);

	copy_target(route);

	return true;
}

}; // end namespace wcc - @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

PHP_METHOD(Wcc_RouteSet, __serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RouteSet* cobj = zval_toc<RouteSet>(ZEND_THIS);

	htab_rc ret = cobj->serialize();
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
	cobj->setFile(name);
}

PHP_METHOD(Wcc_RouteSet, getFile)
{
	ZEND_PARSE_PARAMETERS_NONE();
	
	RouteSet* cobj = zval_toc<RouteSet>(ZEND_THIS);
	str_ptr file = cobj->getFile();

	file.return_zv(return_value);
}

PHP_METHOD(Wcc_RouteSet, getFixed)
{
	ZEND_PARSE_PARAMETERS_NONE();
	
	RouteSet* cobj = zval_toc<RouteSet>(ZEND_THIS);
	htab_ptr fixed =cobj->getFixed();

	fixed.return_zv(return_value);
}

PHP_METHOD(Wcc_RouteSet, getVary)
{
	ZEND_PARSE_PARAMETERS_NONE();
	
	RouteSet* cobj = zval_toc<RouteSet>(ZEND_THIS);

	htab_ptr vary = cobj->getVary();

	vary.return_zv(return_value);
}

PHP_METHOD(Wcc_RouteSet, addRoutes)
{
	zval* list;
	zend_string* prefix = nullptr;
	zend_string* module = nullptr;

	ZEND_PARSE_PARAMETERS_START(1,3)
	Z_PARAM_ARRAY(list);
	Z_PARAM_OPTIONAL
	Z_PARAM_STR_OR_NULL(prefix)
	Z_PARAM_STR_OR_NULL(module)
	ZEND_PARSE_PARAMETERS_END();

	val_ptr htab(list);

	RouteSet* cobj = zval_toc<RouteSet>(ZEND_THIS);
	cobj->addRouteList(htab.zarray(), prefix, module);	

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
	obj_rc robj = cobj->getRoute(name);

	robj.move_zv(return_value);

}

ZEND_METHOD(Wcc_RouteSet, module)
{
	zend_string* name = nullptr;
	ZEND_PARSE_PARAMETERS_START(0,1)
	Z_PARAM_OPTIONAL
	Z_PARAM_STR_OR_NULL(name);
	ZEND_PARSE_PARAMETERS_END();
	RouteSet* cobj = zval_toc<RouteSet>(ZEND_THIS);
	cobj->module(name);	
}

ZEND_METHOD(Wcc_RouteSet, methodSfx)
{
	zend_string* sfx = nullptr;
	ZEND_PARSE_PARAMETERS_START(0,1)
	Z_PARAM_OPTIONAL
	Z_PARAM_STR_OR_NULL(sfx);
	ZEND_PARSE_PARAMETERS_END();
	RouteSet* cobj = zval_toc<RouteSet>(ZEND_THIS);
	cobj->methodSfx(sfx);
}

ZEND_METHOD(Wcc_RouteSet, notFound)
{
	zval* target;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ZVAL(target);
	ZEND_PARSE_PARAMETERS_END();

	RouteSet* cobj = zval_toc<RouteSet>(ZEND_THIS);
	cobj->notFound(target);
}

ZEND_METHOD(Wcc_RouteSet, prefix)
{
	zend_string* name = nullptr;
	ZEND_PARSE_PARAMETERS_START(0,1)
	Z_PARAM_OPTIONAL
	Z_PARAM_STR_OR_NULL(name);
	ZEND_PARSE_PARAMETERS_END();
	RouteSet* cobj = zval_toc<RouteSet>(ZEND_THIS);
	cobj->prefix(name);	
	
}

ZEND_METHOD(Wcc_RouteSet, rex_url)
{
	ZEND_PARSE_PARAMETERS_NONE();

	str_ptr rex = RouteSet::rex_url();

	rex.return_zv(return_value);
}

PHP_METHOD(Wcc_RouteSet, routeUrl)
{
	zend_string*	name;
	zval*           params = nullptr;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(name)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_OR_NULL(params)
	ZEND_PARSE_PARAMETERS_END();

	RouteSet* cobj = zval_toc<RouteSet>(ZEND_THIS);

	//showmem("addRoutes", ZEND_THIS);
	htab_ptr plist;
	if (params) {
		plist = params;
	}
	str_rc robj = cobj->routeUrl(name, plist);

	robj.move_zv(return_value);

}

ZEND_METHOD(Wcc_RouteSet, compile)
{
	zval* robj;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT_OF_CLASS(robj, Route::omg.classEntry())
	ZEND_PARSE_PARAMETERS_END();
	RouteSet* cobj = zval_toc<RouteSet>(ZEND_THIS);
	cobj->compile(zval_toc<Route>(robj));
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