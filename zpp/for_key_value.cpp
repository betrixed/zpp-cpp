#ifndef FOR_KEY_VALUE_CPP
#define FOR_KEY_VALUE_CPP

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
	zptr_ = ZEND_HASH_ELEMENT_EX(ht_, idx_, elemSize);
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
		++zptr_;
		h_ = idx_;
		idx_++;
	}
	else {
		Bucket* bkt = (Bucket*) zptr_;
		zptr_ = &(bkt+1)->val;
		h_ = bkt->h;
		key_ = bkt->key;
		//* No indirect yet for zptr_ in for this usage 
	}
	return true;
}

};

//for_key_value.cpp
#endif