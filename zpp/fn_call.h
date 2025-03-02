 /*  
  *  PHP extension C++ classes - zpp 
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  */

#ifndef FN_CALL_H
#define FN_CALL_H

//fn_call.h
/* #ifndef SHOW_ZPP_H
#include "show_zpp.h"
#endif
*/

#ifndef STATE_INIT_H
#include "state_init.h"
#endif

namespace zpp {

    /**
     *  callable_fn, for PHP "Callable"
     *  Is PHP wrap for call_user_function
     */

    class ZPP_EXPORT args_spread  {
        size_t              argct_;
        zval_init*          argv_;
    public:
        args_spread(htab_read args);
        ~args_spread();

        size_t arg_ct() const { return argct_; }
        zval* arg_v() const { return  (zval*) &argv_[0]; }
        
    };

    bool callable_fn(zval_mgr& result, zval_mgr& callme, int argct = 0, zval* argv = nullptr);
    bool call_spread_fn(zval_mgr& result, zval_mgr& callme, htab_read args);

    class ZPP_EXPORT fn_call {
    protected:
        // C-array of zvals arguments to call_user_fn
        size_t          argct_;
        zval_init*      argv_;
        zval_mgr        result_;
        //zstr_own        method_name_; // real owner of method name
        // PHP call cache info for multiple calls
        zend_fcall_info       fci_;
        zend_fcall_info_cache cache_;
        bool              track_;
        //size_t          call_ct_;
    public:

        void throw_failed();

    	fn_call();
        ~fn_call();

        void set_track(bool value) { track_ = value; }
        void set_fci(zend_object* obj , zstr_user method, HashTable* nargs = nullptr);
        //void set_fname(const char* name);
        void set_fname(zstr_user name);

        // 1-based index for arguments, not zero

        void set_arg(size_t ix, zval* vp);

        zval_mgr&& call_fn();
        
        /** Direct set args and return in one call. 
         *  These must be used from correct size in template<size_t ARGCT>
         */
        zval_mgr&& call1(zval* a1);
        zval_mgr&& call2(zval* a1, zval* a2);
        zval_mgr&& call3(zval* a1, zval* a2, zval* a3);
        zval_mgr&& call4(zval* a1, zval* a2, zval* a3, zval* a4);
        zval_mgr&& call5(zval* a1, zval* a2, zval* a3, zval* a4, zval* a5);
    };




    template <size_t ARGCT>
    class fn_call_args : public fn_call {
    protected:
        zval_init targv_[ARGCT];
    public:
        fn_call_args() : fn_call()
        {
            argv_ = &targv_[0];
            argct_ = ARGCT;
        }
        zval* argptr(size_t ix) { return argv_[ix]; }
    };

// prepared function call table
    
    /**
     * Awkward, since I have never used more than 
     * one argument for file_get_contents.
     * 
     * skip resource - context(null), and use_include path(false)
     */ 
    class file_content : public fn_call_args<5> {
    public:
        zstr_mgr call(zstr_user path, 
            int offset = 0, size_t len = 0);
    };

    class extnloaded : public fn_call_args<1> {
    public:
        bool call(zstr_user name);
    };

    class fnexists : public fn_call_args<1> {
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

        extnloaded    extension_loaded;
        fnexists      function_exists;
        pregquote     preg_quote;
        file_content  file_get_contents;

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

