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

#ifndef WCC_REPLACE_H
#include "replace.h"
#endif

#ifndef FN_CALL_H
#include "zpp/fn_call.h"
#endif

#ifndef WCC_CACHEMGR_H
#include "cachemgr.h"
#endif

#ifndef ASSETS_ARGINFO_H
#define ASSETS_ARGINFO_H

extern "C" {
	#include "stub/assets_arginfo.h"
}
#endif

namespace wcc {

using namespace zpp;

base_obj_mgr<Assets> Assets::omg;



void ASinit::init() 
	{
		assets_cfg = "assets_cfg";
		assets_str = "assets";
		body_blob = "bodyBlobs";

		cache_mgr =  "cache_mgr";
		css_str =    "css";
		file_cache = "file_cache";
		fwd_slash = "/";

		head_blob = "headBlobs";
		inline_styles = "inline_styles";

		js_inline = "js-inline";
		js_str = "js";
		link_str = "link";
		mark_str = "mark";
		order_str = "order";

		prop_expr = R"(#@([a-zA-Z][\w\d]*)#)";
		read_cache = "readcache";
		requires_str = "requires";
		run_str = "run";

		script_tag = "<script>";
		source = "source";

		src_paths = "src_paths";
		script_end = "</script>";
		style_end = "</style>";
		style_tag = "<style>";
		web_dir = "web_dir";
	}

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
		htab_rc asset = assets_.array_property(name);
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

void 
Assets::setRun(obj_ptr obj)
{
	run_ = obj;
	obj_ptr run = run_;
	web_ = run.str_property(ASI.web_dir);

	str_rc assets_file = run.str_property(ASI.assets_cfg);

	if (assets_file.size())
	{
		this->loadAssetFile(assets_file);
	}
}

str_rc 
Assets::jsPut()
{
	str_buf buf;

	render_lock_ = true;
	htab_rc paths = getWebList(ASI.js_str);
	for_key_value kv1;

	for(kv1.start(paths); kv1.ok(); kv1.next())
	{
		str_rc wpath = kv1.value();
		if (verify_path(wpath)) {
			buf << script_wrap(wpath);
		}
		else 
			break;
	}
	return buf.zstr();
}

bool
Assets::jsInline()
{
	htab_rc ipaths = getWebList(ASI.js_inline, val_ptr(), false);
	for_key_value w1;
	for(w1.start(ipaths); w1.ok(); w1.next())
	{
		htab_rc jsline = w1.value();
		htab_rc slist = jsline.get(ASI.source);
		if (slist.size())
		{
			for_key_value w2;
			for(w2.start(slist); w2.ok(); w2.next())
			{
				str_rc wpath = w2.value();

				if ( web_path(wpath) )
				{
					str_rc script =  file_content(wpath);
					if (script.size())
					{
						str_buf buf;
						buf << ASI.script_tag << script << ASI.script_end;
						script = buf.zstr();
						addBlob(script);
					}
				}
				else {
					zend_throw_error(zend_ce_error,"js-inline source %s not found.", wpath.data());
					return false;
				}
			}
		}	
		
	}
	return true;
}

Assets::Assets() : base_d(), render_lock_(false)
{
}

void 
Assets::construct()
{
	//zend_printf("Assets::construct\n");
	
	val_rc nullval; // null value


	src_paths_ = SearchList::omg.new_zobj();
	SearchList* slist = zobj_toc<SearchList>(src_paths_);
	slist->construct(nullval);

	assets_ = Config::omg.new_zobj();

	obj_rc values = Services::service(ASI.run_str);

	if (values.ok())
	{
		setRun(values);
	}
	else {
		run_ = Config::omg.new_zobj();
	}

}

void 
Assets::debug_info(htab_rw di)
{
	di.set(ASI.src_paths, src_paths_);
	di.set(ASI.assets_str, assets_);
	di.set(ASI.order_str, order_);
	di.set(ASI.mark_str, mark_);
	di.set(ASI.inline_styles, inline_styles_);
	di.set(ASI.web_dir, web_);
	di.set(ASI.head_blob, headBlob_);
	di.set(ASI.body_blob, bodyBlob_);
}

void Assets::destruct()
{
	run_.init();
	src_paths_.init();
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
			if (!verify_path(wpath))
			{
				break;
			}
			buf << link_css(wpath);
		}
	}
	return buf.zstr();
}

