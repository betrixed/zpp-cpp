#ifndef HTAB_WALK_H
#define HTAB_WALK_H

/**
 * @file zpp/htab_rw.h
 * @author Michael Rynn <michael.rynn.500@gmail.com>
 * @brief htab_walk - A simple HashTable iterator.
 * @copyright Copyright (c) 2025
 * @license Artistic License 2.0
 */

#ifndef VAL_PTR_H
#include "val_ptr.h"
#endif

#ifndef HTAB_PTR_H
#include "htab_ptr.h"
#endif

#ifndef VAL_RC_H
#include "val_rc.h"
#endif

namespace zpp {


    /**
     * @class htab_walk
     * @brief A simple HashTable iterator.
     * @details This class provides a simple interface to iterate over a HashTable*.
     * It increments or decrements the current position and provides methods to access a copy of the key and value
     * as a val_ptr.
     * 
     */
    class htab_walk {
    private:
        val_rc key_;
        val_rc value_;

    	htab_ptr  wrap_;

        HashPosition iterate_ = 0;

        bool ok_ = false;
    public:

        static val_rc first(HashTable* data);

        htab_walk();
        htab_walk(const htab_walk& c);
        

        ~htab_walk() = default;


        //const htab_walk& operator=(int pos);

         bool ok() const 
         {
            return ok_;
         }
        
        /** restore original empty state */
        void init();

        htab_walk *clone()
        {
            // create a new instance
            return new htab_walk(*this);
        }

        bool start(HashTable* ht);

        bool rewind();
        
        bool next();

        bool prev();

        bool operator=(const htab_walk& c) const
        {
            return (wrap_ == c.wrap_) && (iterate_ == c.iterate_);
        }

        val_ptr key()
        {
            return key_;
        }

        val_ptr value() 
        {
        	return value_;
        }
        // release current key and value
        void release(); 
        bool invalid(); // reset array internal position pointer.


    private:
        
        bool getdata();

        /**
         *  Invalidate the iterator
         *  @return bool
         */
       
    };
}; // namespace zpp

#endif//htab_walk.h