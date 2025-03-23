#ifndef HTAB_READ_H
#define HTAB_READ_H

#ifndef ZVAL_USER_H
#include "zval_user.h"
#endif

#ifndef HTAB_MGR_H
#include "htab_mgr.h"
#endif

typedef int fn_zval(zval*);

namespace zpp {
    
    class  zval_mgr;

 
    class  htab_read {
    protected:
        HashTable* ht_;

        friend class htab_mgr;
        friend class htab_write;
        friend class zval_mgr;
    public:
        static HashTable* make_own(HashTable* ht);

        htab_read() : ht_(nullptr) {}

        htab_read(const htab_read& rc) : ht_(rc.ht_) {}

        htab_read(const htab_mgr& rc) : ht_(rc.ht_) {}
        
        htab_read(HashTable* ht);

        operator HashTable*() const { return ht_; }

        HashTable* ptr() const { return ht_; }

        htab_read(const zval_mgr& zw);

        htab_read(zval_user zu);
        
        htab_read(const zval* p);

        const htab_read& operator=(const zval* p);

        uint32_t size() const;

        bool isNull() 
        {
            return (ht_ == nullptr);
        }
        bool ok() const { return (ht_); }

        zval* get(zend_long idx) const;
        //zval* get(zval_user key) const;
        zval* get(zend_string* zkey) const;

        zval* get(const char* key) const;
        zval* get(const std::string_view& key) const;
        zval* get(zval* key) const;

        zval* operator[](zend_long idx) const { return get(idx); }
        zval* operator[](zend_string* zkey) const { return get(zkey); }

        zval* operator[]  (const std::string_view &zkey) const
        {
                //zend_printf("[string_view&] %s\n", zkey.data());
                return get(zkey);
        }

        zval* operator[]  (const zval_mgr& key) const
        {
                //zend_printf("[zval_own&]\n");
                return get((zval*)key);
        }

        zval* operator[]  (zstr_user skey) const
        {
                //zend_printf("[zstr_ptr&]\n");
                return get( (zend_string*) skey);
        }
        bool  has_index(zend_long key) const;

        bool  has_key(zend_string* skey) const;

        bool  has_key(zval_user skey) const;
        
        bool try_fetch(zend_string* key, zval_user&  store) const;
        bool try_fetch(zend_long key, zval_user& store) const;

        bool try_fetch(zval_user key, zval_user& store) const;
        
        void return_zv(zval* return_value) const;

        zstr_mgr print_kv(const char* label) const;

        //* return indexed array of values
        htab_mgr getValues();

        //* return indexed array of keys
        htab_mgr getKeys();

        /**
         * Replace string segments like @valkey with
         * the text values associated with key "valkey"
         */ 
        zstr_mgr unhive(zstr_user subj);

        void apply_all(fn_zval fn);

    };

}; // namespace zpp
//htab_ptr.h
#endif