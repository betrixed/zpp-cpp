/* wccd extension for PHP */

#ifndef PHP_WCCD_H
# define PHP_WCCD_H

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

extern "C" {
	#include "php.h"
	#include "ext/standard/info.h"
}
extern zend_module_entry wccd_module_entry;
# define phpext_wccd_ptr &wccd_module_entry

# define PHP_WCCD_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_WCCD)
ZEND_TSRMLS_CACHE_EXTERN()
# endif


#include "zpp/base.h"
  
#define STATE_LIST_NAME    wccd_si_list

#define STATE_INIT_ADD(si) STATE_LIST_NAME.add_si(&si);
#define STATE_MOD_INIT     STATE_LIST_NAME.call_mod_init();
#define STATE_MOD_END      STATE_LIST_NAME.call_mod_end();
#define STATE_REQ_INIT     STATE_LIST_NAME.call_req_init();
#define STATE_REQ_END      STATE_LIST_NAME.call_req_end();

extern zpp::state_list  STATE_LIST_NAME;

#endif	/* PHP_WCCD_H */
