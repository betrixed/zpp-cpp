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
	bool minify_;

	obj_rc   config_;
	obj_rc   src_paths_;

	htab_rc  mark_;
	htab_rc  order_;

	htab_rc  bodyBlob_;
	htab_rc  headBlob_;

	htab_rc  inline_styles_;
	str_rc   web_;
	str_rc   minify_name_;

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

	void addStyle(str_ptr style);

 	void clearCache();

 	str_rc cssHeader();

 	void cssMinify();

 	bool has(str_ptr key);

 	str_rc inline_css(str_ptr name);

 	void loadAssetFile(str_ptr file);

 	void reset();

 	void setMinify(str_ptr name);

 	str_rc styleHeader();

 	str_rc footer();

 	str_rc header();

};

}; // namespace wcc

#endif