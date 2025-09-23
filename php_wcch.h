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

#endif	/* PHP_WCCH_H */
