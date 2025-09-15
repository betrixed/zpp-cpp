#ifndef CLASS_DATA_H
#define CLASS_DATA_H

/**
 * @file zpp/class_data.h
 * @brief Handle operations for a zend_class_entry*.
 * @author Michael Rynn <michael.rynn.500@gmail.com>
 * copyright (c) 2025 Michael Rynn
 * Distributed under the Artistic License 2.0
 */ 

#ifndef PHP_EXTERN_H
#include "php_extern.h"
#endif

#ifndef OBJ_RC_H
#include "obj_rc.h"
#endif

namespace zpp {

class class_data {
    protected:
        zend_class_entry* class_entry_;

        bool check(const char* msg);
    public:

        static zend_class_entry* get_class(str_ptr classname);

        static obj_rc create_object(str_ptr classname);

        static obj_rc std_object();
        
        class_data() : class_entry_(nullptr) {}

        class_data(zend_class_entry* ce) : class_entry_(ce) 
        {
            /* void */
        }
        
        class_data(str_ptr classname);

        bool ok() const { return (class_entry_); }

        void set(zend_class_entry* ce)
        {
            class_entry_ = ce;
        }

        bool set(str_ptr classname);

        str_ptr className();

        void add_constant(const char* name, const char* value);
        void add_constant(const char* name, zend_string* zs);
        void add_constant(const char* name, zend_long value);


        // read
        val_rc static_property(zend_string* s);
        
        // write
        void static_property(zend_string* pname, zval* value);

        val_rc constant_value(zend_string* s);
  

        bool new_object(obj_rc& om);

    };

}; // namespace
#endif