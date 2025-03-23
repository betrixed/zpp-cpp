#ifndef HTAB_WRITE_H
#define HTAB_WRITE_H

#ifndef HTAB_READ_H
#include "htab_read.h"
#endif 

namespace zpp {

    class zstr_mgr;

    class htab_write : public htab_read 
    {
    protected:
        void giveback(zval* mgr);

    public:
        // for use in child classes

        htab_write(htab_mgr& mgr);
        htab_write(zval_mgr& mgr);
        htab_write(zval_user mgr);
        htab_write(HashTable* h);
        htab_write(const zval* p);
        
        htab_write(const htab_write& w)
        {
            //zend_printf("htab_write copy&\n");
            ht_ = w.ht_;
        }



        const htab_write& operator=(const zval* p);
        
        //zstr_mgr print_all(const char* label = nullptr);

        
        void merge(HashTable* src);

        void clear();

        /** direct HashTable update methods 
        zval* update(zend_long idx, zval* val);
        zval* update(zend_string* key, zval* val);
        */

        /** direct HashTable removal methods 
        bool remove(zend_long idx);
        bool remove(zend_string* key);
        */
        /** direct HashTable index append 
        zval* append(zval* pz);
        */


        /** indirect methods which call direct methods */
        void push_back(zend_string* zs);
        void push_back(zend_object* zobj);
        void push_back(zval* zv);

        void push_back(const zval_mgr& zv) 
        {
            push_back((zval*) zv);
        }

        void push_back(zval_user zv) 
        {
            push_back((zval*) zv);
        }
        
        void push_back(HashTable* value);

        // try to disambiguent various elemental wrappers used all over the place.
        
        //void push_back(const zval_user ptr);
        //void push_back(const zstr_user bs);
        //void push_back(const zobj_user bs);

        void push_back(const zstr_intern& si)
        {
            push_back((zend_string*) si);
        }

        //void push_back(const zval_mgr& zo);
        void push_back(zstr_mgr sm)
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
        
        void set(zval* key, zval* value);
        void set(zval* key, zend_string* value);
        
        //To avoid cast to zend_string* 
        //void set(zstr_user key, const zstr_mgr& value);

        //void set(zval_user key, zval_user value);

        void set(zend_long idx, zval* value);
        void set(zend_long idx, HashTable* value);

        bool unset(zend_long idx);
        bool unset(zend_string* key);
        bool unset(zval_user key);

		/** Pull out, values given by array of
		 *  string keys, unset them here, 
         *  return them in new array 
         */
        htab_mgr extract(htab_read exkeys);
    };

};

//htab_write.h
#endif