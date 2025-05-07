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
    class  state_init {
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

    
};

#endif
