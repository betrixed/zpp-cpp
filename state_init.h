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
    * Module initialisation will call static init_all() for runtime 
    * initialize, in unknown link, prior to calling functions for class registration.
    */

    class state_init;

    // uniquely provided by extension module!
    class  state_list {
    public:
        state_init* first_;
        state_init* last_;
        const char* name_;

        state_list(const char* name);
        void  add_si(state_init *item);

        void  call_mod_init();
        void  call_mod_end();

        void  call_req_init();
        void  call_req_end();
    };


    class  state_init {
    protected:
        
        state_init* next_;
        bool        registered_;

        friend class state_list;
        
    public:
        // just initialise
        state_init();
        virtual ~state_init();


        
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


};

#endif
