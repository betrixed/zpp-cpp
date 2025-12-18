/* wccz extension for PHP */

#ifndef PHP_WCCZ_H
#define PHP_WCCZ_H

extern "C" {
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "php.h"
#include "ext/standard/info.h"
};

extern zend_module_entry wccz_module_entry;
# define phpext_wccz_ptr &wccz_module_entry

# define PHP_WCCZ_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_WCCZ)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_WCCZ_H */
