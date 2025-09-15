#ifndef FOR_KEY_VALUE_CPP
#define FOR_KEY_VALUE_CPP

/**
  *  @file for_key_value.cpp
  *  @brief Iterate a HashTable for each key and value.
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @license Artistic License 2.0
  */

#ifndef FOR_KEY_VALUE_H
#include "for_key_value.h"
#endif

namespace zpp {

bool
for_key_value::start(HashTable* ht)
{
	lose();
	ht_ = ht;
	own();

	count_ = ht_->nNumOfElements;
	isPacked_ = (HT_FLAGS(ht_) & HASH_FLAG_PACKED);
	int elemSize = ZEND_HASH_ELEMENT_SIZE(ht_); //16 + {0|1}*(4) 16:Packed, 20:Not packed
	idx_ = 0;
	key_ = nullptr;
	next_ = ZEND_HASH_ELEMENT_EX(ht_, idx_, elemSize);
	count_  = ht_->nNumUsed - idx_ + 1; // count_ is subtracted first in next()

	return next();

}

bool 
for_key_value::ok() {
	if (count_)
		return true;
	lose();
	return false;
}

bool 
for_key_value::next()
{
	h_ = -1;
	key_ = nullptr;
	count_--;
	if (!count_) {
		lose();
		return false;
	}
	if (isPacked_)
	{
		zptr_ = next_;
		++next_;
		h_ = idx_;
		idx_++;
	}
	else {
		zptr_ = next_;
		Bucket* bkt = (Bucket*) next_;
		h_ = bkt->h;
		key_ = bkt->key;
		next_ = &((bkt+1)->val);
		//* No indirect yet for zptr_ in for this usage 
	}
	return true;
}

};

//for_key_value.cpp
#endif