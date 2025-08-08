#ifndef HTAB_MGR_H
#define HTAB_MGR_H

#ifndef HTAB_READ_H
#include "htab_rd.h"
#endif

#ifndef HTAB_WRITE_H
#include "htab_wr.h"
#endif

namespace zpp {

    class htab_wr;

    class htab_rc : public htab_rd {
    protected:
        void  own();
        void  lose();
        static bool cowop(HashTable*& inout);

        friend class htab_rd;
        friend class htab_wr;

    public:
        
        
        static HashTable* new_array();
        static HashTable* empty_array();
        static void try_addref(HashTable* h);
        static bool try_decref(HashTable* h);


        static void set_global(str_ptr key, val_ptr value);
        static val_ptr  get_global(str_ptr key);
        
        ~htab_rc();
        htab_rc() : htab_rd() {}

        htab_rc(htab_rc&& m);
        htab_rc(val_rc&& zw);

        
        htab_rc(const val_ptr& zptr);
        

        /** passing nullptr can stop HashTable create */
        htab_rc(HashTable *h);

        htab_rc(const htab_rc& c);
        const htab_rc& operator=(const htab_rc& c);

        htab_rc(const val_rc& zw);
        const htab_rc& operator=(const val_rc& zw);
        const htab_rc& operator=(const val_ptr& zw);
        
        htab_rc& operator=(val_rc&& zw);

        htab_rc(val_ptr zptr);
        htab_rc(zval* p);

        const htab_rc& operator=(val_ptr zptr);

        
        htab_rc& operator=(htab_rc&& m);

        const htab_rc& operator=(HashTable* htab);
        const htab_rc& operator=(zval* zv);

        void  init();
        void  reset();
        void  move_zv(zval* return_value);

        bool isNull() const { return !(ht_); }
        bool isEmpty() const; 

        //! Adopt a system source hashtable with rc==1
        void adopt(HashTable* h);

        operator HashTable* () const { return (HashTable*) ht_; }

        //! Take away, nullptr internal HashTable*, to avoid decref;
        HashTable* steal();

               //* return indexed array of values
        static htab_rc getValues(htab_rd hr);

        //* return indexed array of keys
        static htab_rc getKeys(htab_rd hr);

        // extract subset using keylist with removal from original hfrom
        static htab_rc extract(htab_rd exkeys, htab_wr hfrom);

        // extract subset using keylist without removal from original hfrom
        static htab_rc subset(htab_rd exkeys, htab_rd hfrom, bool nullmiss = false);

        // extract values list using keylist. Missing values as null
        static htab_rc sublist(htab_rd exkeys, htab_rd hfrom);

        
 
    };

    class htab_empty : public htab_rc {
    public:
        htab_empty();
        ~htab_empty();
    };


}; // namespace zpp

#endif//htab_rc.h