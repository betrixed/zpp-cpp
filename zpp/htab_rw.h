#ifndef HTAB_RW_H
#define HTAB_RW_H

#ifndef HTAB_PTR_H
#include "htab_ptr.h"
#endif 

#ifndef STR_RC_H
#include "str_rc.h"
#endif

namespace zpp {

    class str_intern;

    class htab_rw : public htab_ptr 
    {
    protected:
        void giveback(zval* mgr, size_t init = HT_MIN_SIZE);
        // Mark zval type flags if reference counted or not

        
    public:
        /** Each constructor may duplicate and set the HashTable* of its source,
         *  to ensure its reference count is 1.
         */

        htab_rw(htab_rc& mgr, size_t init=HT_MIN_SIZE);
        htab_rw(val_rc& mgr, size_t init=HT_MIN_SIZE);
        htab_rw(val_ptr mgr, size_t init=HT_MIN_SIZE);
        
        htab_rw(zval* p, size_t init=HT_MIN_SIZE);
        
        htab_rw(HashTable* h);

        htab_rw(const htab_rw& w)
        {
            ht_ = w.ht_;
        }
        const htab_rw& operator=(const zval* p);
        
        
        void merge(HashTable* src);

        void clear();


        /** indirect methods which call direct methods */
        void push_back(zend_string* zs);
        void push_back(zend_object* zobj);
        void push_back(zval* zv);

        void push_items(auto&&... args)
        {
            for (auto s : std::initializer_list<str_ptr>{ args... })
                push_back(s);
        }
            
        void push_back(const val_rc& zv);

        void push_back(val_ptr zv);

        void push_back(str_ptr su);
        
        void push_back(HashTable* value);

        void push_back(int value);

        void push_back(const str_intern& si)
        {
            push_back((zend_string*) si);
        }

        void push_back(str_rc sm)
        {
            push_back((zend_string*)sm);
        }

        void push_back(const char* s, std::size_t slen);

        void setnull(zend_string* key);
        
        void set(zend_string* key, zval* value);
        void set(zend_string* key, zend_string* value);
        void set(zend_string* key, HashTable* value);
        void set(zend_string* key, zend_object* obj);
        void set(zend_string* key, int value);
        void set(zend_string* key, double value);
        void setbool(zend_string* key, bool value);
        
        void set(zval* key, zval* value);
        void set(zval* key, zend_string* value);
        
        void set_null(zend_string* key);
        void set_null(zend_long idx);
        void set_null(val_ptr key);
        //To avoid cast to zend_string* 
        //void set(str_ptr key, const str_rc& value);

        //void set(val_ptr key, val_ptr value);

        void set(zend_long idx, zval* value);
        void set(zend_long idx, HashTable* value);
        void set(zend_long idx, zend_object* value);

        bool unset(zend_long idx);
        bool unset(zend_string* key);
        bool unset(val_ptr key);

		/** Pull out, values given by array of
		 *  string keys, unset them here, 
         *  return them in new array 
         */
        

        /** Delete the keys in the exkeys list */
        void removal(htab_ptr exkeys);
    };

};

//htab_rw.h
#endif