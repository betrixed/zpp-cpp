#ifndef HTAB_PTR_H
#define HTAB_PTR_H

/**
 * @file zpp/htab_ptr.h
 * @author Michael Rynn <michael.rynn.500@gmail.com>
 * @brief htab_ptr - non-owning pointer to HashTable
 * @copyright Copyright (c) 2025
 * @license BSD 3-Clause License
 */

#ifndef STR_PTR_H
#include "str_ptr.h"
#endif

typedef int fn_zval(zval*);

namespace zpp {
    
    class  val_rc;
    class  val_ptr;
    class  htab_rc;

    /** 
     * @class htab_ptr
     * @brief Non-owning pointer to a HashTable 
     * This class does not do any reference counting
     * It is used to pass around HashTables without
     * transferring ownership. No write methods are provided.
     * Use htab_rw for that.
     */
    class  htab_ptr {
    protected:
        HashTable* ht_;

        friend class val_rc;
        friend class htab_rc;
        
    public:
        static HashTable* make_own(HashTable* ht);


        static void try_addref(HashTable *ht)
        {
            if (!ht || (GC_FLAGS(ht) & GC_IMMUTABLE))
            {
                return;
            }
            GC_ADDREF(ht);
        }


        static void array_bind(zval* zt, HashTable* ht)
        {
            if (ht)
            {
                Z_ARR_P(zt)=ht;
                Z_TYPE_INFO_P(zt) = (GC_FLAGS(ht) & GC_IMMUTABLE) ? IS_ARRAY : IS_ARRAY_EX;       
            }
            else
            {   
                ZVAL_NULL(zt);
            }
        }
        
        static void try_decref(HashTable* ht)
        {
            if (!ht || (GC_FLAGS(ht) & GC_IMMUTABLE))
            {
                return;
            }
            auto& rct =  ht->gc.refcount;
            if (rct==1) {
                zend_array_destroy(ht);
                return;
            }
            rct--;
        }

        htab_ptr() : ht_(nullptr) {}

        htab_ptr(const htab_ptr& rc) : ht_(rc.ht_) {}
        
        htab_ptr(HashTable* ht);

        operator HashTable*() const { return ht_; }

        HashTable* ptr() const { return ht_; }

        htab_ptr(const val_rc& zw);
        
        htab_ptr(const zval* p);

        const htab_ptr& operator=(const zval* p);

        uint32_t size() const;

        bool isPacked() const {
            return (ht_ && (HT_IS_PACKED(ht_)));
        }

        bool isNull() 
        {
            return (ht_ == nullptr);
        }

        void init() {
            ht_ = nullptr;
        }
        bool ok() const { return (ht_); }

        zval* get(zend_long idx) const;
        
        zval* get(zend_string* zkey) const;

        zval* get(zend_string* zkey, val_ptr adef) const;

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

        bool  has_key(zval* skey) const;

        //! array value search for packed array; -1 for not found
        int   value_index(str_ptr fvalue) const;

        //! All keys must be integer from 0 and in sequence
        bool is_list() const;
        
        bool try_fetch(zend_string* key, val_ptr&  store) const;
        bool try_fetch(zend_long key, val_ptr& store) const;

        bool try_fetch(zval* key, val_ptr& store) const;
        
        void copy_zv(zval* return_value) const;

        str_rc print_kv(const char* label) const;

 
        htab_rc slice(int offset, int length, bool preserve_keys = false);
        /**
         * Replace string segments like @valkey with
         * the text values associated with key "valkey"
         */ 
        str_rc unhive(str_ptr subj);

        void apply_all(fn_zval fn);

        static HashTable* empty_array();

        static zval* get_global(str_ptr key);

        static void set_global(str_ptr key, zval* value);

        static htab_ptr globals();

    };

}; // namespace zpp
//htab_ptr.h
#endif