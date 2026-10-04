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

#include "ZPP_VERSION.h"
# define PHP_WCCD_VERSION PHP_CPPZPP_VERSION
# define PHP_WCCD_AUTHOR PHP_CPPZPP_AUTHOR


# if defined(ZTS) && defined(COMPILE_DL_WCCD)
ZEND_TSRMLS_CACHE_EXTERN()
# endif


#include "zpp/base.h"

# define EXTN_MODULE_NAME  "wccd"
# define STATE_LIST_NAME    wccd_si_list

#include "zpp/state_module.h"

DECLARE_STATE_LIST

#endif	/* PHP_WCCD_H */
