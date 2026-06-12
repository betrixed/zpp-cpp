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

#ifndef FLOCK_COMPAT_H
extern "C" {
    #include <ext/standard/flock_compat.h>
}
#endif

#ifndef FILE_H
extern "C" {
    #include <ext/standard/file.h>
}
#endif

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

#include <cstring>

namespace zpp {

    enum PHP_FILE {
        APPEND = PHP_FILE_APPEND,
        EXCL_LOCK = PHP_LOCK_EX
    };

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

    bool call_spread_fn(val_rc& result, val_rc& callme, htab_ptr args);
    bool callable_fn(val_rc& result, val_rc& callme, int argct = 0, zval* argv = nullptr);

    /**
     * @class fn_call
     * Helper class to set up function calls.
     * This stores internal pointers to its members,
     * and so objects of this class must not be moved around in memory.
     */
    class fn_call {
    protected:
        // call arguments
        zend_fcall_info       fci_;

        // PHP call cache info for repeated calls
        zend_fcall_info_cache cache_;

        void init_f();
        
        friend class fn_result;
    public:

    	fn_call();
        fn_call(zend_string* method);
        fn_call(zend_string* method, zend_object* obj);

        ~fn_call();
        
        void set_fci(zend_string* method, zend_object* obj = nullptr);
        void set_obj(zend_object* obj); // for method calls
    
        void debug_dump();
    };
/* The fn_result class , has space for a function result, 
   and is initialised with a reference to a fn_call 
*/

    class fn_result {
    protected:
        fn_call&  cfi_;

        fn_result(fn_call& fn) : cfi_(fn), result_({0})
        {
            cfi_.fci_.retval = &result_;

        }
        fn_result(fn_call& fn, zend_object* obj) : cfi_(fn), result_({0})
        {
            cfi_.fci_.retval = &result_;
            cfi_.set_obj(obj);
        }


    public:                  
        zval      result_;

        void named_args(HashTable* ht)
        {
            cfi_.fci_.named_params = ht;
        }

        void set_obj(zend_object* c)
        {
            cfi_.set_obj(c);
        }
        void setParams(zval* p, size_t ct, HashTable* nargs = nullptr)
        {
            auto& ci = cfi_.fci_;

            ci.param_count = ct; 
            ci.params = p; 
            ci.named_params = nargs;
            if (ct) {
                 std::memset(p, 0, ct*sizeof(zval));
            }
        }

        bool    call_fn();

        void    throw_failed();
        
        /** Call for various return types */
        val_rc  mixed();
        str_rc  str();
        obj_rc  obj();
        bool    zbool();
        htab_rc array();
        zend_long zlong();

    };
    /*
    Actual calls should be done from these classes, 
    according to the number of parameters to be passed,
    from none, to ARGCT
    */
    class fn_noparams : public fn_result {
    public:
        fn_noparams(fn_call& fn) : fn_result(fn) {
             setParams(nullptr, 0);
        }

        fn_noparams(fn_call& fn, HashTable* nargs) : fn_result(fn)
        {
            setParams(nullptr, 0, nargs);
        }

    };

    template <size_t ARGCT>
    class fn_params : public fn_result {
    public:
        zval      params[ARGCT];
        
        fn_params(fn_call& fn) : fn_result(fn)
        {
            setParams(&params[0], ARGCT);
        }
        fn_params(fn_call& fn, HashTable* nargs) : fn_result(fn)
        {
            setParams(&params[0], ARGCT, nargs);
        }



        zval* argsptr() { return params; /*&params[0];*/ }
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
         *  This function has only the path,
         *  offset and length arguments. 
         *  The other two file_get_contents args are set to
         *  false and null respectively.  
     * 
     *      Skips resource - context(null), and use_include path(false)
     */ 

    

/**
* @class fn_fclose
* @brief call fclose with 1 argument.
* @details
* The PHP function fclose has 1 argument, the resource to close.
* @returns true on success, or false on failure.    
*/

    enum  PathInfo 
    {
        DIRNAME = 1,
        BASENAME = 2,
        EXTENSION = 4,
        FILENAME = 8,
        ALL = DIRNAME + BASENAME + EXTENSION + FILENAME
    };

    val_rc pathinfo(str_ptr path, PathInfo flags = PathInfo::ALL);


    void register_fn_calls();

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

        str_intern  s_addcslashes;
        str_intern  s_array_pop;
        str_intern  s_array_splice;
        str_intern  s_call_user_func_array;
        str_intern  s_class_exists;
        
        str_intern  s_closedir;
        str_intern  s_constant;
        str_intern  s_define;
        str_intern  s_defined;
        str_intern  s_dirname;
        str_intern  s_basename;

