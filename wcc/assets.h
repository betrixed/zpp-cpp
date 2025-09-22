#ifndef WCC_ASSETS_H
#define WCC_ASSETS_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

#ifndef SERVICE_ACCESS_H
#include "service_access.h"
#endif

namespace wcc {

class ASinit : public state_init {
public:
	ASinit() : state_init() {}

	str_intern assets_cfg;
	str_intern assets_str;
	str_intern body_blob;

	str_intern cache_all;
	str_intern css_str;
	str_intern file_cache;
	str_intern fwd_slash;

	str_intern head_blob;
	str_intern inline_styles;

	str_intern js_str;
	str_intern js_inline;
	str_intern link_str;
	str_intern mark_str;
	str_intern order_str;

	str_intern prop_expr;
	str_intern read_cache;
	str_intern requires_str;
	str_intern run_str;

	str_intern script_end;
	str_intern script_tag;
	str_intern source;

	str_intern src_paths;
	str_intern style_end;
	str_intern style_tag;

	str_intern web_dir;
	
	

	void init() override;

};

extern ASinit ASI;

class  Assets : public base_d {
protected:
	bool render_lock_;

	obj_rc   assets_;
	obj_rc   src_paths_;
	obj_rc   run_;

	htab_rc  mark_;
	htab_rc  order_;

	htab_rc  bodyBlob_;
	htab_rc  headBlob_;

	htab_rc  inline_styles_;
	str_rc   web_;

protected:
	str_rc findSourceFile(str_ptr path);
	htab_rc filterPaths(htab_ptr paths);

	bool markAdd(str_ptr item);
	bool jsInline();
	str_rc jsPut();
	bool verify_path(str_rc& p_inout);
	bool web_path(str_rc& path);
	bool source_path(str_ptr path);


	static str_rc link_css(str_ptr webpath);
	static str_rc script_wrap(str_ptr path);

	
public:
	Assets();

	static base_obj_mgr<Assets>	omg;

	static str_rc implode_blob(htab_ptr blobs);

	VIRTUAL_ZOBJPTR

	virtual   void debug_info(htab_rw hw);

	void construct();
	void destruct();

	void add(val_ptr list);

	htab_rc addAssets(htab_ptr data);

	void addBlob(str_rc blob, bool header = false);

	htab_ptr addSourcePath(str_ptr path);

	str_rc cssHeader();

	htab_rc  getWebList(
		str_ptr selector, 
		val_ptr names = val_ptr(), 
		bool list=true);

	void addStyle(str_ptr style);

	str_rc footer();

 	bool has(str_ptr key);

 	obj_ptr getSearchList();

 	str_rc header();

 	str_rc inline_css(str_ptr name);

 	str_rc link();

 	htab_rc loadAssetFile(str_ptr file);

 	void reset();

 	str_rc styleHeader();
};

}; //namespace wcc

#endif