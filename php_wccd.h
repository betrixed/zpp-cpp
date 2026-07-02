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
# define PHP_WCCD_AUTHOR  "Michael Rynn"

# if defined(ZTS) && defined(COMPILE_DL_WCCD)
ZEND_TSRMLS_CACHE_EXTERN()
# endif


#include "zpp/base.h"

# define EXTN_MODULE_NAME  "wccd"
# define STATE_LIST_NAME    wccd_si_list

#include "zpp/state_module.h"

DECLARE_STATE_LIST

#endif	/* PHP_WCCD_H */
