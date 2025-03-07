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
	zstr_intern module_name;
	zstr_intern prefix;
	zstr_intern method_sfx;
	zstr_intern fallback;
	zstr_intern route_set;
	zstr_intern rex_url;

	RouteAddData() : state_init() {}

	virtual void init()
	{
		module_name = zstr_intern("module_name");
		prefix = zstr_intern("prefix");
		method_sfx = zstr_intern("method_sfx");
		fallback = zstr_intern("fallback");
		route_set = zstr_intern("route_set");

		rex_url = zstr_intern(
			R"x(/\/?()x"
			R"x(:?[\w\d])x" R"x([_\-\.\w\d]*)x" R"x(|{\w)x" R"x([_\-\.\w\d]*)x" R"x(}|\s+$)x"
			R"x()/mi)x"
		);
	}
};

RouteAddData radata;

void RouteAdd::debug_info(htab_write di)
{
	di.set(radata.route_set, route_set_);
	di.set(radata.module_name, module_name_);
	di.set(radata.prefix, url_prefix_);
	di.set(radata.method_sfx, method_sfx_);
	di.set(radata.fallback, fallback_);
}

void RouteAdd::construct(zobj_user rset)
{
	if (rset.isNull())
	{
		route_set_ =  RouteSet::omg.new_zobj();
	}
	else {
		route_set_ = rset;
	}
}

zstr_user 
RouteAdd::rex_url()
{
	return radata.rex_url;
}

void 
RouteAdd::addRoutes(htab_read list, zstr_user prefix, zstr_user module)
{
	if (prefix.size())
	{
		if (prefix.vstr() != "<none>")
		{
			url_prefix_ = prefix;
		}
		else {
			url_prefix_.init();
		}
	}

	if(module.size())
	{
		if (module.vstr() != "<none>")
		{
			module_name_ = module;
		}
		else {
			module_name_.init();
		}
	}

	htab_walk wk;
	auto value = wk.value();
	RouteSet* rs = zobj_toc<RouteSet>(route_set_);

	for(wk.start(list); wk.ok(); wk.next())
	{
		zobj_user route(value.zobject());
		Route* r = zobj_toc<Route>(route);

		zstr_user compiled = r->getCompiled();

		if (!compiled.size()) {
			ready(r);
		}
		rs->addRoute(value);
	}
}


void RouteAdd::fallback(zval_user backup)
{
	fallback_ = backup;
}

zobj_user 
RouteAdd::getRouteSet() const
{
	return route_set_;
}

void RouteAdd::methodSfx(zstr_user sfx)
{
	method_sfx_ = sfx;	
}


void RouteAdd::module(zstr_user name)
{
	module_name_ = name;
}


void RouteAdd::prefix(zstr_user start)
{
	url_prefix_ = start;
}


void RouteAdd::ready(Route* route)
{
	htab_mgr captures;

	size_t   pr2;

	zstr_buffer  pattern;
	zstr_buffer  compiled;

	zstr_mgr url(route->getPattern());

	zstr_user rex(RouteAdd::rex_url());

	bool slashBegins = false;

	if ((url.size()==1) && (url.data()[0] == '/'))
	{
		pattern << '/';
		slashBegins = true;
		pr2 = 0;
	}
	else {
		preg   urlseg(rex,0,true); // flags=0, global=true
		pr2 = urlseg.matches(url);
		if (pr2) {
			captures = urlseg.captures();
		}
	}
	
	htab_mgr segs;


	// try and prevent bad double-// without pattern reset
	zstr_user prefix(url_prefix_);
	if (prefix.size())
	{
		bool preslash = (prefix.data()[0] == '/');

		if (!slashBegins) {
			if (!preslash)
			{
				pattern << '/';
			}
			pattern << prefix;
		}
		else {
			if (preslash) {
				zend_printf("preslash \n");
				pattern << prefix.substr(1);
			}
			else {
				pattern << prefix;
			}
		}
	}
	//showstr("pattern ", pattern);
	zstr_mgr temp;

	temp = pattern.zstr();
	compiled << temp; 
	pattern << temp; // reset zstr_buffer as fresh content

	
	zstr_mgr name;
	zstr_mgr blob;
	htab_mgr params_tab;
	htab_write params(params_tab);


	// while allows for a break
	while (pr2 > 0) 
	{ 
		htab_read m2(captures);

		if (m2.size() < 2)
		{
			break;
		}
		htab_read segs = m2.get((int) 1);
		htab_walk wk;
		auto value = wk.value();

		int   param_ix = 1;

		for(wk.start(segs); wk.ok(); wk.next())
		{
			zstr_user useg = value.zstr();

			zstr_mgr seg = useg.trim();
			useg = seg;

			seg = useg.to_lower();
			useg = seg;

			int firstchar = useg.data()[0];

			if (firstchar == ':')
			{
				name = useg.substr(1);
				zstr_buffer bb;
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


	}
	auto pcount = params.size();
	zstr_mgr cpattern;
	zstr_mgr rpattern(std::move(pattern));

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

	zval_user target = route->getTarget();
	if (target.isObject()) {
		zobj_user tobj = target.zobject();

		if (tobj.instanceof(Target::omg.classEntry())) 
		{
			Target* t = zobj_toc<Target>(tobj);
			zstr_user sfx(method_sfx_);

			if (sfx.size() && (sfx.vstr() != "<none>")) {
				zstr_buffer fbuf(t->getFunc());

				if (sfx.vstr() == "<verb>") 
				{
					fbuf << Route::getVerb(route->getVerbs());
				}
				else {
					fbuf << sfx;
				}
				zstr_mgr fname = fbuf.zstr();
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

	zobj_user param(rset_obj);

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

	zval_user htab(list);

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
	zobj_user rset = cobj->getRouteSet();

	rset.return_zv(return_value);
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