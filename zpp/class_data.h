#ifndef CLASS_DATA_H
#define CLASS_DATA_H

namespace zpp {

class ZPP_EXPORT class_data {
    protected:
        zend_class_entry* class_entry_;

        bool check(const char* msg);
    public:

        static zobj_mgr create_object(zstr_user classname);

        static zobj_mgr std_object();
        
        class_data() : class_entry_(nullptr) {}

        class_data(zend_class_entry* ce) : class_entry_(ce) 
        {
            /* void */
        }
        
        class_data(zstr_user classname);

        bool ok() const { return (class_entry_); }

        void set(zend_class_entry* ce)
        {
            class_entry_ = ce;
        }

        bool set(zstr_user classname);

        zstr_user className();

        void add_constant(const char* name, const char* value);
        void add_constant(const char* name, zend_string* zs);
        void add_constant(const char* name, zend_long value);


        //void add_constant(const zstr_intern& name,  zval_user value, int flags = ZEND_ACC_PUBLIC);


        zval_mgr static_property(zend_string* s);
        zval_mgr static_constant(zend_string* s);


        bool new_object(zobj_mgr& om);

    };

}; // namespace
#endif