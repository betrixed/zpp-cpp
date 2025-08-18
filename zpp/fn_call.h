 /*  
  *  PHP extension C++ classes - zpp 
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  */

#ifndef FN_CALL_H
#define FN_CALL_H

#ifndef STR_RC_H
#include "str_rc.h"
#endif

#ifndef STATE_INIT_H
#include "state_init.h"
#endif

#ifndef HTAB_PTR_H
#include "htab_ptr.h"
#endif

#ifndef VAL_RC_H
#include "val_rc.h"
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
        args_spread(htab_ptr args, int prefixct = 0);
        ~args_spread();

        size_t arg_ct() const { return argct_; }
        zval* arg_v() const { return  (zval*) &argv_[0]; }
        
    };

    bool callable_fn(val_rc& result, val_rc& callme, int argct = 0, zval* argv = nullptr);
    bool call_spread_fn(val_rc& result, val_rc& callme, htab_ptr args);

    class fn_call {
    protected:
        // C-array of zvals arguments to call_user_fn
        size_t          argct_;
        zval*           argv_;
        val_rc        result_;
        //zstr_own        method_name_; // real owner of method name
        // PHP call cache info for multiple calls
        zend_fcall_info       fci_;
        zend_fcall_info_cache cache_;
        bool                  autowipe_;
    public:

        void throw_failed();

    	fn_call();
        ~fn_call();

        void set_fci(zend_object* obj , str_ptr method, HashTable* nargs = nullptr);
        
        void set_fname(str_ptr name);
        void set_named_args(HashTable* nargs);

        void wipe() const
        {
            memset(argv_, 0, argct_*sizeof(zval));
        }

        val_rc&& call_fn();


        //! This is the only means to ensure auto clean  
        //! of arg space, prior to setting parameters
        //! call only once for each function call.
        zval* argsptr() const { wipe(); return (zval*) argv_; }



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
        str_rc call(str_ptr path, 
            int offset = 0, size_t len = 0);
    };

    class fn_fopen : public fn_call_args<2> {
    public:
        val_rc call(str_ptr path, str_ptr modestr);
    };

    class fn_fclose : public fn_call_args<1> {
    public:
        bool call(val_ptr fres);
    };

    class fn_fgetcsv : public fn_call_args<1> {
    public:
        fn_fgetcsv();
        val_rc call(val_ptr file_res);
    };

    class fn_stripslashes : public fn_call_args<1> {
    public:
        fn_stripslashes();
        str_rc call(str_ptr name);
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

        val_rc call(str_ptr path, int flags);
    };

    class fnexists : public fn_call_args<1> {
    public:
        bool call(str_ptr name);
    }; 

   

    class extnloaded : public fn_call_args<1> {
    public:
        bool call(str_ptr name);
    };

    class pregquote : public fn_call_args<2> {
    public:
        str_rc call(str_ptr str, str_ptr delimiter);
    };

    str_rc addcslashes(str_ptr s, str_ptr escapes);
    str_rc mb_detect_order(const val_rc& encoding);
    str_rc mb_detect_encoding(str_ptr str, const val_rc& encodings, bool strict = false );
    str_rc rawurlencode(str_ptr s);
    str_rc strtr(str_ptr subj, str_ptr from, str_ptr to);
    str_rc ucwords(str_ptr subj);

    val_rc json_decode(str_ptr str, bool asArray, int flags = 0);


    class FCall2 : public fn_call_args<2>
    {
    public:
        FCall2();

        FCall2(str_ptr func);
        val_rc call(zval* arg1, zval* arg2);
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

    bool extension_loaded(str_ptr name);

    bool function_exists(str_ptr name);

    str_rc preg_quote(str_ptr expr, str_ptr delimiter);

    str_rc file_get_contents(str_ptr path, int offset=0, size_t len=0);

}; // end namespace zpp
#endif

