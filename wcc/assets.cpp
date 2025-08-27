#ifndef WCC_ASSETS_CPP
#define WCC_ASSETS_CPP

#ifndef WCC_ASSETS_H
#include "assets.h"
#endif

#ifndef WCC_CONFIG_H
#include "config.h"
#endif

#ifndef SEARCH_LIST_H
#include "search_list.h"
#endif

#ifndef WCC_SERVICES_H
#include "services.h"
#endif

namespace wcc {

using namespace zpp;

base_obj_mgr<Assets> Assets::omg;

class ASinit : public state_init {
public:
	ASinit() : state_init() {}

	str_intern assets_cfg;
	str_intern assets_str;
	str_intern cache_all;
	str_intern css_str;
	str_intern file_cache;
	str_intern link_str;
	str_intern read_cache;
	str_intern requires_str;
	str_intern run_str;

	str_intern script_end;
	str_intern script_tag;
	str_intern src_paths;
	str_intern style_end;
	str_intern style_tag;
	str_intern web_dir;
	str_intern js_inline;
	

	void init() override 
	{
		assets_cfg = "assets_cfg";
		assets_str = "assets";
		cache_all = "cache_all";
		css_str = "css";
		file_cache = "file_cache";
		link_str = "link";
		read_cache = "readcache";
		requires_str = "requires";
		run_str = "run";
		script_tag = "<script>";
		src_paths = "src_paths";
		script_end = "</script>";
		style_end = "</style>";
		style_tag = "<style>";
		web_dir = "web_dir";
		js_inline = "js-inline";
	}
};

ASinit  ASI;


str_rc 
Assets::findSourceFile(str_ptr path)
{
	SearchList* sp = zobj_toc<SearchList>(src_paths_);
	str_rc test = sp->findLeaf(path);
	return test;
}

htab_rc  
Assets::getWebList(str_ptr selector,
	 val_ptr names, bool list)
{
	htab_rc result;
	htab_rc order;
	if (names.isNull() || names.empty())
	{
		order = order_;
	}
	else if (names.isString())
	{
		htab_rw hw(order);
		hw.push_back(names.zstr());
	}
	else {
		order = names.zarray();
	}

	for_key_value w1;
	htab_rw hw(result);

	for(w1.start(order); w1.ok(); w1.next())
	{
		str_ptr name = w1.value();
		htab_rc asset = assets_.property(name);
		if (asset.size())
		{
			htab_rc items = asset.get(selector);
			if (items.size())
			{
				if (list)
				{
					htab_walk w2;
					auto wpath = w2.value();
					for(w2.start(items); w2.ok(); w2.next())
					{
						hw.push_back(wpath);
					}
				}
				else {
					hw.push_back(items);
				}
			}
		}
	}
	return result;
}

void Assets::jsInline()
{
	htab_rc ipaths = getWebList(ASI.js_inline);
	for_key_value w1;
	for(w1.start(ipaths); w1.ok(); w1.next())
	{
		str_rc path = w1.value();
		path = verify_path(path);
		str_rc script = file_get_contents(path);
		str_buf buf;
		buf << ASI.script_tag << script << ASI.script_end;
		script = buf.zstr();
		addBlob(script);
	}
}

Assets::Assets() : ServiceAccess(), render_lock_(false)
{
}

void 
Assets::construct()
{
	val_rc nullval; // null value

	ServiceAccess::construct(nullval);

	src_paths_ = SearchList::omg.new_zobj();
	
	SearchList* slist = zobj_toc<SearchList>(src_paths_);
	slist->construct(nullval);

	obj_rc run = this->service(ASI.run_str);

	web_ = run.property(ASI.web_dir);

	str_rc assets_file = run.property(ASI.assets_cfg);

	if (assets_file.size())
	{
		this->loadAssetFile(assets_file);
	}
	else {
		assets_ = Config::omg.new_zobj();
	}
}

str_rc 
Assets::cssHeader()
{
	render_lock_ = true;
	str_buf buf;
	htab_rc paths = getWebList(ASI.css_str);
	
	if (paths.size())
	{
		for_key_value kv1;

		for(kv1.start(paths); kv1.ok(); kv1.next())
		{
			str_rc wpath = kv1.value();
			wpath = verify_path(wpath);
			buf << link_css(wpath);
		}
	}
	return buf.zstr();


}

str_rc //static 
Assets::link_css(str_ptr webpath)
{
	str_buf buf;

	buf << R"(<link rel="stylesheet" type="text/css" href=")"
	    << webpath << R"(">)" << '\n';
	return buf.zstr();
}

