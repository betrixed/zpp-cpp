/* wccm extension for PHP */

#ifndef PHP_WCCM_H
#define PHP_WCCM_H

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

extern "C" {
	#include "php.h"
	#include "ext/standard/info.h"
}

extern zend_module_entry wccm_module_entry;
#define phpext_wccm_ptr &wccm_module_entry

#define PHP_WCCM_VERSION "0.1.0"

#if defined(ZTS) && defined(COMPILE_DL_WCCM)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif	/* PHP_WCCM_H */
