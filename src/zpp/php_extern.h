#ifndef PHP_EXTERN_H
#define PHP_EXTERN_H

/**
 * @file zpp/php_extern.h
 * @author Michael Rynn
 * @brief  Include C headers from PHP distribution.
 */
#include <string>
#include <stdexcept>

extern "C" {
	#include <php.h>
	#include <Zend/zend.h>  
  	#include <Zend/zend_compile.h> 
    #include <Zend/zend_API.h>
	#include <zend_types.h>
	#include <Zend/zend_exceptions.h>
    #include <ext/json/php_json.h>
	#include <ext/standard/file.h>
	#include <ext/standard/php_array.h>
	#include <Zend/zend_smart_str.h>
};


#endif