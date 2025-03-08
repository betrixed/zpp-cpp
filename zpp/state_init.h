 /*  
  *  PHP extension C++ classes - zpp 
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  */

#ifndef STATE_INIT_H
#define STATE_INIT_H

namespace zpp {
    /**
    * A strategy for php module initialise.
    * C++ default initializes them in ANY ORDER 
    * (compiler undefined),
    * Later call tp static init_all() for runtime 
    * initialize, is in random order of being linked.
    */
    class ZPP_EXPORT state_init {
    protected:
        static state_init* first_;
        static state_init* last_;


        state_init* next_;
    public:
        virtual ~state_init();

        state_init();
        
        // iterate links for module start/end
        static void init_all();
        static void end_all();

        // iterate links for request start/end
        static void  init_request();
        static void  end_request();

        // module init/end calls
        virtual void init();
        virtual void end();

        // request init/end calls
        virtual void init_req();
        virtual void end_req();

        // iterate links for request start/end
    };

    /** 
     * functions to use a zend_class_entry*
     */

    class  ZPP_EXPORT class_data {
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

        zend_string* className()
        {
            if (class_entry_)
            {
                return class_entry_->name;
            }
            return zend_empty_string;
        }

        void add_constant(const char* name, const char* value);
        void add_constant(const char* name, zend_string* zs);
        void add_constant(const char* name, zend_long value);


        //void add_constant(const zstr_intern& name,  zval_user value, int flags = ZEND_ACC_PUBLIC);


        zval_mgr static_property(zend_string* s);
        zval_mgr static_constant(zend_string* s);


        bool new_object(zobj_mgr& om);

    };
};

#endif
