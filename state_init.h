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
    class  state_init {
    protected:
        static state_init* first_;
        static state_init* last_;

        state_init* next_;
    public:
        virtual ~state_init();

        state_init();
        
        
        // At module start/end (Call in one extension only)
        static void init_all();
        static void end_all();

        // At request start/end  (Call in one extension only)
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

    
};

#endif
