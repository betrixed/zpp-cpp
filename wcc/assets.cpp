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

namespace wcc {

using namespace zpp;

base_obj_mgr<Assets> Assets::omg;

class ASinit : public state_init {
public:
	ASinit() : state_init() {}

	str_intern assets_cfg;
	str_intern css_str;
	str_intern requires_str;
	str_intern run_str;
	str_intern script_end;
	str_intern script_tag;
	str_intern style_end;
	str_intern style_tag;
	str_intern web_dir;
	str_intern js_inline;




	

	void init() override 
	{
		assets_cfg = "assets_cfg";
		css_str = "css";
		requires_str = "requires";
		run_str = "run";
		script_tag = "<script>";
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
	if (names.isNull() || names.isEmpty())
	{
		order = order_;
	}
	else if (names.isString())
	{
		htab_write hw(order);
		hw.push_back(names.zstr());
	}
	else {
		order = names.zarray();
	}

	for_key_value w1;

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
						result.push_back(wpath);
					}
				}
				else {
					result.push_back(items);
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

		if (warn_missing_)
		{
			verify(path);
		}
		str_rc script = file_get_contents(path);
		str_buf buf;
		buf << ASI.script_tag << script << ASI.script_end;
		script = buf.zstr();
		addBlob(script);
	}
}

/*
	bool warn_missing_;
	bool render_lock_;
	bool minify_;
*/
Assets::Assets() : ServiceAccess(), warn_missing_(true), render_lock_(false)
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

	web_ = run->property(ASI.web_dir);

	str_rc assets_file = run->property(ASI.assets_cfg);

	if (assets_file.size())
	{
		this->loadAssetFile(assets_file);
	}
	else {
		assets_ = Config::omg.new_zobj();
	}
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
	if (!mark_.has(item))
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
			val_ptr requires = alist.get(ASI.requires_str);
			if (requires.ok())
			{
				this->add(requires);
			}
			this->order_.push_back(item);
			this->mark_.setbool(item,true);
		}
	}
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
		for(loop.start(nlist.zarray()), loop.ok(), loop.next())
		{
			str_rc name = loop.value().zstr();
			this->markAdd(name);
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
Assets::addBlob(str_rc blob, bool header = false)
{
	if (header)
	{
		headBlob_.push_back(blob);
	}
	else {
		bodyBlob_.push_back(blob);
	}
}

htab_ptr 
Assets::addSourcePath(str_ptr path)
{
	SourceList* paths = zobj_toc<SourceList>(src_paths_);
	paths->addPath(path);
	return paths->getPaths();
}

void 
Assets::addStyle(str_ptr style)
{
	str_rc sct = str_replace(ASI.style_tag, str_rc::empty_str, style);
	sct = str_replace(ASI.style_end, str_rc::empty_str, sct);
	inline_styles_.push_back(sct);
}

str_rc 
Assets::footer()
{
	render_lock_ = true;
	this->jsInline();
}

obj_ptr 
Assets::getSearchList()
{
	return src_paths_;
}


bool 
Assets::has(str_ptr key)
{
	return assets_.has(key);
}

str_rc 
Assets::header()
{
	render_lock_ = true;
	return getHeadBlob();
}


str_rc 
Assets::inline_css(str_ptr name)
{
	htab_rc paths = getWebList(ASI.css_str, name);
	str_buf buf;

	if (paths.size())
	{
		for_key_value w1;

		for(w1.start(paths); w1.ok(); w1.next())
		{
			str_rc css_path = w1.value();
			if (css_path.size())
			{
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

}

void 
Assets::loadAssetFile(str_ptr file)
{
	
}

void 
Assets::reset()
{
	
}

str_rc 
Assets::styleHeader()
{
	
}


}; //end namespace wcc

#endif