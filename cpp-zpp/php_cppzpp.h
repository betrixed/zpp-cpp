/* cppzpp extension for PHP */

#ifndef PHP_CPPZPP_H
#define PHP_CPPZPP_H

extern "C" {
	#ifdef HAVE_CONFIG_H
	# include <config.h>
	#endif

	#include "php.h"
	#include "ext/standard/info.h"
}

extern zend_module_entry cppzpp_module_entry;
# define phpext_cppzpp_ptr &cppzpp_module_entry

#include "ZPP_VERSION.h"

# if defined(ZTS) && defined(COMPILE_DL_CPPZPP)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#include "zpp/base.h"

#define EXTN_MODULE_NAME   "cppzpp"
#define STATE_LIST_NAME    cppzpp_si_list

#include "zpp/state_module.h"

DECLARE_STATE_LIST

#endif	/* PHP_CPPZPP_H */
