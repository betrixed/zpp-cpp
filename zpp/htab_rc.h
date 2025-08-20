#ifndef HTAB_RC_H
#define HTAB_RC_H

#ifndef HTAB_PTR_H
#include "htab_ptr.h"
#endif

#ifndef HTAB_RW_H
#include "htab_rw.h"
#endif

namespace zpp {

    class htab_rw;

    class htab_rc : public htab_ptr {
    protected:
        void  own();
        void  lose();
        static bool cowop(HashTable*& inout, size_t init = HT_MIN_SIZE);

        friend class htab_ptr;
        friend class htab_rw;

    public:
        
        
        static HashTable* new_array(size_t init = HT_MIN_SIZE);
        static HashTable* empty_array();
        static void try_addref(HashTable* h);
        static bool try_decref(HashTable* h);


        static void set_global(str_ptr key, val_ptr value);
        static val_ptr  get_global(str_ptr key);
        
        ~htab_rc();
        htab_rc() : htab_ptr() {}

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
        static htab_rc getValues(htab_ptr hr);

        //* return indexed array of keys
        static htab_rc getKeys(htab_ptr hr);

        // extract subset using keylist with removal from original hfrom
        static htab_rc extract(htab_ptr exkeys, htab_rw hfrom);

        // extract subset using keylist without removal from original hfrom
        static htab_rc subset(htab_ptr exkeys, htab_ptr hfrom, bool nullmiss = false);

        // extract values list using keylist. Missing values as null
        static htab_rc sublist(htab_ptr exkeys, htab_ptr hfrom);

        
 
    };

    class htab_empty : public htab_rc {
    public:
        htab_empty();
        ~htab_empty();
    };


}; // namespace zpp

#endif//htab_rc.h