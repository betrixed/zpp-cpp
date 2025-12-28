#ifndef MARKHTML_CPP
#define MARKHTML_CPP


#ifndef MARKHTML_H
#include "markhtml.h"
#endif



// order of c files important


#include "md4cp.cpp"
#include "src/entity.c"
#include "md-html.cpp"

#ifndef MARKHTML_ARGINFO
#define MARKHTML_ARGINFO
extern "C" {
    #include "stub/md2html_arginfo.h"
}
#endif

namespace wcc {

base_obj_mgr<MarkToHtml> MarkToHtml::omg;

/*
struct membuffer {
    char* data_;
    size_t cap_;
    size_t size_;

    membuffer(size_t nsize)
    {
    	size_ = 0;
    	cap_ = 0;
    	data_ = nullptr;
    }

    ~membuffer()
    {
    	if (data_) {
    		efree(data_);
    		data_ = nullptr;
    	}
    }

    error_return
    capacity(size_t nsize)
    {
    	error_return result;

    	data_ = erealloc(data_, nsize);
    	if (data_ == nullptr)
    	{
    		result.error() << "resize(" << nsize << ") failed";
    	}
    	else {
    		cap_ = nsize;
    	}
    	return result;
    }

    error_return
    append(const char* data, size_t dlen)
    {
    	error_return result;
    	if (cap_ < size_ + dlen){
    		size_t ncap = size_ + size_/ 2 + dlen;
    		result = this->capacity(ncap);
    		if (result.has_errors())
    		{
    			return result;
    		}
    	}
    	memcpy(data_ + size_, data, dlen);
    	size_ += dlen;
    	return result;
    }
};
*/

MTHInit  MTH;

void 
MarkToHtml::debug_info(htab_rw info)
{
    info.set(MTH.pflags_s, pflags_);
    info.set(MTH.rflags_s, rflags_);
}


static void  
md4c_callback(const MD_CHAR* html, MD_SIZE hlen, obj_ptr userdata)
{
    MarkToHtml* mth = (MarkToHtml*) zobj_toc<MarkToHtml>(userdata);

    mth->html_append(html, hlen);
}   

void 
MarkToHtml::construct(int parse, int render)
{
	pflags_ = parse;
    rflags_ = render;
}

str_return 
MarkToHtml::text(str_ptr markdown)
{
    str_return       result;

    input_ = markdown;
    htmlbuf_.reset();

    int ret = md_html(input_.data(), input_.size(),  md4c_callback,
                this->self_, pflags_, rflags_);

    if (ret)
    {
        result.error() << "md4c error " << ret;
    }

    result = htmlbuf_.zstr();

    return result;
}

void 
MarkToHtml::html_append(const char* txt, size_t tlen)
{
    htmlbuf_.append(txt, tlen);
}

};

using namespace wcc;


ZEND_METHOD(Wcc_MarkToHtml, __construct)
{
    zarg_rd args(execute_data);

    zend_long parse = 0;
    zend_long render = 0;

    args.zlong(parse, args.option(0));
    args.zlong(render, args.option(1));

    if (!args.throw_errors())
    {
        MarkToHtml* mth = zval_toc<MarkToHtml>(ZEND_THIS);
        mth->construct(parse,render);
    }
}

ZEND_METHOD(Wcc_MarkToHtml, text)
{
    zarg_rd args(execute_data);

    str_ptr    input;
    str_return result;

    args.zstring(input, args.need(0));

    if (!args.throw_errors())
    {
        MarkToHtml* mth = zval_toc<MarkToHtml>(ZEND_THIS);
        result = mth->text(input);
        result.throw_errors();
        result.value_.move_zv(return_value);
    }
}

PHP_MINIT_FUNCTION(Wcc_MarkToHtml_reg)
{
    MarkToHtml::omg.classEntry(register_class_Wcc_MarkToHtml());

    return SUCCESS;
}

#endif
