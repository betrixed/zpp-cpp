 /** 
 *   @file fn_call.cpp
  *  @brief Implementation of fn_call class for calling PHP functions from C++
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2025 Michael Rynn
    *  @license BSD 3-Clause License
  */

#ifndef FN_CALL_CPP
#define FN_CALL_CPP

#ifndef FN_CALL_H
#include "fn_call.h"
#endif

#ifndef HTAB_WALK_H
#include "htab_walk.h"
#endif

#include <filesystem>

#ifndef DIRECTORY_SEPARATOR
#define DIRECTORY_SEPARATOR '/'
#endif

namespace zpp {

// static and externals
fntable FTAB;
strtable STAB;

void register_fn_calls()
{

    STATE_INIT_ADD(FTAB);
    STATE_INIT_ADD(STAB);
}

class fn_weakref_create : public fn_call_args<1>
{
public:
    obj_rc call(obj_ptr wref);
};

class fn_weakref_get : public fn_call
{
public:
    obj_rc call(obj_ptr wref);
};

class fn_constant : public  fn_call_args<1>
{
public:
    val_rc call(str_ptr name);
};



class fn_realpath : public fn_call_args<1>
{
public:
    str_rc call(str_ptr path);
};

class fn_opendir : public fn_call_args<1> {
public:
    val_rc call(str_ptr path);
};

class fn_readdir : public fn_call_args<1> {
public:
    val_rc call(val_ptr dh);
};

class fn_closedir : public fn_call_args<1> {
public:
    void call(val_ptr dh);
};

class fn_glob : public fn_call_args<2> {
public:
    htab_rc call(str_ptr name, int flags=0);
};

class fn_mkdir : public fn_call_args<3> {
public:
    bool call(str_ptr path, int permissions = 0755, bool recurse = false);
};

class TLfnTable {
public:
    TLfnTable() : configured_(false) {}

    bool  configured_;
    fn_call          class_exists;
    fn_call          dirname;
    fn_call          extension_loaded;
    fn_call          file_get_contents;
    fn_call          fopen;
    fn_call          function_exists;
    fn_call          preg_quote;
        
    
    fn_call_args1    fclose;

    fn_constant   get_constant;
    
    fn_define     define;
    fn_defined    defined;
    PathInfo      pathinfo;
    FCall2        call_user_func_array;
    fn_call       php_sapi_name;
    fn_filemtime  filemtime;
    //fn_simple_loader simple_loader;

    fn_call_args1 is_dir;
    fn_call_args1 is_file;

    fn_opendir    opendir;
    fn_readdir    readdir;
    fn_closedir   closedir;
    fn_mkdir      mkdir;
    fn_realpath   realpath; 
    fn_glob       glob;

    fn_call_args2     unlink;
    fn_call_args2     fgets;
    fn_call_args3     fwrite;
    fn_call_args2     fread;
    fn_call_args2     sha1;

    fn_call_args2     unserialize;
    fn_call_args1     serialize;

    fn_call_args1     is_readable;

    fn_weakref_create weakref_create;
    fn_weakref_get    weakref_get;

