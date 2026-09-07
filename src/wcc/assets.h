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

	str_intern assets_cfg;
	str_intern assets_str;
	str_intern body_blob;
	str_intern config_dir;
	str_intern classname_str;

	str_intern cache_mgr;
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
	bool     throwIfDuplicate_;
	obj_rc   src_paths_;
	obj_rc   run_; // a properties holding object

	htab_rc  mark_;
	htab_rc  order_;

	htab_rc  bodyBlob_;
	htab_rc  headBlob_;

	htab_rc  inline_styles_;
	str_rc   web_;

	//module management
	htab_rc  loaded_;
	htab_rc  modules_;
	obj_rc   activeModule_;
	htab_rc  moduleCfg_;

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

	error_return construct();
	void destruct();

	void add(val_ptr list);

	htab_return addAssets(htab_ptr data);

	void addBlob(str_rc blob, bool header = false);

	htab_ptr addSourcePath(str_ptr path);

	

	str_rc cssHeader();

	htab_rc  getWebList(
		str_ptr selector, 
		val_ptr names = val_ptr(), 
		bool list=true, bool unset=false);

	void addStyle(str_ptr style);

	str_rc footer();

 	bool has(str_ptr key);

 	obj_ptr getSearchList();

 	str_rc header();

 	str_rc inline_css(str_ptr name);

 	str_rc jsPull(str_ptr name);

 	str_rc link();

 	htab_return loadAssetFile(str_ptr file);

 	void reset();

 	str_rc styleHeader();

 	void unmark(str_ptr item);

 	void setRun(obj_ptr obj);

 	bool_return clearCache();
 	// modules

 	obj_rc     getActiveModule();

 	obj_return addModule(str_ptr name, val_ptr data);

 	obj_return getDefaultModule();

 	str_rc getModuleName();

 	obj_rc getModule(str_ptr name);

 	obj_return setModule(str_ptr name);

 	void setModuleCfg(htab_ptr modlist);


};

}; //namespace wcc

#endif