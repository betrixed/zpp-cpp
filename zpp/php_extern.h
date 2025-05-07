#ifndef PHP_EXTERN_H
#define PHP_EXTERN_H

extern "C" {
	#include <php.h>
	#include <Zend/zend.h>  
  	#include <Zend/zend_compile.h> 
    #include <Zend/zend_API.h>
	#include <zend_types.h>
	#include <Zend/zend_exceptions.h>
    #include <Zend/zend_alloc.h>
    #include <ext/json/php_json.h>
	#include <Zend/zend_smart_str.h>
};

#include <string> 
#include <stdexcept>

#endif