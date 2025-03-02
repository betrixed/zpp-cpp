#ifndef HTAB_WALK_CPP
#define HTAB_WALK_CPP

bool htab_walk::start(HashTable* ht)
{
    wrap_ = ht;

    if (ht)
    {
    	ok_ = true;
        iterate_ = 0;

        zend_hash_internal_pointer_reset_ex(ht, &iterate_);
        if (getdata()) return true;

        return next();
    }
    else
    {
        return invalid();
    }
}


bool htab_walk::next()
{
	HashTable* ht = wrap_;

    if (!(ht && ok_)) return false;

    if (zend_hash_move_forward_ex(ht, &iterate_) == SUCCESS)
    {
        if (getdata()) return true;

        return next();
    }
    else
    {
        return invalid();
    }
}

bool htab_walk::prev()
{
    // leap out if we're not even iterating over a hash table
    HashTable* ht =  wrap_;

    if (!ht) return false;

    if (!ok_)
    {
        // move to end
        zend_hash_internal_pointer_end_ex(ht, &iterate_);
    }
    else if (zend_hash_move_backwards_ex(ht, &iterate_) == FAILURE)
    {
        // invalidate current position
        return invalid();
    }

    if (getdata()) return true;

    return prev(); // recurse
}

bool htab_walk::getdata()
{
	HashTable* ht = wrap_;

    data_.first.set_null();
    // read in the current key
    // if string key, data_.first has a ZVAL_STR_COPY of key 

    zend_hash_get_current_key_zval_ex(ht, data_.first, &iterate_);
    zend_string* keystr;
    if (data_.first.getStringData(&keystr))
    {
        const char* zero = ZSTR_VAL(keystr);
        /* 
        * reason for this was not well explained in PHP-CPP,
        * but an empty string key may be an empty storage slot
        */
        if (*zero == (char)0)
        {
            return false;
        }
    }

    // if the key is set to NULL, it means that the object is not at a valid position
    if (data_.first.isNull()) 
    {
    	return invalid();
    }

    // iterator is at a valid position,  fetch data, add reference
 
    zval* zptr = zend_hash_get_current_data_ex(ht, &iterate_);

    data_.second.set(zptr);

    // we can now update the current data

    // if the key is private (it starts with a null character) we should return
    // false to report that the object is not in a completely valid state
    return true;
}

/**
 *  Invalidate the iterator
 *  @return bool
 */
bool htab_walk::invalid()
{
	HashTable* ht = wrap_;
	ok_ = false;

	if (ht)
	{
    // Move 1 past end
    	zend_hash_internal_pointer_end_ex(ht, &iterate_);
    	zend_hash_move_forward_ex(ht, &iterate_);
    }
   
    data_ = std::pair<zval_own,zval_own>();

    return false;
}


#endif
//htab_walk.cpp