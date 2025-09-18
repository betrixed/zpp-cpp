/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Michael Rynn                                                 |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_WCC_H
#define PHP_WCC_H

// zend_module_entry wcc_module_entry;
//#define phpext_wcc_ptr &wcc_module_entry
#define WCC_VERSION "0.5.4"


#ifdef __cplusplus
extern "C" {
#endif

# if defined(ZTS) && defined(COMPILE_DL_WCC)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

PHP_MSHUTDOWN_FUNCTION(wcc);
PHP_MINFO_FUNCTION(wcc);
PHP_RINIT_FUNCTION(wcc);

#ifdef __cplusplus
};
#endif

#endif	/* PHP_WCC_H */