bool 
Assets::markAdd(str_ptr item)
{
	if (!mark_.has_key(item))
	{
		Config* cfg = zobj_toc<Config>(assets_);

		val_rc data;

		data = cfg->getOrNot(item, data);

		if (data.isNull())
		{
			zend_throw_error(zend_ce_error, "Asset Key '%s' not found.", item.data());
			return false;
		}
		htab_ptr alist = data.zarray();
		if (alist.size())
		{
			val_ptr req_names = alist.get(ASI.requires_str);
			if (req_names.ok())
			{
				this->add(req_names);
			}
			htab_rw wo(order_);
			wo.push_back(item);

			htab_rw wm(mark_);
			wm.setbool(item,true);
		}
	}
	return true;
}

void 
Assets::add(val_ptr nlist)
{
	if (render_lock_) {
		zend_throw_error(zend_ce_error, "Assets locked during render");
		return;
	}
	if (nlist.isArray())
	{
		for_key_value loop;
		for(loop.start(nlist.zarray()); loop.ok(); loop.next())
		{
			str_rc name = loop.value();
			if (!this->markAdd(name))
				break;
		}
	}
	else if (nlist.isString())
	{
		this->markAdd(nlist);
	}
}

void 
Assets::addAssets(htab_ptr data)
{
	htab_walk wk;
	auto key = wk.key();
	auto value = wk.value();

	for(wk.start(data); wk.ok(); wk.next())
	{
		assets_.property(key, value);
	}
}

void 
Assets::addBlob(str_rc blob, bool header)
{
	if (header)
	{
		htab_rw hw(headBlob_);
		hw.push_back(blob);
	}
	else {
		htab_rw hw(bodyBlob_);
		hw.push_back(blob);
	}
}

htab_ptr 
Assets::addSourcePath(str_ptr path)
{
	SearchList* paths = zobj_toc<SearchList>(src_paths_);
	paths->addPath(path);
	return paths->getPaths();
}

void 
Assets::addStyle(str_ptr style)
{
	str_ptr estr = str_ptr::empty_str();

	str_rc sct = str_replace(ASI.style_tag, estr, style);
	sct = str_replace(ASI.style_end, estr, sct);
	htab_rw hw(inline_styles_);

	hw.push_back(sct);
}

str_rc 
Assets::footer()
{
	render_lock_ = true;
	this->jsInline();
	str_buf buf;

	str_rc temp = jsPut();
	buf << temp;

	temp = implode_blob(bodyBlob_);
	buf << temp;

	return buf.zstr();
}

str_rc implode_blob(htab_ptr blobs)
{
	str_rc result;

	if (blobs.size())
	{
		result = implode(str_ptr::empty_str(), blobs);
	}
	return result;
}


obj_ptr 
Assets::getSearchList()
{
	return src_paths_;
}


bool 
Assets::has(str_ptr key)
{
	return assets_.has_property(key);
}

str_rc 
Assets::header()
{
	render_lock_ = true;
	return implode_blob(headBlob_);
}


str_rc 
Assets::inline_css(str_ptr name)
{
	val_rc temp(name);
	htab_rc paths = getWebList(ASI.css_str, temp);
	str_buf buf;

	obj_rc run = Services::service(ASI.run_str);
	Replace pathnames(run, PRI.prop_expr);

	if (paths.size())
	{
		for_key_value w1;

		for(w1.start(paths); w1.ok(); w1.next())
		{
			str_rc css_path = w1.value();
			if (css_path.size())
			{
				css_path = pathnames.eval(css_path);
				css_path = findSourceFile(css_path);

				str_rc data = file_get_contents(css_path);
				buf << endl << ASI.style_tag << endl;
				buf << data << endl << ASI.style_end << endl;
			}
			else {
				zend_throw_error(zend_ce_error,"Empty css path for %s", name.data());
				break;
			}
		}
	}
	return buf.zstr();
}

str_rc 
Assets::link()
{
	render_lock_ = true;

	htab_rc list = getWebList(ASI.link_str,val_ptr(), false);

	str_buf buf;

	for_key_value kv1;

	for(kv1.start(list); kv1.ok(); kv1.next())
	{
		htab_ptr ltab = kv1.value();
		if (ltab.size())
		{
			for_key_value kv2;
			buf << "<link";

			for(kv2.start(ltab); kv2.ok(); kv2.next())
			{
				str_ptr attr = kv2.key();
				str_ptr val = kv2.value();
				buf << ' ' << attr << '=';
				buf.quote_name(val);
			}
			buf << '>' << endl;
		}
	} 
	return buf.zstr();
}

