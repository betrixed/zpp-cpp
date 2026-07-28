#ifndef WCC_ROUTE_ADD_CPP
#define WCC_ROUTE_ADD_CPP

#ifndef WCC_ROUTE_ADD_H
#include "route_add.h"
#endif

namespace wcc {

base_obj_mgr<RouteAdd> RouteAdd::omg;

class RouteAddData : public state_init 
{
public:
	str_intern module_name;
	str_intern prefix;
	str_intern method_sfx;
	str_intern fallback;
	str_intern route_set;
	
	str_intern none_tag;
	str_intern verb_tag;

	str_intern rex_url;

	void init() override
	{
		module_name = "module_name";
		prefix = "prefix";
		method_sfx = "method_sfx";
		fallback = "fallback";
		route_set = "route_set";
		none_tag = "<none>";
		verb_tag = "<verb>";

		rex_url = 
			R"x(/\/?()x"
			R"x(:?[\w\d])x" R"x([_\-\.\w\d]*)x" R"x(|{\w)x" R"x([_\-\.\w\d]*)x" R"x(}|\s+$)x"
			R"x()/mi)x";
	}
};

RouteAddData radata;

void RouteAdd::debug_info(htab_rw di)
{
	di.set(radata.route_set, route_set_);
	di.set(radata.module_name, module_name_);
	di.set(radata.prefix, url_prefix_);
	di.set(radata.method_sfx, method_sfx_);
	di.set(radata.fallback, fallback_);
}

void RouteAdd::construct(obj_ptr rset)
{
	if (rset.isNull())
	{
		route_set_ =  RouteSet::omg.new_zobj();
	}
	else {
		route_set_ = rset;
	}
}

str_ptr 
RouteAdd::rex_url()
{
	return radata.rex_url;
}

void 
RouteAdd::addRoutes(htab_ptr list, str_ptr prefix, str_ptr module)
{
	if (prefix.size())
	{
		if (zs_cmp_ci(prefix, radata.none_tag) != 0) 
		{
			url_prefix_ = prefix;
		}
		else {
			url_prefix_.init();
		}
	}

	if(module.size())
	{
		if (zs_cmp_ci(module,radata.none_tag) != 0)
		{
			module_name_ = module;
		}
		else {
			module_name_.init();
		}
	}
	//showstr("module_name_", module_name_);
	htab_walk wk;
	auto value = wk.value();
	//showobj("RouteSet", route_set_);

	RouteSet* rs = zobj_toc<RouteSet>(route_set_);

	//showdata("list", list);
	for(wk.start(list); wk.ok(); wk.next())
	{
		obj_ptr route(value.zobject());

		//showobj("route", route);

		Route* r = zobj_toc<Route>(route);

		str_ptr compiled = r->getCompiled();
		//showstr("compiled", compiled);

		if (!compiled.size()) {
			ready(r);
		}
		rs->addRoute(value);
	}
	//zend_printf("Routes added\n" );
}


void RouteAdd::fallback(val_ptr backup)
{
	fallback_ = backup;
}

obj_ptr 
RouteAdd::getRouteSet() const
{
	return route_set_;
}

void RouteAdd::methodSfx(str_ptr sfx)
{
	method_sfx_ = sfx;	
}


void RouteAdd::module(str_ptr name)
{
	module_name_ = name;
}


void RouteAdd::prefix(str_ptr start)
{
	url_prefix_ = start;
}


void RouteAdd::ready(Route* route)
{
	htab_rc captures;

	size_t   pr2;

	str_buf  pattern;
	str_buf  compiled;

	str_rc url(route->getPattern());
	showstr("url", url);

	str_ptr rex(RouteAdd::rex_url());

	bool slashBegins = false;

	if ((url.size()==1) && (url.data()[0] == '/'))
	{
		pattern << '/';
		slashBegins = true;
		pr2 = 0;
	}
	else {
		zend_printf("Try matches ");
		showstr("rex", rex);
		preg   urlseg(rex, 0, true); // flags=0, global=true
		pr2 = urlseg.matches(url);
		if (pr2) {
			captures = urlseg.captures();
			//showdata("captures", captures);
		}
		else {
			zend_printf("No matches\n");
		}
	}

	// try and prevent bad double-// without pattern reset
	str_ptr prefix(url_prefix_);

	if (prefix.size())
	{
		bool preslash = (prefix.data()[0] == '/');
		zend_printf("preslash %d slashBegins %d\n", preslash, slashBegins);

		if (!slashBegins) {
			if (!preslash)
			{
				pattern << '/';
			}
			pattern << prefix;
		}
		else {
			if (preslash) {
				//zend_printf("preslash \n");
				pattern << prefix.substr(1);
			}
			else {
				pattern << prefix;
			}
		}
	}
	//str_out sink;
	//sink << pr2 << " pattern vstr" << pattern.vstr() << '\n';

	str_rc temp;

	temp = pattern.zstr();
	//showstr("pattern", temp);

	compiled << temp; 
	pattern << temp; // reset str_buf as fresh content

	
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
		//showdata("segs", segs);

		for(wk.start(segs); wk.ok(); wk.next())
		{
			str_ptr seg = value.zstr();

			str_rc useg = seg.trim();	
			useg.lowercase();


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
	//showstr("rpattern", rpattern);
	//showstr("cpattern", cpattern);
	route->setPattern(rpattern);
	route->setCompiled(cpattern);

	val_ptr target = route->getTarget();
	if (target.isObject()) {
		obj_ptr tobj = target.zobject();

		if (tobj.instanceof(Target::omg.classEntry())) 
		{
			Target* t = zobj_toc<Target>(tobj);
			str_ptr sfx(method_sfx_);

			if (sfx.size() && (zs_cmp_ci(sfx, radata.none_tag)!=0)) {
				str_buf fbuf(t->getFunc());

				if (zs_cmp_ci(sfx, radata.verb_tag) == 0) 
				{
					fbuf << Route::getVerb(route->getVerbs());
				}
				else {
					fbuf << sfx;
				}
				str_rc fname = fbuf.zstr();
				showstr("fname", fname);
				t->setFunc(fname);
			}
			t->module(module_name_);
		}
	}
}

}; // namespace wcc
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ZEND_METHOD(Wcc_RouteAdd, __construct)
{
	zval* rset_obj  = nullptr;

	ZEND_PARSE_PARAMETERS_START(0,1)
	Z_PARAM_OPTIONAL
	Z_PARAM_OBJECT_OF_CLASS_OR_NULL(rset_obj, RouteSet::omg.classEntry())
	ZEND_PARSE_PARAMETERS_END();

	RouteAdd* cobj = zval_toc<RouteAdd>(ZEND_THIS);

	obj_ptr param(rset_obj);

	cobj->construct(param);	
}

ZEND_METHOD(Wcc_RouteAdd, addRoutes)
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

