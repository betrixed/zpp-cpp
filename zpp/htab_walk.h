#ifndef HTAB_WALK_H
#define HTAB_WALK_H

#ifndef ZVAL_USER_H
#include "val_ptr.h"
#endif

#ifndef HTAB_READ_H
#include "htab_rd.h"
#endif

#ifndef ZVAL_MGR_H
#include "val_rc.h"
#endif

namespace zpp {

    class htab_walk {
    private:
        val_rc key_;
        val_rc value_;

    	htab_rd  wrap_;

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