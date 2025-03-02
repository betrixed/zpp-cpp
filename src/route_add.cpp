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

void RouteAdd::debug_info(HashTable* ht)
{
	htab_ptr di(ht);
	di.set(radata.route_set, route_set_);
	di.set(radata.module_name, module_name_);
	di.set(radata.prefix, url_prefix_);
	di.set(radata.method_sfx, method_sfx_);
	di.set(radata.fallback, fallback_);
}

void RouteAdd::construct(zobj_ptr rset)
{
	if (rset.isNull())
	{
		route_set_ = zobj_own(RouteSet::omg.make_new());
	}
	else {
		route_set_ = rset;
	}
}

zstr_ptr 
RouteAdd::rex_url()
{
	return radata.rex_url;
}

void RouteAdd::addRoutes(htab_ptr list, zstr_ptr prefix, zstr_ptr module)
{
	if (prefix.size())
	{
		if (prefix.vstr() != "<none>")
		{
			url_prefix_ = prefix;
		}
		else {
			url_prefix_.lose();
		}
	}

	if(module.size())
	{
		if (module.vstr() != "<none>")
		{
			module_name_ = module;
		}
		else {
			module_name_.lose();
		}
	}

	htab_walk wk;
	auto& value = wk.value();
	RouteSet* rs = zobj_toc<RouteSet>(route_set_);

	for(wk.start(list); wk.ok(); wk.next())
	{
		zobj_ptr route(value.zobject());
		Route* r = zobj_toc<Route>(route);

		zstr_ptr compiled = r->getCompiled();

		if (!compiled.size()) {
			ready(r);
		}
		rs->addRoute(r);
	}
}


void RouteAdd::fallback(zval_ptr backup)
{
	fallback_ = backup;
}


const zobj_own& 
RouteAdd::getRouteSet() const
{
	return route_set_;
}


void RouteAdd::methodSfx(zstr_ptr sfx)
{
	method_sfx_ = sfx;	
}


void RouteAdd::module(zstr_ptr name)
{
	module_name_ = name;
}


void RouteAdd::prefix(zstr_ptr start)
{
	url_prefix_ = start;
}


void RouteAdd::ready(Route* route)
{
	htab_own m2((HashTable*)nullptr);
	size_t   pr2;

	zstr_buffer  pattern;
	zstr_buffer  compiled;

	zstr_own url(route->getPattern());

	zstr_ptr rex(RouteAdd::rex_url());

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
			m2 = urlseg.captures();
		}
	}
	
	htab_ptr segs;

	// try and prevent bad double-// without pattern reset
	if (url_prefix_.size())
	{
		bool preslash = (url_prefix_.data()[0] == '/');

		if (!slashBegins) {
			if (!preslash)
			{
				pattern << '/';
			}
			pattern << url_prefix_;
		}
		else {
			if (preslash) {
				zend_printf("preslash \n");
				pattern << url_prefix_.substr(1);
			}
			else {
				pattern << url_prefix_;
			}
		}
	}
	//showstr("pattern ", pattern);

	if ((pr2 > 0)&&(m2.size() > 1)) {
		segs = m2.get((int) 1);
	}

	zstr_own temp;

	temp = pattern.zstr();
	compiled << temp; 
	pattern << temp; // reset zstr_buffer as fresh content

	htab_walk wk;
	auto& value = wk.value();
	zstr_own name;
	zstr_own blob;
	htab_own params;

	if (segs.size()) 
	{

		int   param_ix = 1;

		for(wk.start(segs); wk.ok(); wk.next())
		{
			zstr_ptr iseg = value.zstr();

			zstr_own seg = iseg.trim();

			seg = seg.to_lower();
			int firstchar = seg.data()[0];

			if (firstchar == ':')
			{
				name = seg.substr(1);
				zstr_buffer bb;
				bb << '{' << name << '}'; 
				blob = bb.zstr();
			}
			else if (firstchar == '{')
			{
				name = seg.substr(1,-1);
				blob = seg;
			}
			else {
				name = seg;
				blob.lose();
				pattern << '/' << seg;
				compiled << '/' << seg;
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
	zstr_own cpattern;
	zstr_own rpattern(std::move(pattern));

	if (pcount > 0)
	{
		cpattern = compiled.zstr();
		compiled << "#^" << cpattern << "$#";
		cpattern = compiled.zstr();
		zval_own zparams(params);
		route->setParams(zparams);
	}
	else {
		cpattern = rpattern;
	}
	//showstr("rpattern", rpattern);
	//showstr("cpattern", cpattern);
	route->setPattern(rpattern);
	route->setCompiled(cpattern);

	zval_own target = route->getTarget();
	if (target.isObject()) {
		zobj_own tobj = target.zobject();
		if (tobj.instanceof(Target::omg.classEntry())) 
		{
			Target* t = zobj_toc<Target>(tobj);
			if (method_sfx_.size() && (method_sfx_.vstr() != "<none>")) {
				zstr_buffer fbuf(t->getFunc());

				if (method_sfx_.vstr() == "<verb>") 
				{
					fbuf << Route::getVerb(route->getVerbs());
				}
				else {
					fbuf << method_sfx_;
				}
				zstr_own fname = fbuf.zstr();
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

	zobj_ptr param(rset_obj);

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

	zval_ptr htab(list);

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
	const zobj_own& rset = cobj->getRouteSet();

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
	Z_PARAM_OBJECT_OF_CLASS(robj, route_mgr.classEntry())
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