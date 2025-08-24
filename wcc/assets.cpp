#ifndef WCC_ASSETS_CPP
#define WCC_ASSETS_CPP

#ifndef WCC_ASSETS_H
#include "assets.h"
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

	str_intern run_str;
	str_intern web_dir;
	str_intern assets_cfg;

	void init() override 
	{
		run_str = "run";
		web_dir = "web_dir";
		assets_cfg = "assets_cfg";
	}


};

ASinit  ASI;

/*
	bool warn_missing_;
	bool render_lock_;
	bool minify_;
*/
Assets::Assets() : ServiceAccess(), warn_missing_(true), render_lock_(false), minify_(false)
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

void 
Assets::add(val_ptr list)
{
}

void 
Assets::addAssets(htab_ptr data)
{
}

void 
Assets::addBlob(str_rc blob, bool header = false)
{
}

htab_ptr 
Assets::addSourcePath(str_ptr path)
{
}

void 
Assets::addStyle(str_ptr style)
{
}

void 
Assets::clearCache()
{
	
}

str_rc 
Assets::cssHeader()
{
	
}

void 
Assets::cssMinify()
{
	
}

bool 
Assets::has(str_ptr key)
{
	
}

str_rc 
Assets::inline_css(str_ptr name)
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

void 
Assets::setMinify(str_ptr name)
{
	
}

str_rc 
Assets::styleHeader()
{
	
}

str_rc 
Assets::footer()
{
	
}

str_rc 
Assets::header()
{
	
}

}; //end namespace wcc

#endif