htab_rc 
Assets::filterPaths(htab_ptr paths)
{
	obj_rc run = Services::service(ASI.run_str);
	htab_rc result;

	if (paths.size())
	{
		htab_rw hw(result);
		Replace pathnames(run, PRI.prop_expr);
		for_key_value kv1;
		for(kv1.start(paths); kv1.ok(); kv1.next())
		{
			str_rc sp = kv1.value();
			sp = pathnames.eval(sp);
			hw.push_back(sp);
		}
	}
	return result;
}
void 
Assets::loadAssetFile(str_ptr file)
{
	if (! file_exists(file))
	{
		zend_throw_error(zend_ce_error,"Asset file '%s' not found", file.data());
		return;
	}

	obj_rc cache_all = Services::service(ASI.cache_all);

	val_rc filename(file);
	val_rc cachename(ASI.file_cache);
	htab_rc data = cache_all.call(ASI.read_cache, filename, cachename);
	htab_rc paths;

	val_ptr paths_v = data.get(ASI.src_paths);
	if (paths_v.isString())
	{
		htab_rw hw(paths);
		hw.push_back(paths_v.zstr());
	}
	else {
		paths = paths_v.zarray();
	}
	if (paths.size())
	{
		paths = filterPaths(paths);
		SearchList* slist = zobj_toc<SearchList>(src_paths_);
		slist->addPaths(paths);
	}

	val_ptr temp = data.get(ASI.assets_str);
	if (temp.isArray())
	{
		assets_ = Config::omg.new_zobj();
		Config* cfg = zobj_toc<Config>(assets_);
		cfg->construct(temp);
	}
	else if (temp.isObject())
	{
		assets_ = temp.zobject();
	}
}

void 
Assets::reset()
{
	order_.init();
	mark_.init();
}

str_rc 
Assets::styleHeader()
{
	str_buf buf;

	if (inline_styles_.size())
	{
		buf << ASI.style_tag << endl;
		for_key_value kv1;
		for(kv1.start(inline_styles_); kv1.ok(); kv1.next())
		{
			str_ptr s = kv1.value();
			buf << s;
		}
		buf << ASI.style_end << endl;
	}
	return buf.zstr();
}


}; //end namespace wcc

using namespace wcc;
using namespace zpp;

ZEND_METHOD(Wcc_Assets, __construct)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Assets* cobj = zval_toc<Assets>(ZEND_THIS);

	cobj->construct();
}

ZEND_METHOD(Wcc_Assets, add)
{
	zarg_rd args(execute_data);

	val_ptr list(args.need(1));

	if (!(list.isString() || list.isArray()))
	{
		args.error() << "; Expect String or Array";
	}
	if (!args.throw_errors())
	{
		Assets* cobj = zval_toc<Assets>(ZEND_THIS);
		cobj->add(list);
	}
}

ZEND_METHOD(Wcc_Assets, addAssets)
{
	zarg_rd args(execute_data);
	htab_ptr data;
	args.zarray(data, args.need(1));

	if (!args.throw_errors())
	{
		Assets* cobj = zval_toc<Assets>(ZEND_THIS);
		cobj->addAssets(data);
	}
}

ZEND_METHOD(Wcc_Assets, addBlob)
{
	zarg_rd args(execute_data);
	str_ptr blob;
	bool    headblob = false;
	args.zstring(blob, args.need(1));
	args.zbool(headblob, args.option(2));
	if (!args.throw_errors())
	{
		Assets* cobj = zval_toc<Assets>(ZEND_THIS);
		cobj->addBlob(blob, headblob);
	}
}

ZEND_METHOD(Wcc_Assets, addSourcePath)
{
	zarg_rd args(execute_data);
	str_ptr path;
	args.zstring(path, args.need(1));
	if (!args.throw_errors())
	{
		Assets* cobj = zval_toc<Assets>(ZEND_THIS);
		cobj->addSourcePath(path);
	}
}

ZEND_METHOD(Wcc_Assets, addStyle)
{
	zarg_rd args(execute_data);
	str_ptr style;
	args.zstring(style, args.need(1));
	if (!args.throw_errors())
	{
		Assets* cobj = zval_toc<Assets>(ZEND_THIS);
		cobj->addStyle(style);
	}
}

