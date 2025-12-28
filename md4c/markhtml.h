
#ifndef MARKHTML_H
#define MARKHTML_H


#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif



namespace wcc {

using namespace zpp;

class MTHInit : public state_init {
public:

    str_intern  pflags_s;
    str_intern  rflags_s;
    str_intern  cb_link_s;
    str_intern  title_attr;
    str_intern  href_attr;

    MTHInit() : state_init() {
        
    }

    void init() override  {
    	pflags_s = "pflags";
        rflags_s = "rflags";
    	cb_link_s = "cblink";
    	title_attr = "title";
    	href_attr = "href";
    }
};

extern MTHInit  MTH;

class  MarkToHtml : public base_d {
protected:
	int     pflags_;
	int     rflags_;
	str_buf htmlbuf_;
	str_rc  input_;

public:
	static base_obj_mgr<MarkToHtml> omg;

	void debug_info(htab_rw info) override;

	void construct(int parse = 0, int render = 0);

	str_return text(str_ptr markdown);

	void html_append(const char* txt, size_t tlen);

	htab_rc cblink(htab_ptr link);
};


}// namespace wcc

#endif
