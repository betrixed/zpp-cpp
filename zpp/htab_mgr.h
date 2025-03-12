#ifndef HTAB_MGR_H
#define HTAB_MGR_H


namespace zpp {

    class htab_read;
    class htab_write;

    class htab_mgr {
    protected:
        HashTable* ht_;

        
        void  own();
        void  lose();
        static bool cowop(HashTable*& inout);

        friend class htab_read;
        friend class htab_write;

    public:
        
        
        static HashTable* new_array();
        static HashTable* empty_array();
        static void try_addref(HashTable* h);
        static bool try_decref(HashTable* h);
        
        ~htab_mgr();

        htab_mgr() : ht_(nullptr) {}

        htab_mgr(htab_mgr&& m);
        htab_mgr(zval_mgr&& zw);

        htab_mgr(const zval_user& zptr);
        

        /** passing nullptr can stop HashTable create */
        htab_mgr(HashTable *h);

        htab_mgr(const htab_mgr& c);
        const htab_mgr& operator=(const htab_mgr& c);

        htab_mgr(const zval_mgr& zw);
        const htab_mgr& operator=(const zval_mgr& zw);
        const htab_mgr& operator=(const zval_user& zw);
        
        htab_mgr& operator=(zval_mgr&& zw);

        htab_mgr(zval_user zptr);
        htab_mgr(zval* p);

        const htab_mgr& operator=(zval_user zptr);

        
        htab_mgr& operator=(htab_mgr&& m);

        const htab_mgr& operator=(HashTable* htab);
        const htab_mgr& operator=(zval* zv);

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
 
    };

    class htab_empty : public htab_mgr {
    public:
        htab_empty();
    };


}; // namespace zpp

#endif//htab_mgr.h