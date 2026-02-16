/* wcch extension for PHP */

#ifndef PHP_WCCH_H
# define PHP_WCCH_H

extern "C" {
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include "php.h"
#include "ext/standard/info.h"
};

extern zend_module_entry wcch_module_entry;
# define phpext_wcch_ptr &wcch_module_entry

# define PHP_WCCH_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_WCCH)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#include "zpp/base.h"
  
#define STATE_LIST_NAME    wcch_si_list

#define STATE_INIT_ADD(si) STATE_LIST_NAME.add_si(&si);
#define STATE_MOD_INIT     STATE_LIST_NAME.call_mod_init();
#define STATE_MOD_END      STATE_LIST_NAME.call_mod_end();
#define STATE_REQ_INIT     STATE_LIST_NAME.call_req_init();
#define STATE_REQ_END      STATE_LIST_NAME.call_req_end();

extern zpp::state_list  STATE_LIST_NAME;

#endif	/* PHP_WCCH_H */
