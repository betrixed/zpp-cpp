#ifndef FOR_KEY_VALUE_H
#define FOR_KEY_VALUE_H

/**
  *  @file for_key_value.h
  *  @brief Iterate a HashTable for key and value.
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @license BSD 3-Clause License
  *  copyright 2025 Michael Rynn
  */

#ifndef PHP_EXTERN_H
#include "php_extern.h"
#endif

#ifndef HTAB_RC_H
#include "htab_ptr.h"
#endif

namespace zpp {

/**
  * @class for_key_value
  * @brief Iterate a HashTable for key and value.
  * @details	
 * Iterate the array passed to start()   for(.start(array); .ok(); .next()).
 * Can also be used in while, or sequence of statements.
 * Unlike htab_walk, usage requires 
 * call of value() key() or index() after start() or next() loop,
 * as these values will change with each interation.
 * This depends on internal details of HashTable.
 * 
 */
class for_key_value : public htab_rc {
protected:
	bool       		isPacked_;
	int        		count_; // countdown, and ok condition
	size_t     		idx_;   
	int             h_;
	zend_string*  	key_;
	zval*			zptr_;
	zval*			next_;
public:
	zval* 				  value() const { return zptr_; }
	zend_long       index() const { return h_; }
	zend_string*		key() const { return key_; }

	bool start(HashTable* ht);
	bool ok();
	bool next();
};

}; //namespace


#endif