        str_intern  s_extension_loaded;
        str_intern  s_fclose;
        str_intern  s_fgetcsv;
        str_intern  s_fgets;
        str_intern  s_file_get_contents;
        str_intern  s_file_put_contents;
        
        str_intern  s_filemtime;
        str_intern  s_fopen;
        str_intern  s_fread;
        str_intern  s_function_exists;
        str_intern  s_fwrite;
        
        str_intern  s_getcwd;
        str_intern  s_glob;
        str_intern  s_isdir;
        str_intern  s_isfile;
        str_intern  s_islink;
        str_intern  s_isreadable;

        str_intern  s_mb_detect_encoding;
        str_intern  s_mb_detect_order;
        str_intern  s_mkdir;
        str_intern  s_opendir;
        str_intern  s_pathinfo;

        str_intern  s_php_sapi_name;
        str_intern  s_preg_quote;
        str_intern  s_rawurlencode;
        str_intern  s_readdir;
        str_intern  s_realpath;

        str_intern  s_serialize;
        str_intern  s_copy;
        str_intern  s_rmdir;

        str_intern  s_sha1;
        str_intern  s_stripslashes;
        str_intern  s_strtr;
        str_intern  s_ucwords;

        str_intern  s_unlink;
        str_intern  s_unserialize;
        str_intern  s_weakref_create;
        str_intern  s_weakref_get;
        
        
        str_intern  session_write_close_fn;
        str_intern  session_status_fn;
        str_intern  session_id_fn;
        str_intern  session_name_fn;
        str_intern  session_regenerate_id_fn;
        str_intern  session_start_fn;
        str_intern  session_set_save_handler_fn;
        str_intern  session_save_path_fn;

        str_intern  headers_sent_fn;
        
        void init() override;
        void init_req() override;


    };

    class strtable : public state_init {
    public:
        str_intern  construct_key;
        str_intern  mb_detect_order;
        
        str_intern  setdate;
        str_intern  settime;
        str_intern  diff;
        str_intern  date;
        str_intern  strtotime;
        str_intern  invoke_fn;


        
        void init() override;
    };


    class file_res {
    public:
        val_rc    file_;//PHP file resource object
        file_res(str_ptr path, 
                 str_ptr mode, 
                    bool use_inc_path = false, 
                    val_ptr ctx = val_ptr());

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

    str_rc stripslashes(str_ptr str);

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

    val_rc fgetcsv(val_ptr file_res, htab_ptr named_args = htab_ptr());

    bool fclose(val_ptr fres);

    val_rc fwrite(val_ptr fres, str_ptr data, zend_long length = -1);

    str_rc fread(val_ptr fres, int length=-1);

    str_rc preg_quote(str_ptr expr, str_ptr delimiter);


    str_rc file_content(str_ptr path, int offset = 0, size_t len = 0);

    // Ackward full version
    str_rc file_get_contents(
        str_ptr path, 
        bool include_path = false, 
        val_ptr context = val_ptr(), 
        int offset=0, 
        size_t len=0
        );

    int file_put_contents(str_ptr filename, val_ptr data,
        int flags = 0, val_ptr context = nullptr);

    // the "constant" is renamed as get_constant
    val_rc constant(str_ptr name);

    bool is_dir(str_ptr path);

    str_rc dirname(str_ptr path, int level=1);

    str_rc basename(str_ptr path, str_ptr sfx = str_ptr::empty_str());

    bool defined(str_ptr name);

    bool define(str_ptr name, val_ptr value);

    bool define(str_ptr name, str_ptr value);



    val_rc call_user_func_array(zval* arg1, zval* arg2);

    str_rc php_sapi_name();

    long filemtime(str_ptr path);

    bool is_dir(str_ptr path);

    bool is_file(str_ptr path);

    bool is_link(str_ptr path);

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

    bool copy_file(str_ptr from, str_ptr to);

    bool rmdir(str_ptr dpath);

    val_rc unserialize(str_ptr data, htab_ptr options = htab_ptr());

    str_rc sha1(str_ptr value, bool binary = false);

    str_rc file_extension(str_ptr path);

    int session_status();

    bool session_write_close();

    str_rc session_id(str_ptr id = str_ptr());

    str_rc session_name(str_ptr name = str_ptr());

    bool session_regenerate_id(bool deleteOld = false);

    bool session_start();

    bool session_set_save_handler(obj_ptr adapter, bool reg_shutdown = true);

    str_rc session_save_path();

    str_rc session_save_path(str_ptr path);

    bool headers_sent(str_rc& filename, val_rc& lineNum);

    bool headers_sent();

}; // end namespace zpp
#endif

