#ifndef WCC_STR_FNS_H
#define WCC_STR_FNS_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

using namespace zpp;


extern  str_rc phiz_uncamel(const zend_string *src, const zend_string *sep);

extern  str_rc phiz_camel(const zend_string *src, const zend_string *sep);

extern zend_class_entry* wcc_str_ce;
#endif
