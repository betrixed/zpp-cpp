#ifndef HTAB_USER_H
#define HTAB_USER_H

#ifndef ZVAL_USER_H
#include "zval_user.h"
#endif

#ifndef HTAB_MGR_H
#include "htab_mgr.h"
#endif

typedef int fn_zval(zval*);

namespace zpp {
    
    class  zval_mgr;

 
    class  htab_user {
    protected:
        HashTable* ht_;

        friend class htab_mgr;
        friend class zval_mgr;

        static HashTable* make_own(HashTable* h);
    public:
        // for use in child classes

        enum {
           COW_VIOLATE = (1<<6) // temporary set to write to arrays with gc > 1
        };

/** Constructors 
 */
        htab_user() : ht_(nullptr) {}

        htab_user(const htab_user& rc) : ht_(rc.ht_) {}

        htab_user(HashTable* ht);

        operator HashTable*() const { return ht_; }

        HashTable* ptr() const { return ht_; }

        htab_user(const zval_mgr& zw);

        htab_user(zval_user zu);
        
        htab_user(const zval* p);

        bool isNull() {
                return (ht_ == nullptr);
        }

        const htab_user& operator=(const zval* p);

        void cow_violate(bool on=true);
        
        void apply_all(fn_zval fn);

        //zstr_mgr print_all(const char* label = nullptr);

        uint32_t size() const;

        bool  has_index(zend_long key) const;

        bool  has_key(zend_string* skey) const
        {
                return (get(skey) != nullptr);
        }

        bool  has_key(zval_user skey) const;

        void clear();

        /** direct HashTable update methods */
        void update(zend_long idx, zval* val);
        void update(zend_string* key, zval* val);

        /** direct HashTable removal methods */
        bool remove(zend_long idx);
        bool remove(zend_string* key);

        /** direct HashTable index append */
        void append(zval* pz);



        /** indirect methods which call direct methods */
        void push_back(zend_string* zs);
        void push_back(zend_object* zobj);

        void push_back(zval* zv);

        void push_back(HashTable* value);

        // try to disambiguent various elemental wrappers used all over the place.
        
        void push_back(const zval_user ptr);
        void push_back(const zstr_user bs);
        void push_back(const zobj_user bs);

        void push_back(const zval_mgr& zo);

        void push_back(const char* s, std::size_t slen);

        void set(zend_string* key, zval* value);
        void set(zend_string* key, zend_string* value);

        void set(zend_string* key, int value);
        void set(zend_string* key, double value);
        
        void set(zval_user key, zval_user value);

        void set(zend_string* key, HashTable* value);
        void set(zend_string* key, zend_object* obj);

        void set(zend_long idx, HashTable* value);

        void set(zend_long idx, zval* val)
        {
            update(idx, val);
        }
        
        bool unset(zend_long idx);
        bool unset(zend_string* key);
        bool unset(zval_user key);

        zval* get(zend_long idx) const;
        zval* get(zval_user key) const;
        zval* get(zend_string* zkey) const ;

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

        
        bool try_fetch(zend_string* key, zval_user&  store) const;
        bool try_fetch(zend_long key, zval_user& store) const;

        bool try_fetch(zval_user key, zval_user& store) const;
        
        void return_zv(zval* return_value) const;

        /** Pull out, values by string key, unset them, 
         * return them in new array 
         */
        htab_mgr extract(htab_user exkeys);

        //* return indexed array of values
        htab_mgr getValues();

        //* return indexed array of keys
        htab_mgr getKeys();

        void merge(HashTable* src);
        /**
         * Replace string segments like @valkey with
         * the text values associated with key "valkey"
         */ 
        zstr_mgr unhive(zstr_user subj);


    };

}; // namespace zpp
//htab_ptr.h
#endif