    void init(const fntable& ftab)
    {
        if (configured_)
        {
            return;
        }

        configured_ = true;

        class_exists.set_fci(ftab.s_class_exists);
        extension_loaded.set_fci(ftab.s_extension_loaded);
        fclose.set_fci(ftab.s_fclose);
        fopen.set_fci(ftab.s_fopen);

        file_get_contents.set_fci(ftab.s_file_get_contents);
        function_exists.set_fci(ftab.s_function_exists);
        preg_quote.set_fci(ftab.s_preg_quote);

        

        
        
        
        pathinfo.set_fci(ftab.s_pathinfo);
        
        

        is_dir.set_fci(ftab.s_isdir);
        is_file.set_fci(ftab.s_isfile);
        is_readable.set_fci(ftab.s_isreadable);

        opendir.set_fci(ftab.s_opendir);
        readdir.set_fci(ftab.s_readdir);
        closedir.set_fci(ftab.s_closedir);
        mkdir.set_fci(ftab.s_mkdir);

        realpath.set_fci(ftab.s_realpath);

        weakref_create.set_fci(ftab.s_weakref_create);
        weakref_get.set_fci(ftab.s_weakref_get);

        defined.set_fci(ftab.s_defined);
        define.set_fci(ftab.s_define);
        get_constant.set_fci(ftab.s_constant);
        dirname.set_fci(ftab.s_dirname);
        
        call_user_func_array.set_fci(ftab.s_call_user_func_array);
        php_sapi_name.set_fci(ftab.s_php_sapi_name);
        filemtime.set_fci(ftab.s_filemtime);
        glob.set_fci(ftab.s_glob);
        unlink.set_fci(ftab.s_unlink);
        fwrite.set_fci(ftab.s_fwrite);
        fread.set_fci(ftab.s_fread);

        fgets.set_fci(ftab.s_fgets);
        serialize.set_fci(ftab.s_serialize);
        unserialize.set_fci(ftab.s_unserialize);
        sha1.set_fci(ftab.s_sha1);
        
    }


};
// as thread_local , won't be exporting the instance of this

thread_local TLfnTable TLFNs;




void 
fn_result::throw_failed()
{
    val_ptr fn(&cfi_.fci_.function_name);
    str_rc name(fn.zstr());
    if (!name.ok())
    {
        name = "Name not set";
    }
    zend_throw_error(zend_ce_error, "fn_call fail %s", name.data());
}

bool
fn_result::call_fn()
{
    if (cfi_.fci_.size==0)
    {
        throw_failed();
    }
    else if (zend_call_function(&cfi_.fci_,  &cfi_.cache_) == SUCCESS)
    {
        return true;
    }
    return false;
}

val_rc  
fn_result::mixed()
{
    val_rc result;
    if (call_fn())
    {
        // ZVAL_COPY_VALUE, 
        result = std::move(result_);
    }
    return result;
}

str_rc  
fn_result::str()
{
    str_rc result;
    if (call_fn())
    {
        // ZVAL_COPY_VALUE, 
        result = std::move(result_);
    }
    return result;
}

obj_rc  
fn_result::obj()
{
    obj_rc result;
    if (call_fn())
    {
        // ZVAL_COPY_VALUE, 
        result = std::move(result_);
    }
    return result;
}

bool  
fn_result::zbool()
{
    if (call_fn())
    {
        auto ztype = val_ptr(&result_).ref_type();
        switch(ztype)
        {
        case IS_TRUE:
            return true;
        case IS_FALSE:
            return false;
        default:
            throw_failed();
        }
    }
    return false;
}


/** reset this from the constructor information */
void 
fn_call::set_fci(zend_string* method, zend_object* obj)
{
    // Not reference counted!   
    zval *p = &fci_.function_name;
    *p = {0};
    val_ptr::string_bind(p, method);  
    // mark as inited
    fci_.size = sizeof(fci_); 
    fci_.object = obj;
    cache_.object = obj;
}

fn_call::fn_call() : fci_({0}), cache_({0})
{

}

fn_call::fn_call(zend_string* method, zend_object* obj) : fci_({0}), cache_({0})
{
    set_fci(method, obj);
}

fn_call::~fn_call()
{
}


void fn_call::debug_dump()
{
    zend_printf("fci_.size %ld\n", fci_.size);
    showobj("fci_.object", fci_.object);
    showmem("fci_.function_name", &fci_.function_name);
    zend_printf("param_count %d, params %lx\n", fci_.param_count, (unsigned long) fci_.params);

    for(uint32_t i = 0; i < fci_.param_count; i++)
    {
        zend_printf("%d: ", i); showmem("zval ", fci_.params+i);
    }
    /*zend_function *function_handler;
    zend_class_entry *calling_scope;
    zend_class_entry *called_scope;
    zend_object *object; // Instance of object for method calls 
    zend_object *closure; // Closure reference, only if the callable *is* the object
    */
    zend_printf("function_handler %lx\n", (unsigned long)cache_.function_handler);
    zend_printf("calling_scope %lx\n", (unsigned long)cache_.calling_scope);
    zend_printf("called_scope %lx\n", (unsigned long)cache_.called_scope);
    zend_printf("object %lx\n", (unsigned long)cache_.object);
    zend_printf("closure %lx\n", (unsigned long)cache_.closure);
}

void 
fn_call::set_obj(zend_object* zob)
{
    //? why is this required twice? 
    // First time the cache_ value will be empty.
    fci_.object = zob;
    cache_.object = zob;
}

bool class_exists(str_ptr arg)
{
    fn_params<1> c_exists(TLFNs.class_exists);
    val_ptr::string_bind(&c_exists.params[0], arg);
    return c_exists.zbool();
}

bool 
function_exists(str_ptr name)
{
    fn_params<1> fex(TLFNs.function_exists);
    val_ptr::string_bind(&fex.params[0], name);
    return fex.zbool();
}


val_rc 
fopen(str_ptr path, str_ptr mode,
    bool use_include_path, val_ptr context)
{
    fn_params<4> fn(TLFNs.fopen);
    zval* pz = fn.argsptr();

    val_ptr::string_bind(pz, path);
    pz++;
    val_ptr::string_bind(pz, mode);
    pz++;
    ZVAL_BOOL(pz, use_include_path);//3
    pz++;

    if (context.ok())
    {
        ZVAL_COPY_VALUE(pz, context);
    }
    else {
        ZVAL_NULL(pz);
    }//4

    return fn.mixed();
}


file_res::file_res(
    str_ptr path, str_ptr mode, 
    bool use_inc_path, val_ptr ctx)
{
    file_ = fopen(path, mode, use_inc_path, ctx);
}

file_res::~file_res()
{
    close();
}

void file_res::close(){
    if (file_.ok())
    {
        fclose(file_);
        file_.set_null();
        //showmem("After close", file_);
    }
}

bool file_res::isopen()
{
    return file_.ok();
}

str_rc 
sha1(str_ptr value, bool binary)
{
    fn_params<1> fn(TLFNs.sha1);
    zval* pz = fn.argsptr();

    val_ptr::string_bind(pz, value);
    pz++;
    ZVAL_BOOL(pz, binary);

    return fn.str();
}

bool is_readable(str_ptr path)
{
    fn_params<1> fn(TLFNs.is_readable);
    val_ptr::string_bind(&fn.params[0], path);
    return fn.zbool();
}

bool 
fclose(val_ptr fres)
{   
    fn_params<1>  fn(TLFNs.fclose);
    ZVAL_COPY_VALUE(fn.argsptr(), fres);
    return fn.zbool();
}


str_rc 
stripslashes(str_ptr str)
{
    fn_call fc(STAB.stripslashes);
    fn_params<1> fn(fc);
    val_ptr::string_bind(fn.argsptr(), str);
    return fn.str();
}

