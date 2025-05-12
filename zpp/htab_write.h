#ifndef HTAB_WRITE_H
#define HTAB_WRITE_H

#ifndef HTAB_READ_H
#include "htab_read.h"
#endif 

#ifndef ZSTR_MGR_H
#include "zstr_mgr.h"
#endif

namespace zpp {

    class zstr_intern;

    class htab_write : public htab_read 
    {
    protected:
        void giveback(zval* mgr);
        // Mark zval type flags if reference counted or not

        
    public:
        /** Each constructor may duplicate and set the HashTable* of its source,
         *  to ensure its reference count is 1.
         */

        htab_write(htab_mgr& mgr);
        htab_write(zval_mgr& mgr);
        htab_write(zval_user mgr);
        htab_write(HashTable* h);
        htab_write(const zval* p);
        
        htab_write(const htab_write& w)
        {
            ht_ = w.ht_;
        }



        const htab_write& operator=(const zval* p);
        
        
        void merge(HashTable* src);

        void clear();


        /** indirect methods which call direct methods */
        void push_back(zend_string* zs);
        void push_back(zend_object* zobj);
        void push_back(zval* zv);

        void push_back(const zval_mgr& zv);

        void push_back(zval_user zv);

        void push_back(zstr_user su);
        
        void push_back(HashTable* value);

        void push_back(const zstr_intern& si)
        {
            push_back((zend_string*) si);
        }

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
        
        void set_null(zend_string* key);
        void set_null(zend_long idx);
        void set_null(zval_user key);
        //To avoid cast to zend_string* 
        //void set(zstr_user key, const zstr_mgr& value);

        //void set(zval_user key, zval_user value);

        void set(zend_long idx, zval* value);
        void set(zend_long idx, HashTable* value);
        void set(zend_long idx, zend_object* value);

        bool unset(zend_long idx);
        bool unset(zend_string* key);
        bool unset(zval_user key);

		/** Pull out, values given by array of
		 *  string keys, unset them here, 
         *  return them in new array 
         */
        

        /** Delete the keys in the exkeys list */
        void removal(htab_read exkeys);
    };

};

//htab_write.h
#endif