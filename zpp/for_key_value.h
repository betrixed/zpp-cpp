#ifndef FOR_KEY_VALUE_H
#define FOR_KEY_VALUE_H
//for_key_value.h

#ifndef HTAB_MGR_H
#include "htab_mgr.h"
#endif

namespace zpp {

/**
 * Unlike htab_walk, usage requires 
 * call of value() key() or index()
 * as values will change
 * every iteration,
 */
class for_key_value : public htab_mgr {
protected:
	bool       		isPacked_;
	int        		count_; // countdown, and ok condition
	size_t     		idx_;
	int             h_;
	zend_string*  	key_;
	zval*			zptr_;
	zval*			next_;
public:
	zval* 				value() const { return zptr_; }
	zend_long       	index() const { return h_; }
	zend_string*		key() const { return key_; }

	bool start(HashTable* ht);
	bool ok();
	bool next();
};

}; //namespace


#endif