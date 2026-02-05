 /*  
  *  @file fn_call.h
  *  @brief Declaration of fn_call class for calling PHP functions from C++
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2025 Michael Rynn
 *   @license BSD 3-Clause License
  *  
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

    /* somehow this hasn't found a use */
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

    /**
     * @class fn_call
     * Helper class to set up function calls.
     * This stores internal pointers to its members,
     * and so objects of this class must not be moved around in memory.
     */
    class fn_call {
    protected:
        // C-array of zvals arguments to call_user_fn
        //zstr_own        method_name_; // real owner of method name
        // PHP call cache info for multiple calls
        zend_fcall_info       fci_;
        zend_fcall_info_cache cache_;
        zval    result_;
    public:

        void throw_failed();

    	fn_call();
        ~fn_call();
        
        void set_fci(zend_object* obj , str_ptr method, HashTable* nargs = nullptr);
        void set_fname(str_ptr name);
        void set_obj(zend_object* obj); // for method calls
        
        void set_named_args(HashTable* nargs);

        void wipe();

        // fetching result as move operator result also clears it.
        // The result must be moved, from permanent, into request memory.
        val_rc call_fn();

        //! Since this calls wipe,
        //! must call only once for each call setup.
        //! 
        zval* argsptr() { wipe(); return fci_.params; }
    };
/**
    @class  fn_call_args
    @brief  Template class to create fn_call with fixed number of arguments.
    @tparam ARGCT Number of arguments for function call.
    @details
    This class creates a fn_call with a fixed number of arguments.
    The argument array is created as a member of the class.
    This is more efficient than creating a fn_call and then allocating
    the argument array separately.
    template
 */
    template <size_t ARGCT>
    class fn_call_args : public fn_call {
    public:
        zval  params[ARGCT];
        fn_call_args() : fn_call()
        {
            fci_.param_count = ARGCT; 
            fci_.params = (zval*) &params; 
        }
    };


// prepared function call table
    
    /**
        *  @class file_content 

        *  @brief call file_get_contents with up to 5 arguments.
         * 
         *  The PHP function file_get_contents has 5 arguments.
         *  The first is the path, the second is a boolean for
         *  whether to use the include path, the third is a resource
         *  context, the fourth is an offset, and the fifth is a length.
         * 
         *  This class provides a call method with only the path,
         *  offset and length arguments. The other two are set to
         *  false and null respectively.
         *  This is a common use case for file_get_contents.
         *  Note that the offset and length arguments are optional.
         
     * I have never before used more than 
     * one argument with file_get_contents.
     * 
     * skip resource - context(null), and use_include path(false)
     */ 
    class file_content : public fn_call_args<5> {
    public:
        str_rc call(str_ptr path, 
            int offset = 0, size_t len = 0);
    };


/**
* @class fn_fclose
* @brief call fclose with 1 argument.
* @details
* The PHP function fclose has 1 argument, the resource to close.
* @returns true on success, or false on failure.    
*/
    class fn_fclose : public fn_call_args<1> {
    public:
        bool call(val_ptr fres);
    };

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 

    class fn_filemtime : public fn_call_args<1> {
    public:
        long call(str_ptr path);
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

    class fn_define : public fn_call_args<2> {
    public:
        bool call(str_ptr constant_name, val_ptr value);
        bool call(str_ptr constant_name, str_ptr value);
    };

    class fn_defined : public fn_call_args<1> {
    public:
        bool call(str_ptr name);
    };


    typedef fn_call_args<1> fn_call_args1;
    typedef fn_call_args<2> fn_call_args2;
    typedef fn_call_args<3> fn_call_args3;
    typedef fn_call_args<4> fn_call_args4;
    /*
    class fn_simple_loader : public fn_call_args<1> {
    public:
        val_rc call(str_ptr path);
    };
    */

    /**
     * @class
        *  PathInfo
        *  @brief call pathinfo with 2 arguments.
        *  @details
        *  The PHP function pathinfo has 2 arguments.
        *  The first is the path, the second is an optional flags argument.
      */
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

        // The result must be moved, into request memory.
        val_rc call(str_ptr path, int flags = ALL);
    };

    class fn_class_exists : public fn_call_args<1> {
    public:
        bool call(str_ptr name);
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
        str_intern  s_function_exists;
        str_intern  s_class_exists;
        str_intern  s_file_get_contents;
        str_intern  s_extension_loaded;
        str_intern  s_preg_quote;
        str_intern  s_pathinfo;
        str_intern  s_call_user_func_array;
        str_intern  s_fgetcsv;
        str_intern  s_fopen;


        str_intern  s_fclose;
        str_intern  s_constant;
        str_intern  s_dirname;
        str_intern  s_defined;
        str_intern  s_define;
        str_intern  s_getcwd;
        str_intern  s_weakref_create;
        str_intern  s_weakref_get;

        str_intern  s_isdir;
        str_intern  s_readdir;
        str_intern  s_opendir;
        str_intern  s_closedir;
        str_intern  s_mkdir;
        str_intern  s_php_sapi_name;
        str_intern  s_filemtime;
        str_intern  s_realpath;
        str_intern  s_glob;
        str_intern  s_unlink;

        str_intern  s_fwrite;
        str_intern  s_fread;

        str_intern  s_fgets;

        str_intern  s_serialize;
        str_intern  s_unserialize;
        str_intern  s_sha1;
        str_intern  s_isreadable;
        
        void init() override;
        void init_req() override;
    };

    class strtable : public state_init {
    public:
        str_intern  construct_key;
        str_intern  mb_detect_order;
        str_intern  mb_detect_encoding;
        str_intern  setdate;
        str_intern  settime;
        str_intern  diff;
        str_intern  date;
        str_intern  strtotime;
        str_intern  addcslashes;
        str_intern  rawurlencode;
        str_intern  strtr;
        str_intern  ucwords;
        str_intern  stripslashes;
        str_intern  array_pop;
        str_intern  array_splice;

        
        void init() override;
    };


    class file_res {
    public:
        val_rc    file_;//PHP file resource object
        file_res(str_ptr path, str_ptr mode, 
                    bool use_inc_path=false, val_ptr ctx = val_ptr());
        ~file_res();
        void close();
        bool isopen();

        operator val_ptr () { return (zval*) file_; }
    };
    

    extern fntable   FTAB;
    extern strtable  STAB;

