#ifndef HTAB_MGR_H
#define HTAB_MGR_H


namespace zpp {

    class htab_read;
    class htab_write;

    class htab_mgr {
    protected:
        HashTable* ht_;

        void init();
        void own();
        void lose();
        static bool cowop(HashTable*& inout);

        friend class htab_read;
        friend class htab_write;

    public:

        static HashTable* new_array();
        static HashTable* empty_array();

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

        void  decref();

        void reset();
        void move_zv(zval* return_value);

        operator HashTable* () const { return (HashTable*) ht_; }

        //! Take away, nullptr internal HashTable*, to avoid decref;
        HashTable* steal();
 
    };

    class htab_init : public htab_mgr {
    public:
        htab_init();
    };


}; // namespace zpp

#endif//htab_mgr.h