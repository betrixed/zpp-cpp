/* wccr extension for PHP */

#ifndef PHP_WCCR_H
#define PHP_WCCR_H

extern "C" {
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include "php.h"
#include "ext/standard/info.h"
};

extern zend_module_entry wccr_module_entry;
# define phpext_wccr_ptr &wccr_module_entry

# define PHP_WCCR_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_WCCR)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#include "zpp/base.h"
#include "zpp/state_init.h"
  
#define STATE_LIST_NAME    wccr_si_list

#define STATE_INIT_ADD(si) STATE_LIST_NAME.add_si(&si);
#define STATE_MOD_INIT     STATE_LIST_NAME.call_mod_init();
#define STATE_MOD_END      STATE_LIST_NAME.call_mod_end();
#define STATE_REQ_INIT     STATE_LIST_NAME.call_req_init();
#define STATE_REQ_END      STATE_LIST_NAME.call_req_end();

extern zpp::state_list  STATE_LIST_NAME;
#endif	/* PHP_WCCR_H */
