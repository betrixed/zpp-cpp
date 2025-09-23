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

#endif	/* PHP_WCCR_H */
