#ifndef WCC_ASSETS_H
#define WCC_ASSETS_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

#ifndef SERVICE_ACCESS_H
#include "service_access.h"
#endif

namespace wcc {

class  Assets : public ServiceAccess {
protected:
	bool warn_missing_;
	bool render_lock_;

	obj_rc   config_;
	obj_rc   src_paths_;

	htab_rc  mark_;
	htab_rc  order_;

	htab_rc  bodyBlob_;
	htab_rc  headBlob_;

	htab_rc  inline_styles_;
	str_rc   web_;

protected:
	void markAdd(str_ptr item);
	void jsInline();

	static str_rc link_css(str_ptr webpath);

public:
	Assets();

	static base_obj_mgr<Assets>	omg;

	VIRTUAL_ZOBJPTR

	virtual   void debug_info(htab_rw hw);

	void construct();

	void add(val_ptr list);

	void addAssets(htab_ptr data);

	void addBlob(str_rc blob, bool header = false);

	htab_ptr addSourcePath(str_ptr path);

	htab_rc  getWebList(str_ptr selector, bool list = true, htab_ptr names);

	void addStyle(str_ptr style);

	str_rc footer();

 	bool has(str_ptr key);

 	str_rc header();

 	str_rc inline_css(str_ptr name);

 	str_rc link();

 	void loadAssetFile(str_ptr file);

 	void reset();

 	str_rc styleHeader();
};

}; // namespace wcc

#endif