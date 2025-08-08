#ifndef HTAB_READ_H
#define HTAB_READ_H

#ifndef ZVAL_USER_H
#include "val_ptr.h"
#endif

#ifndef ZSTR_USER_H
#include "str_ptr.h"
#endif

typedef int fn_zval(zval*);

namespace zpp {
    
    class  val_rc;

    class  htab_rc;

    class  htab_rd {
    protected:
        HashTable* ht_;

        friend class val_rc;
        friend class htab_rc;
        
    public:
        static HashTable* make_own(HashTable* ht);

        htab_rd() : ht_(nullptr) {}

        htab_rd(const htab_rd& rc) : ht_(rc.ht_) {}
        
        htab_rd(HashTable* ht);

        operator HashTable*() const { return ht_; }

        HashTable* ptr() const { return ht_; }

        htab_rd(const val_rc& zw);

        htab_rd(val_ptr zu);
        
        htab_rd(const zval* p);

        const htab_rd& operator=(const zval* p);

        uint32_t size() const;

        bool isNull() 
        {
            return (ht_ == nullptr);
        }

        void init() {
            ht_ = nullptr;
        }
        bool ok() const { return (ht_); }

        zval* get(zend_long idx) const;
        //zval* get(val_ptr key) const;
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

        zval* operator[]  (const val_rc& key) const;

        zval* operator[]  (str_ptr skey) const;

        bool  has_index(zend_long key) const;

        bool  has_key(zend_string* skey) const;

        bool  has_key(val_ptr skey) const;
        
        bool try_fetch(zend_string* key, val_ptr&  store) const;
        bool try_fetch(zend_long key, val_ptr& store) const;

        bool try_fetch(val_ptr key, val_ptr& store) const;
        
        void return_zv(zval* return_value) const;

        str_rc print_kv(const char* label) const;

 
        htab_rc slice(int offset, int length, bool preserve_keys = false);
        /**
         * Replace string segments like @valkey with
         * the text values associated with key "valkey"
         */ 
        str_rc unhive(str_ptr subj);

        void apply_all(fn_zval fn);

    };

}; // namespace zpp
//htab_ptr.h
#endif