ZEND_METHOD(Wcc_Assets, footer)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Assets* cobj = zval_toc<Assets>(ZEND_THIS);

	str_rc text = cobj->footer();
	text.move_zv(return_value);
}

ZEND_METHOD(Wcc_Assets, getSearchList)
{
	ZEND_PARSE_PARAMETERS_NONE();
	Assets* cobj = zval_toc<Assets>(ZEND_THIS);
	obj_rc  obj = cobj->getSearchList();
	obj.move_zv(return_value);
}

/*
getWebList(
 		string $selector, 
 		null|string|array $names = null,
 		bool $list = true) : array {} */

ZEND_METHOD(Wcc_Assets, getWebList)
{
	zarg_rd args(execute_data);

	str_rc  typekey;
	val_ptr names;
	bool    aslist = true;
	htab_rc result;

	args.zstring(typekey, args.need(1));

	val_ptr names = args.option(2);
	if (names.ok() && (names.isString() || names.isArray())) {
		args.zbool(aslist, args.option(3));
	}
	if (!args.throw_errors())
	{
		Assets* cobj = zval_toc<Assets>(ZEND_THIS);
		result = cobj->getWebList(typekey, names, aslist);
		result.move_zv(return_value);
	}	
}

ZEND_METHOD(Wcc_Assets, has)
{
	zarg_rd args(execute_data);
	str_rc  namekey;

	args.zstring(namekey, args.need(1));
	if (!args.throw_errors())
	{
		Assets* cobj = zval_toc<Assets>(ZEND_THIS);
		bool result = cobj->has(namekey);
		RETURN_BOOL(result);
	}
}

ZEND_METHOD(Wcc_Assets, header)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Assets* cobj = zval_toc<Assets>(ZEND_THIS);

	str_rc text = cobj->header();

	text.move_zv(return_value);
}

ZEND_METHOD(Wcc_Assets, inline_css)
{
	zarg_rd args(execute_data);

	str_ptr name;

	args.zstring(name, args.need(1));
	if (!args.throw_errors())
	{
		Assets* cobj = zval_toc<Assets>(ZEND_THIS);
		str_rc text = cobj->inline_css(name);
		text.move_zv(return_value);
	}
}

ZEND_METHOD(Wcc_Assets, link)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Assets* cobj = zval_toc<Assets>(ZEND_THIS);
	str_rc text = cobj->link();
	text.move_zv(return_value);

}
ZEND_METHOD(Wcc_Assets, loadAssetFile)
{
	zarg_rd args(execute_data);

	str_ptr file;

	args.zstring(file, args.need(1));

	if (!args.throw_errors())
	{
		Assets* cobj = zval_toc<Assets>(ZEND_THIS);
		str_rc text = cobj->loadAssetFile(file);
		text.move_zv(return_value);
	}
}

ZEND_METHOD(Wcc_Assets, reset)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Assets* cobj = zval_toc<Assets>(ZEND_THIS);

	cobj->reset();
}

ZEND_METHOD(Wcc_Assets, styleHeader)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Assets* cobj = zval_toc<Assets>(ZEND_THIS);

	str_rc text = cobj->styleHeader();

	text.move_zv(text);
}

/*public function __construct(object $obj, ?string $rexpr = null);*/
ZEND_METHOD(Wcc_Replace, __construct)
{
	zarg_rd args(execute_data);

	obj_ptr obj;
	str_ptr rexpr;

	args.zobject(obj, args.need(1));
	args.zstring_null(rexpr, args.option(2));
	if (!args.throw_errors())
	{
		Assets* cobj = zval_toc<Assets>(ZEND_THIS);
		cobj->construct(obj, rexpr);
	}
}

/*public function eval(string $subj): string {}*/

ZEND_METHOD(Wcc_Replace, eval)
{
	zarg_rd args(execute_data);

	str_ptr subj;

	args.zstring(subj, args.need(1));
	if (!args.throw_errors())
	{
		Assets* cobj = zval_toc<Assets>(ZEND_THIS);
		str_rc value = cobj->eval(subj);
		value.move_zv(return_value);
	}
}

/*static public function property(object $obj, string $data) : string {}*/
ZEND_METHOD(Wcc_Replace, property) 
{
	zarg_rd args(execute_data);
	obj_ptr obj;
	str_ptr subj;

	if (args.zobject(obj, args.need(1)))
	{
		args.zstring(subj, args.need(2));
	}
	if (!args.throw_errors())
	{
		str_rc value = Replace::property(obj, subj);
		value.move_zv(return_value);
	}
}


#endif