str_rc //static 
Assets::script_wrap(str_ptr path)
{
	str_buf buf;

	buf << R"(<script src=")" << path << R"("></script>)" << endl;

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

// create file at wpath, by topy from
// one of listed source folders
bool
Assets::source_path(str_ptr wpath)
{

	str_rc srcfile = findSourceFile(wpath);

	bool exists = (srcfile.size()) ? true : false;
	if (exists)
	{
		zend_result copied = php_copy_file(srcfile.data(), wpath.data());
		if (copied != SUCCESS)
		{
			exists = false;
		}
	}
	if (!exists)
	{
		zend_throw_error(zend_ce_error,"File '%s' not found", wpath.data());
	}
	return exists;
}

bool 
Assets::web_path(str_rc& path)
{
	str_rc fpath(path);
	int check = fpath.find('@');
	if (check >= 0)
	{
		Replace path_subst(run_);
		fpath = path_subst.eval(fpath);
	}
	if (fpath.starts_with(ASI.fwd_slash))
	{
		fpath = web_ + fpath;
	}

	bool exists = file_exists(fpath);

	if(!exists)
	{
		exists = this->source_path(fpath);
	}

	if (exists)
	{
		path = fpath;
	}
	return exists;
}

bool 
Assets::verify_path(str_rc& p_inout)
{
	str_rc fpath(p_inout);

	int check = fpath.find('@');
	if (check >= 0)
	{
		Replace path_subst(run_);
		fpath = path_subst.eval(fpath);
		p_inout = fpath;
	}
	str_rc real_path = fpath;
	// starting with '/'
	if (fpath.starts_with(ASI.fwd_slash)) 
	{
		real_path = web_ + real_path;
	}

	if (!file_exists(real_path)) 
	{
		if (this->source_path(real_path))
		{
			return true;
		}
		else {
			return false;
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
		this->markAdd(nlist.zstr());
	}
}

htab_rc 
Assets::addAssets(htab_ptr data)
{
	htab_walk wk;
	auto key = wk.key();
	auto value = wk.value();

	htab_rc keys_added;
	htab_rw hw(keys_added);

	for(wk.start(data); wk.ok(); wk.next())
	{
		hw.push_back(key);
		assets_.property(key.zstr(), value);
	}
	//showdata("keys_added", keys_added);
	return keys_added;
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
	str_rc result;
	
	render_lock_ = true;
	if (!this->jsInline())
	{
		return result;
	}
	str_buf buf;

	str_rc temp = jsPut();
	buf << temp;

	temp = implode_blob(bodyBlob_);
	buf << temp;
	result = buf.zstr();
	
	return result;
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

	Replace pathnames(run_, ASI.prop_expr);

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

				str_rc data = file_content(css_path);
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
	htab_rc result;

	if (paths.size())
	{
		htab_rw hw(result);
		Replace pathnames(run_, ASI.prop_expr);
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

htab_rc 
Assets::loadAssetFile(str_ptr file)
{
	//printf("Assets::loadAssetFile %s\n", file.data());

	htab_rc result = htab_ptr::empty_array();

	if (! file_exists(file))
	{
		zend_throw_error(zend_ce_error,"Asset file '%s' not found", file.data());
		return result;
	}

	obj_rc cache_mgr = Services::service(ASI.cache_mgr);

	if(!cache_mgr.ok())
	{
		zend_throw_error(zend_ce_error,"No Cache Mgr service");
		return result;
	}

	/*
	val_rc filename(file);
	val_rc cachename(ASI.file_cache);
	htab_rc data = cache_mgr.call(ASI.read_cache, filename, cachename);
	*/
	CacheMgr* cmgr = zobj_toc<CacheMgr>(cache_mgr);

	val_return vdata = cmgr->readCache(file, ASI.file_cache);

	if (vdata.throw_errors())
	{
		return htab_ptr::empty_array();
	}
	htab_rc data = vdata.value_.zarray();


	//showdata("cached data:  ", data);

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

	val_rc temp = data.get(ASI.assets_str);
	if (temp.isObject())
	{
		obj_rc cfg_obj = temp.zobject();
		if (cfg_obj.ok())
		{
			Config* cfg = zobj_toc<Config>(cfg_obj);
			temp = cfg->toArray();
		}
	}
	if (temp.isArray())
	{
		result = this->addAssets(temp.zarray());
		//showdata("result", result);
	}
	return result;
}

void 
Assets::reset()
{
	render_lock_ = false;
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



str_rc 
Assets::implode_blob(htab_ptr blobs)
{
	str_ptr estr = str_ptr::empty_str();
	if (blobs.size())
	{
		return implode(estr, blobs);
	}
	else {
		return estr;
	}
}

void 
Assets::unmark(str_ptr item)
{
	if (mark_.has_key(item)) 
	{
		int ix = order_.value_index(item);
		if (ix >= 0)
		{
			array_splice(this->order_, ix, 1);
		}
		htab_rw hw(mark_);
		hw.unset(item);
	}
}

/*
    public function unmark(string $item) : void
    {
        if (isset($this->mark[$item]))
        {
            $ix = array_search($item,$this->order);
            if (is_integer($ix))
            {
                array_splice($this->order,$ix,1);
            }
            unset($this->mark[$item]);
        }
    }*/


}; //end namespace wcc
//============================================================================================

using namespace wcc;
using namespace zpp;

ZEND_METHOD(Wcc_Assets, __construct)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Assets* cobj = zval_toc<Assets>(ZEND_THIS);

	cobj->construct();
}

ZEND_METHOD(Wcc_Assets, __destruct)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Assets* cobj = zval_toc<Assets>(ZEND_THIS);

	cobj->destruct();
}

ZEND_METHOD(Wcc_Assets, add)
{
	zarg_rd args(execute_data);

	val_ptr list(args.need(0));

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
	args.zarray(data, args.need(0));

	if (!args.throw_errors())
	{
		Assets* cobj = zval_toc<Assets>(ZEND_THIS);
		htab_rc result = cobj->addAssets(data);
		result.move_zv(return_value);
	}
}

ZEND_METHOD(Wcc_Assets, addBlob)
{
	zarg_rd args(execute_data);
	str_ptr blob;
	bool    headblob = false;
	args.zstring(blob, args.need(0));
	args.zbool(headblob, args.option(1));
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
	args.zstring(path, args.need(0));
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
	args.zstring(style, args.need(0));
	if (!args.throw_errors())
	{
		Assets* cobj = zval_toc<Assets>(ZEND_THIS);
		cobj->addStyle(style);
	}
}

ZEND_METHOD(Wcc_Assets, cssHeader)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Assets* cobj = zval_toc<Assets>(ZEND_THIS);

	str_rc text = cobj->cssHeader();
	text.move_zv(return_value);
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

	args.zstring(typekey, args.need(0));

    names = args.option(1);

	if (names.ok() && (names.isString() || names.isArray())) {
		args.zbool(aslist, args.option(2));
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

	args.zstring(namekey, args.need(0));
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

	args.zstring(name, args.need(0));
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

	args.zstring(file, args.need(0));
	htab_rc result;

	if (!args.throw_errors())
	{
		Assets* cobj = zval_toc<Assets>(ZEND_THIS);
		result = cobj->loadAssetFile(file);
	}
	else {
		result = htab_ptr::empty_array();
	}
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Assets, reset)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Assets* cobj = zval_toc<Assets>(ZEND_THIS);

	cobj->reset();
}

ZEND_METHOD(Wcc_Assets, setRun)
{
	zarg_rd args(execute_data);

	obj_ptr env;

	env = args.obj(args.need(0));

	if (!args.throw_errors())
	{
		Assets* cobj = zval_toc<Assets>(ZEND_THIS);
		cobj->setRun(env);
	}
}

ZEND_METHOD(Wcc_Assets, styleHeader)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Assets* cobj = zval_toc<Assets>(ZEND_THIS);

	str_rc text = cobj->styleHeader();

	text.move_zv(return_value);
}

ZEND_METHOD(Wcc_Assets, unmark)
{
	zarg_rd args(execute_data);

	str_ptr item;

	args.zstring(item, args.need(0));

	if (!args.throw_errors())
	{
		Assets* cobj = zval_toc<Assets>(ZEND_THIS);
		cobj->unmark(item);
	}
}

PHP_MINIT_FUNCTION(wcc_assets_reg)
{

	Assets::omg.classEntry(register_class_Wcc_Assets());

	STATE_INIT_ADD(ASI)
	
	return SUCCESS;

}
#endif