 str_rc 
 file_content(str_ptr path, int offset, size_t len)
 {
    fn_params<3>   fn(TLFNs.file_get_contents);
    
    zval* pz = fn.argsptr();

    val_ptr::string_bind(pz, path); 
    ZVAL_BOOL(pz+1, false);
    ZVAL_NULL(pz+2); // resource arg
    ZVAL_LONG(pz+3, offset);
    if (len > 0)
    {
        ZVAL_LONG(pz+4, len);
    }
    else {
        ZVAL_NULL(pz+4);
    }
    return fn.str();
}

val_rc 
pathinfo(str_ptr path, int flags)
{
    fn_params<2> fn(TLFNs.pathinfo);
    zval* pz = fn.argsptr();
    val_ptr::string_bind(pz, path);
    ZVAL_LONG(pz+1, flags);

    return fn.mixed();
}

//bool callable_fn(val_rc& result, val_rc& callme, int argct = 0, zval* argv = nullptr);

/*
val_rc 
fn_simple_loader::call(str_ptr path)
{
    val_rc result;
    showmem("fn name", &fci_.function_name);
    val_ptr::string_bind(argsptr(), path);
    showstr("\nload path", path);
    result = call_fn();
    showmem("loader call", result);
    return result;
}
*/

val_rc
array_pop(val_rc& array_ref)
{
    fn_call_args<1> fn;

    fn.set_fname(STAB.array_pop);
    
    //zend_printf("\nprepare array_pop\n");
    htab_rw hw(array_ref); // must be writable
    
    array_ref.make_ref(); // must be reference


    //zend_printf("\nprepare 2 array_pop\n");
    ZVAL_COPY_VALUE(fn.argsptr(), array_ref);
    return fn.call_fn();
}


htab_rc
array_splice(htab_rc& input, int offset, int length, htab_ptr replace)
{
    fn_call_args<4> fn;
    htab_rc result;

    fn.set_fname(STAB.array_splice);
    zval* args = fn.argsptr();

    htab_rw hw(input); // make writable
    val_ptr::array_bind(args, input);
    ZVAL_NEW_REF(args, args);
    args++;
    ZVAL_LONG(args,offset);
    args++;
    ZVAL_LONG(args, length);
    args++;
    if (replace.size())
    {
        val_ptr::array_bind(args, replace);
    }
    else {
        ZVAL_EMPTY_ARRAY(args);
    }
    result = fn.call_fn();
    return result;
}


str_rc 
preg_quote(str_ptr expr, str_ptr delimiter)
{
    fn_params<2> pqt(TLFNs.preg_quote);
    val_ptr::string_bind(&pqt.params[0], expr);
    val_ptr::string_bind(&pqt.params[1], delimiter);
    return pqt.str();
}



str_rc 
addcslashes(str_ptr s, str_ptr escapes)
{
    fn_call_args<2>  fn;
    fn.set_fci(nullptr, STAB.addcslashes, nullptr);
    zval* pz = fn.argsptr();
    val_ptr::string_bind(pz, (zend_string*) s);
    val_ptr::string_bind(pz+1, (zend_string*) escapes);
    str_rc result = fn.call_fn();
    return result;
}



bool 
file_exists(str_ptr path)
{
    return std::filesystem::exists(path.vstr());
}

bool 
extension_loaded(str_ptr name)
{
    fn_params<1> extloaded(TLFNs.extension_loaded);
    val_ptr::string_bind(&extloaded.params[0], name);
    return extloaded.zbool();
}

fn_fgetcsv::fn_fgetcsv() : fn_call_args<1>()
{
    set_fname(FTAB.s_fgetcsv);
}

str_rc
getcwd()
{
    fn_call fc;
    fc.set_fname(FTAB.s_getcwd);
    return fc.call_fn();
}

val_rc 
fn_constant::call(str_ptr name)
{
    //showstr("fn_constant call", name);
    //showmem("fn", &fci_.function_name);
    val_ptr::string_bind(argsptr(), name);
    val_rc result;
    result = call_fn();
    //showmem("constant value", result);
    return result;
}

long 
fn_filemtime::call(str_ptr path)
{
    val_ptr::string_bind(argsptr(), path);
    val_rc result = call_fn();
    return result.zlong();
}

bool 
fn_defined::call(str_ptr name)
{
    val_ptr::string_bind(argsptr(), name);
    val_rc result = call_fn();
    return result.isTrue();
}

bool 
fn_define::call(str_ptr constant_name, str_ptr value)
{
    zval* args = argsptr();
    val_ptr::string_bind(args, constant_name);
    val_ptr::string_bind(args+1, value);
    val_rc result = call_fn();
    return result.isTrue();
}

bool 
fn_define::call(str_ptr constant_name, val_ptr value)
{
    zval* args = argsptr();
    val_ptr::string_bind(args, constant_name);
    ZVAL_COPY_VALUE(args+1, value);
    val_rc result = call_fn();
    return result.isTrue();
}

val_rc 
fn_fgetcsv::call(val_ptr file_res)
{
    ZVAL_COPY_VALUE(argsptr(), file_res);
    return call_fn();
}



val_rc 
fn_opendir::call(str_ptr path)
{
    val_ptr::string_bind(argsptr(), path);
    return call_fn();
}

val_rc 
fn_readdir::call(val_ptr dh)
{
    ZVAL_COPY_VALUE(argsptr(), dh);
    return call_fn();
}

void 
fn_closedir::call(val_ptr dh)
{
    ZVAL_COPY_VALUE(argsptr(), dh);
    call_fn();
}

bool
fn_mkdir::call(str_ptr path, int permissions, bool recurse)
{
    zval* args = argsptr();
    val_ptr::string_bind(args, path);
    ZVAL_LONG(args+1, permissions);
    ZVAL_BOOL(args+2, recurse);

    val_rc result = call_fn();
    return result.isTrue();
}

bool extnloaded::call(str_ptr name)
{
    val_ptr::string_bind(argsptr(), (zend_string*) name);
    val_rc result = call_fn();
    return val_ptr(result).isTrue();
}

val_rc 
get_constant(str_ptr name)
{
    return TLFNs.get_constant.call(name);
}

str_rc 
fn_realpath::call(str_ptr path)
{
    val_ptr::string_bind(argsptr(), path);
    return call_fn();
}





str_rc
realpath(str_ptr path)
{
    return TLFNs.realpath.call(path);
}

str_rc 
dirname(str_ptr path, int level)
{
    fn_params<2>   dirname(TLFNs.get_dirname);
    valptr::string_bind(&dirname.params[0], path);
    ZVAL_LONG(&dirname.params[1], level);
    return dirname.str();
}

bool 
defined(str_ptr name)
{
    return TLFNs.defined.call(name);
}

bool 
define(str_ptr name, val_ptr value)
{
    return TLFNs.define.call(name, value);
}

bool 
define(str_ptr name, str_ptr value)
{
    //showstr("define ", name);
    //showstr(" = ", value);
    return TLFNs.define.call(name, value);
}

str_rc 
mb_detect_order(const val_rc& encoding)
{
    fn_call_args<1>  fn;

    fn.set_fci(nullptr, STAB.mb_detect_order, nullptr);
    ZVAL_COPY_VALUE(fn.argsptr(), encoding);

    return fn.call_fn();
}
    
str_rc 
mb_detect_encoding(str_ptr str, const val_rc& encodings, bool strict)
{
    fn_call_args<3>  fn;

    zval* pz = fn.argsptr();
    val_ptr::string_bind(pz, (zend_string*) str);
    ZVAL_COPY_VALUE(pz+1, encodings);
    ZVAL_BOOL(pz+2,strict);

    fn.set_fci(nullptr, STAB.mb_detect_encoding, nullptr);
    return fn.call_fn();
}

str_rc 
rawurlencode(str_ptr s)
{
    fn_call_args<1>  fn;
    val_ptr::string_bind(fn.argsptr(), s);

    fn.set_fci(nullptr, STAB.rawurlencode, nullptr);
    return fn.call_fn();

}

str_rc 
ucwords(str_ptr s)
{
    fn_call_args<1>  fn;
    val_ptr::string_bind(fn.argsptr(), s);
    fn.set_fci(nullptr, STAB.ucwords, nullptr);
    return fn.call_fn();
}

str_rc
strtr(str_ptr s, str_ptr from, str_ptr to)
{
    fn_call_args<3>  fn;
    zval* ap = fn.argsptr();

    val_ptr::string_bind(ap, s);
    val_ptr::string_bind(ap+1, from);
    val_ptr::string_bind(ap+2, to);

    fn.set_fci(nullptr, STAB.strtr, nullptr);
    return fn.call_fn();
}

val_rc 
json_decode(str_ptr str, bool asArray,  int flags)
{
    if (asArray)
    {
        flags |= PHP_JSON_OBJECT_AS_ARRAY;
    }
    val_rc result;

    //zend_result check = 
    php_json_decode_ex(result, str.data(), str.size(), flags, 512);

    return result;
}


void fntable::init_req()
{
    //zend_printf("init_req\n");

    TLFNs.init(*this);
}

void  // virtual
fntable::init()
{        
    s_extension_loaded = "extension_loaded";
    s_function_exists = "function_exists";
    s_class_exists = "class_exists";

    s_weakref_create = "weakreference::create";
    s_weakref_get = "get";

    s_preg_quote = "preg_quote";
    s_file_get_contents = "file_get_contents";
    s_pathinfo = "pathinfo";
    s_call_user_func_array = "call_user_func_array";
    s_fgetcsv = "fgetcsv";
    s_fopen = "fopen";
    s_fclose = "fclose";
    s_constant = "constant";
    s_dirname = "dirname";

    s_isdir = "is_dir";
    s_isfile = "is_file";

    s_opendir = "opendir";
    s_closedir = "closedir";
    s_readdir = "readdir";
    s_mkdir = "mkdir";

    s_getcwd = "getcwd";
    s_defined = "defined";
    s_define = "define";
    s_php_sapi_name = "php_sapi_name";
    s_filemtime = "filemtime";
    s_realpath = "realpath";
    s_glob = "glob";
    s_unlink = "unlink";

    s_fwrite = "fwrite";
    s_fread = "fread";

    s_fgets = "fgets";
    s_serialize = "serialize";
    s_unserialize = "unserialize";
    s_sha1 = "sha1";
    s_isreadable = "is_readable";
}

void  // virtual
strtable::init()
{
    construct_key = "__construct";
    mb_detect_order = "mb_detect_order";
    mb_detect_encoding = "mb_detect_encoding";
    setdate = "setdate";
    settime = "settime";
    diff = "diff";
    date = "date";
    strtotime = "strtotime";
    addcslashes = "addcslashes";
    rawurlencode = "rawurlencode";
    strtr = "strtr";
    ucwords = "ucwords";
    stripslashes = "stripslashes"; 
    array_pop = "array_pop";
    array_splice = "array_splice";
}

/**
 * Dyanmic args setup, array values only
 */
args_spread::args_spread(htab_ptr args, int prefixct)
{
    argct_ = args.size() + prefixct;
    if (argct_)
    {
        // get cleared memory from PHP. Used to have 3rd offset argument.
        argv_ = (zval*) ecalloc(argct_, sizeof(zval));

        // general but inefficient
        htab_walk wk;
        auto value = wk.value();
        size_t ix = prefixct;
        for(wk.start(args); wk.ok(); wk.next(), ix++)
        {
            ZVAL_COPY_VALUE(argv_+ix, value);
        }
    }
    else {
        argv_ = nullptr;
    }
    
}

args_spread::~args_spread()
{
    if (argv_) {
        efree(argv_);
    }
}


/** result is reference */
bool callable_fn(
    val_rc& result, 
    val_rc& callme, 
    int argct, 
    zval* argv)
{

    //showmem("argv", argv);
    //showmem("result", result);

    if (call_user_function( 
        CG(function_table), 
        (zval*) nullptr, 
        (zval*) callme, 
        (zval*) result, 
        argct, 
        argv) != SUCCESS)
    {
        // TODO: exception message callme toString ??
        zend_throw_error(zend_ce_error, "Invalid callable");
        return false;
    }
    return true;
}



bool 
call_spread_fn(
    val_rc& result, 
    val_rc& callme, 
    htab_ptr args)
{
    args_spread spread(args);
    return callable_fn(result, callme, spread.arg_ct(), spread.arg_v());
}

FCall2::FCall2()
{  
}

FCall2::FCall2(str_ptr func)
{
    set_fname(func);
}

val_rc  
FCall2::call(zval* arg1, zval* arg2)
{
    ZVAL_COPY_VALUE(&params[0], arg1);
    ZVAL_COPY_VALUE(&params[1], arg2);
    return call_fn();
}


val_rc 
call_user_func_array(zval* arg1, zval* arg2)
{
    return TLFNs.call_user_func_array.call(arg1,arg2);
}

str_rc php_sapi_name()
{
    return TLFNs.php_sapi_name.call_fn();
}

long 
filemtime(str_ptr path)
{
    return TLFNs.filemtime.call(path);
}

bool 
is_dir(str_ptr path)
{
    auto& fn = TLFNs.is_dir;
    val_ptr::string_bind(fn.argsptr(), path);
    val_rc result = fn.call_fn();
    return result.isTrue();
}

bool 
is_file(str_ptr path)
{
    auto& fn = TLFNs.is_file;
    val_ptr::string_bind(fn.argsptr(), path);
    val_rc result = fn.call_fn();
    return result.isTrue();
}


val_rc 
opendir(str_ptr path)
{
    return TLFNs.opendir.call(path);
}

val_rc readdir(val_ptr dh)
{
    return TLFNs.readdir.call(dh);
}

void closedir(val_ptr dh)
{
    TLFNs.closedir.call(dh);
}


bool mkdir(str_ptr path, int permissions, bool recurse)
{
    return TLFNs.mkdir.call(path, permissions, recurse);
}

htab_rc 
fn_glob::call(str_ptr name, int flags)
{
    zval* pz = argsptr();
    val_ptr::string_bind(pz, name);
    ZVAL_LONG(pz+1, flags);

    htab_rc result;
    result = call_fn();
    return result;
}

htab_rc 
glob(str_ptr wcard, int flags)
{
    htab_rc result;
    result = TLFNs.glob.call(wcard, flags);
    return result;
}

str_rc 
fgets(val_ptr res, zend_long limit)
{
    auto& fn = TLFNs.fgets;

    zval *ap = fn.argsptr();
    ZVAL_COPY_VALUE(ap, res);
    ap++;
    if (limit > 0)
    {
        ZVAL_LONG(ap, limit);
    }
    else 
    {
        ZVAL_NULL(ap);
    }
    str_rc result = fn.call_fn();
    return result;
}

str_rc fread(val_ptr fres, int length)
{
    auto& fn = TLFNs.fread;

    zval *ap = fn.argsptr();

    ZVAL_COPY_VALUE(ap, fres);
    ap++;
    ZVAL_LONG(ap, length);

    str_rc result = fn.call_fn();
    return result;
}

val_rc 
fwrite(val_ptr res, str_ptr data, zend_long len)
{
    val_rc result;

    auto& fn = TLFNs.fwrite;

    zval *ap = fn.argsptr();
    ZVAL_COPY_VALUE(ap, res);
    ap++;
    val_ptr::string_bind(ap, data);
    ap++;
    if (len > 0)
    {
        ZVAL_LONG(ap, len);
    }
    else 
    {
        ZVAL_NULL(ap);
    }
    result = fn.call_fn();
    return result; 
}

bool
unlink(str_ptr path, val_ptr context)
{
    auto& fn = TLFNs.unlink;
    zval* ap = fn.argsptr();
    val_ptr::string_bind(ap, path);
    ap++;
    if (context.ok())
    {
        ZVAL_COPY_VALUE(ap, context);
    }
    else {
        ZVAL_NULL(ap);
    }
    val_rc result = fn.call_fn();
    return result.isTrue();
}

obj_rc 
fn_weakref_create::call(obj_ptr wref)
{
    //showobj("weakref arg", wref);
    //showmem("fn_name", &fci_.function_name);
    ZVAL_OBJ(argsptr(), wref);
    val_rc result = call_fn();
    //showmem("fn_weakref_create", result);
    return result.zobject();
}

obj_rc 
weakref_create(obj_ptr obj)
{
    return TLFNs.weakref_create.call(obj);
}

obj_rc
fn_weakref_get::call(obj_ptr wref)
{
    set_obj(wref);
    val_rc result = call_fn();
    return result.zobject();
}

obj_rc 
weakref_get(obj_ptr wref)
{
    return TLFNs.weakref_get.call(wref);
}

str_rc 
serialize(val_ptr value)
{
    auto& fn = TLFNs.serialize;
    zval* ap = fn.argsptr();
    ZVAL_COPY_VALUE(ap, value);
    str_rc result = fn.call_fn();
    return result;
}


val_rc 
unserialize(str_ptr data, htab_ptr options)
{
    auto& fn = TLFNs.unserialize;
    zval* ap = fn.argsptr();
    val_ptr::string_bind(ap, data);
    ap++;
    if (options.size())
    {
        val_ptr::array_bind(ap, options);
    }
    else {
        ZVAL_EMPTY_ARRAY(ap);
    }
    val_rc result = fn.call_fn();
    return result;
}

} // end namespace zpp
//fn_call.cpp
#endif

