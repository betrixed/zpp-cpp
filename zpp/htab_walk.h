#ifndef HTAB_WALK_H
#define HTAB_WALK_H

#ifndef ZVAL_USER_H
#include "zval_user.h"
#endif

#ifndef HTAB_READ_H
#include "htab_read.h"
#endif

namespace zpp {

    class htab_walk {
        typedef std::pair<zval_mgr,zval_mgr> rzpair;

        rzpair data_;

    	htab_read  wrap_;

        HashPosition iterate_ = 0;

        bool ok_ = false;
    public:

        htab_walk(): iterate_(0), ok_(false) 
        {  /* void */ }
        
        htab_walk(const htab_walk &c)
            : wrap_(c.wrap_), iterate_(c.iterate_)
        { /* void */ }

        ~htab_walk() = default;


        //const htab_walk& operator=(int pos);

         bool ok() const 
         {
            return ok_;
         }
         

        htab_walk *clone()
        {
            // create a new instance
            return new htab_walk(*this);
        }

        bool start(HashTable* ht);

        bool next();

        bool prev();

        bool operator=(const htab_walk& c) const
        {
            return (wrap_ == c.wrap_) && (iterate_ == c.iterate_);
        }

        zval_user key(){
        	return zval_user(data_.first);
        }

        zval_user value() {
        	return zval_user(data_.second);
        }


        const rzpair& data() const
        {
            return data_;
        }

    private:
        
        bool getdata();

        /**
         *  Invalidate the iterator
         *  @return bool
         */
        bool invalid();
    };
}; // namespace zpp

#endif//htab_walk.h