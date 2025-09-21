#ifndef HTAB_WALK_CPP
#define HTAB_WALK_CPP

/**
 * @file zpp/htab_rw.h
 * @author Michael Rynn <michael.rynn.500@gmail.com>
 * @brief htab_rw - read/write, not-reference counting, HashTable manager
 * @copyright Copyright (c) 2025
 * @license Artistic License 2.0
 */


#ifndef HTAB_WALK_H
#include "htab_walk.h"
#endif

namespace zpp {

htab_walk::htab_walk(): key_(), iterate_(0), ok_(false) 
{  
}
        
htab_walk::htab_walk(const htab_walk &c)
    : key_(c.key_), value_(c.value_), wrap_(c.wrap_), iterate_(c.iterate_)
{ 
}

bool htab_walk::start(HashTable* ht)
{
    wrap_ = ht;

    if (ht)
    {
        // val_ptr are reset by invalid
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

val_rc // static
htab_walk::first(HashTable* data)
{
    htab_walk wk;
    wk.start(data);
    return wk.value();
}

bool htab_walk::rewind()
{
    return start(wrap_);
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

void htab_walk::init()
  {
     invalid();
     wrap_ = nullptr;
     iterate_ = 0;
     ok_ = false;
  }

bool htab_walk::getdata()
{
    HashTable* ht = wrap_;

    key_.lose();
    // read in the current key
    // key can be ZVAL_STR_COPY of key, may require decref later


    zend_hash_get_current_key_zval_ex(ht, (zval*)key_, &iterate_);

    zend_string* keystr;
    val_ptr vkey(key_); // check what it is

    if (!vkey.getStringData(&keystr))
    {

        // const char* zero = ZSTR_VAL(keystr);
        /* 
        * reason for this was not well explained in PHP-CPP,
        * but an empty string key may be an empty storage slot
        * especially private and protected object property names
        */

        //return false;

        //if (*zero == (char)0)
        //{
        //    return false;
        //}

        // integer key expected
        if (vkey.isNull()) 
        {
            return invalid();
        }
    }

    // if the key is set to NULL, it means that the object is not at a valid position

    // iterator is at a valid position,  fetch data, add reference
 
    value_ = zend_hash_get_current_data_ex(ht, &iterate_);

    // we can now update the current data

    // if the key is private (it starts with a null character) we should return
    // false to report that the object is not in a completely valid state
    return true;
}


void htab_walk::release()
{
    key_.lose();
    value_.lose();
}
/**
 *  Invalidate the iterator
 *  @return bool
 */
bool htab_walk::invalid()
{
    key_.lose();
    value_.lose();
	
    ok_ = false;
    HashTable* ht = wrap_;
    if (ht)
    {
    // Move 1 past end
        zend_hash_internal_pointer_end_ex(ht, &iterate_);
        zend_hash_move_forward_ex(ht, &iterate_);
    }
    return false;
}

};//namespace

#endif
//htab_walk.cpp