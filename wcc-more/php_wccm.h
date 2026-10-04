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

#include "ZPP_VERSION.h"
#define PHP_WCCM_VERSION PHP_CPPZPP_VERSION
#define PHP_WCCM_AUTHOR  PHP_CPPZPP_AUTHOR

#if defined(ZTS) && defined(COMPILE_DL_WCCM)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#include "zpp/base.h"
 
# define EXTN_MODULE_NAME   "wccm"
# define STATE_LIST_NAME    wccm_si_list

#include "zpp/state_module.h"

DECLARE_STATE_LIST

#endif	/* PHP_WCCM_H */