	RouteAdd* cobj = zval_toc<RouteAdd>(ZEND_THIS);
	cobj->addRoutes(htab.zarray(), prefix, module);	
}

ZEND_METHOD(Wcc_RouteAdd, fallback)
{
	zval* target;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ZVAL(target);
	ZEND_PARSE_PARAMETERS_END();

	RouteAdd* cobj = zval_toc<RouteAdd>(ZEND_THIS);
	cobj->fallback(target);
}

ZEND_METHOD(Wcc_RouteAdd, getRouteSet)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RouteAdd* cobj = zval_toc<RouteAdd>(ZEND_THIS);
	obj_ptr rset = cobj->getRouteSet();

	rset.copy_zv(return_value);
}



ZEND_METHOD(Wcc_RouteAdd, methodSfx)
{
	zend_string* sfx = nullptr;
	ZEND_PARSE_PARAMETERS_START(0,1)
	Z_PARAM_OPTIONAL
	Z_PARAM_STR_OR_NULL(sfx);
	ZEND_PARSE_PARAMETERS_END();
	RouteAdd* cobj = zval_toc<RouteAdd>(ZEND_THIS);
	cobj->methodSfx(sfx);
}

ZEND_METHOD(Wcc_RouteAdd, module)
{
	zend_string* name = nullptr;
	ZEND_PARSE_PARAMETERS_START(0,1)
	Z_PARAM_OPTIONAL
	Z_PARAM_STR_OR_NULL(name);
	ZEND_PARSE_PARAMETERS_END();
	RouteAdd* cobj = zval_toc<RouteAdd>(ZEND_THIS);
	cobj->module(name);	
}

ZEND_METHOD(Wcc_RouteAdd, prefix)
{
	zend_string* name = nullptr;
	ZEND_PARSE_PARAMETERS_START(0,1)
	Z_PARAM_OPTIONAL
	Z_PARAM_STR_OR_NULL(name);
	ZEND_PARSE_PARAMETERS_END();
	RouteAdd* cobj = zval_toc<RouteAdd>(ZEND_THIS);
	cobj->prefix(name);	
	
}

ZEND_METHOD(Wcc_RouteAdd, ready)
{
	zval* robj;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT_OF_CLASS(robj, Route::omg.classEntry())
	ZEND_PARSE_PARAMETERS_END();
	RouteAdd* cobj = zval_toc<RouteAdd>(ZEND_THIS);
	cobj->ready(zval_toc<Route>(robj));
}

PHP_MINIT_FUNCTION(wcc_route_add)
{
	zend_class_entry *ce = register_class_Wcc_RouteAdd();
	RouteAdd::omg.classEntry(ce);
	
	return SUCCESS;

}

//route_add.cpp
#endif