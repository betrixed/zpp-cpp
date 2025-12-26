
#ifndef MARKHTML_H
#define MARKHTML_H


#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif



namespace wcc {

using namespace zpp;

class  MarkToHtml : public base_d {
protected:
	int     pflags_;
	int     rflags_;
	str_buf htmlbuf_;
	str_rc  input_;

public:
	static base_obj_mgr<MarkToHtml> omg;

	void construct(int parse = 0, int render = 0);

	str_return text(str_ptr markdown);

	void html_append(const char* txt, size_t tlen);
};


}// namespace wcc

#endif
