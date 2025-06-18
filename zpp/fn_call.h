 /*  
  *  PHP extension C++ classes - zpp 
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  */

#ifndef FN_CALL_H
#define FN_CALL_H

#ifndef ZSTR_MGR_H
#include "zstr_mgr.h"
#endif

#ifndef STATE_INIT_H
#include "state_init.h"
#endif

#ifndef HTAB_READ_H
#include "htab_read.h"
#endif

#ifndef ZVAL_MGR_H
#include "zval_mgr.h"
#endif



namespace zpp {


    /**
     *  callable_fn, for PHP "Callable"
     *  Is PHP wrap for call_user_function
     */
    /**
     * Create parameter space dynamically.
     *  and copy array values starting at prefix index.
     */ 
    class args_spread  {
        size_t              argct_;
        zval*               argv_;
    public:
        //! Reserve extra number of parameters in front.
        args_spread(htab_read args, int prefixct = 0);
        ~args_spread();

        size_t arg_ct() const { return argct_; }
        zval* arg_v() const { return  (zval*) &argv_[0]; }
        
    };

    bool callable_fn(zval_mgr& result, zval_mgr& callme, int argct = 0, zval* argv = nullptr);
    bool call_spread_fn(zval_mgr& result, zval_mgr& callme, htab_read args);

    class fn_call {
    protected:
        // C-array of zvals arguments to call_user_fn
        size_t          argct_;
        zval*           argv_;
        zval_mgr        result_;
        //zstr_own        method_name_; // real owner of method name
        // PHP call cache info for multiple calls
        zend_fcall_info       fci_;
        zend_fcall_info_cache cache_;
        bool                  autowipe_;
    public:

        void throw_failed();

    	fn_call();
        ~fn_call();

        void set_fci(zend_object* obj , zstr_user method, HashTable* nargs = nullptr);
        
        void set_fname(zstr_user name);
        void set_named_args(HashTable* nargs);

        void wipe() const
        {
            memset(argv_, 0, argct_*sizeof(zval));
        }

        zval_mgr&& call_fn();


        //! This is the only means to ensure auto clean  
        //! of arg space, prior to setting parameters
        //! call only once for each function call.
        zval* argsptr() const { wipe(); return (zval*) argv_; }

        /** Direct set args and return in one call. 
         *  These must be used from correct size in template<size_t ARGCT>
         *
        zval_mgr&& call1(zval* a1);
        zval_mgr&& call2(zval* a1, zval* a2);
        zval_mgr&& call3(zval* a1, zval* a2, zval* a3);
        zval_mgr&& call4(zval* a1, zval* a2, zval* a3, zval* a4);
        zval_mgr&& call5(zval* a1, zval* a2, zval* a3, zval* a4, zval* a5);
        */


    };




    template <size_t ARGCT>
    class fn_call_args : public fn_call {
    public:
        zval  params[ARGCT];
        fn_call_args() : fn_call()
        {
            argv_ =  (zval*) &params;
            argct_ = ARGCT;
        }
    };


// prepared function call table
    
    /**
     * Awkward, since I have never before used more than 
     * one argument with file_get_contents.
     * 
     * skip resource - context(null), and use_include path(false)
     */ 
    class file_content : public fn_call_args<5> {
    public:
        zstr_mgr call(zstr_user path, 
            int offset = 0, size_t len = 0);
    };

    class fn_fopen : public fn_call_args<2> {
    public:
        zval_mgr call(zstr_user path, zstr_user modestr);
    };

    class fn_fclose : public fn_call_args<1> {
    public:
        bool call(zval_user fres);
    };

    class fn_fgetcsv : public fn_call_args<1> {
    public:
        fn_fgetcsv();
        zval_mgr call(zval_user file_res);
    };

    class fn_stripslashes : public fn_call_args<1> {
    public:
        fn_stripslashes();
        zstr_mgr call(zstr_user name);
    };

    class PathInfo : public fn_call_args<2> 
    {
    public:
        enum {
            DIRNAME = 1,
            BASENAME = 2,
            EXTENSION = 4,
            FILENAME = 8,
            ALL = DIRNAME + BASENAME + EXTENSION + FILENAME
        };

        zval_mgr call(zstr_user path, int flags);
    };

    class fnexists : public fn_call_args<1> {
    public:
        bool call(zstr_user name);
    }; 

   

    class extnloaded : public fn_call_args<1> {
    public:
        bool call(zstr_user name);
    };

    class pregquote : public fn_call_args<2> {
    public:
        zstr_mgr call(zstr_user str, zstr_user delimiter);
    };

    zstr_mgr addcslashes(zstr_user s, zstr_user escapes);
    zstr_mgr mb_detect_order(const zval_mgr& encoding);
    zstr_mgr mb_detect_encoding(zstr_user str, const zval_mgr& encodings, bool strict = false );
    zstr_mgr rawurlencode(zstr_user s);
    zstr_mgr strtr(zstr_user subj, zstr_user from, zstr_user to);
    zstr_mgr ucwords(zstr_user subj);

    zval_mgr json_decode(zstr_user str, bool asArray, int flags = 0);


    class FCall2 : public fn_call_args<2>
    {
    public:
        FCall2();

        FCall2(zstr_user func);
        zval_mgr call(zval* arg1, zval* arg2);
    };

    /** 
     *  zend_string passed to set_fname
     *  MUST be defined prior to the fci_args
     *  setup call set_fname.
     * 
     *  Cannot be in a different state_init
     *  
     */
    class fntable : public state_init {
    public:

        zstr_intern  s_function_exists;
        zstr_intern  s_file_get_contents;
        zstr_intern  s_extension_loaded;
        zstr_intern  s_preg_quote;
        zstr_intern  s_pathinfo;
        zstr_intern  s_call_user_func_array;
        zstr_intern  s_fgetcsv;
        zstr_intern  s_fopen;
        zstr_intern  s_fclose;

        extnloaded    extension_loaded;
        fnexists      function_exists;
        pregquote     preg_quote;
        file_content  file_get_contents;
        fn_fopen      fopen;
        fn_fclose      fclose;

        PathInfo      pathinfo;
        FCall2        call_user_func_array;
        fn_call       get_called_class;
        
        virtual void init();

    };

    class strtable : public state_init {
    public:
        zstr_intern  construct_key;
        zstr_intern  mb_detect_order;
        zstr_intern  mb_detect_encoding;
        zstr_intern  setdate;
        zstr_intern  settime;
        zstr_intern  diff;
        zstr_intern  date;
        zstr_intern  strtotime;
        zstr_intern  addcslashes;
        zstr_intern  rawurlencode;
        zstr_intern  strtr;
        zstr_intern  ucwords;
        zstr_intern  stripslashes;

        
        virtual void init();
    };



    

    extern fntable   FTAB;
    extern strtable  STAB;

    bool extension_loaded(zstr_user name);

    bool function_exists(zstr_user name);

    zstr_mgr preg_quote(zstr_user expr, zstr_user delimiter);

    zstr_mgr file_get_contents(zstr_user path, int offset=0, size_t len=0);

}; // end namespace zpp
#endif

