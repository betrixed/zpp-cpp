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

#define EXTN_MODULE_NAME  "wccr"
#define STATE_LIST_NAME    wccr_si_list

#include "zpp/state_module.h"

DECLARE_STATE_LIST

#endif	/* PHP_WCCR_H */