/** 
 * Various functions that call the PHP function with the same name
 */
    str_rc addcslashes(str_ptr s, str_ptr escapes);
    str_rc mb_detect_order(const val_rc& encoding);
    str_rc mb_detect_encoding(str_ptr str, const val_rc& encodings, bool strict = false );
    str_rc rawurlencode(str_ptr s);
    str_rc strtr(str_ptr subj, str_ptr from, str_ptr to);
    str_rc ucwords(str_ptr subj);

    val_rc json_decode(str_ptr str, bool asArray, int flags = 0);

    int64_t strtotime(str_ptr datetime, int64_t base_timestamp = -1);

    // calls to direct function implementations
    bool file_exists(str_ptr fname);

    val_rc array_pop(val_rc& arrayref);


    // wrappers for TLfnTable calls
    bool extension_loaded(str_ptr name);

    bool function_exists(str_ptr name);

    bool class_exists(str_ptr name);

    val_rc fopen(str_ptr name, str_ptr fmode, 
                bool use_include_path=false, val_ptr context=val_ptr());

    str_rc fgets(val_ptr fres, zend_long limit = -1);

    bool fclose(val_ptr fres);

    val_rc fwrite(val_ptr fres, str_ptr data, zend_long length = -1);

    str_rc fread(val_ptr fres, int length=-1);

    str_rc preg_quote(str_ptr expr, str_ptr delimiter);

    str_rc file_get_contents(str_ptr path, int offset=0, size_t len=0);

    // the "constant" is renamed as get_constant
    val_rc get_constant(str_ptr name);

    bool is_dir(str_ptr path);

    str_rc dirname(str_ptr path, int level=1);

    bool defined(str_ptr name);

    bool define(str_ptr name, val_ptr value);

    val_rc pathinfo(str_ptr path, int flags = PathInfo::ALL);

    val_rc call_user_func_array(zval* arg1, zval* arg2);

    str_rc php_sapi_name();

    long filemtime(str_ptr path);

    bool is_dir(str_ptr path);

    bool is_file(str_ptr path);

    bool is_readable(str_ptr path);

    bool unlink(str_ptr path, val_ptr context = val_ptr());

    str_rc getcwd();

    val_rc opendir(str_ptr path);

    val_rc readdir(val_ptr dh);

    void closedir(val_ptr dh);

    str_rc realpath(str_ptr path);

    htab_rc glob(str_ptr wcard, int flags = 0);

    bool mkdir(str_ptr path, int permissions = 0755, bool recurse = false);

    obj_rc weakref_create(obj_ptr obj);

    obj_rc weakref_get(obj_ptr wref);

    htab_rc array_splice(htab_rc& input, int offset, int length = 0, htab_ptr replace = htab_ptr());

    str_rc serialize(val_ptr value);

    val_rc unserialize(str_ptr data, htab_ptr options = htab_ptr());

    str_rc sha1(str_ptr value, bool binary = false);


}; // end namespace zpp
#endif

