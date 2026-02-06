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
class fn_dirname : public fn_call_args<1>
{
public:
    str_rc call(str_ptr name, int level=1);
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
    extnloaded       extension_loaded;
    fnexists         function_exists;
    fn_class_exists  class_exists;
    pregquote        preg_quote;
    file_content     file_get_contents;
    fn_call_args4    fopen;
    fn_call_args1    fclose;

    fn_constant   get_constant;
    fn_dirname    get_dirname;
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
        
        extension_loaded.set_fname(ftab.s_extension_loaded);
        function_exists.set_fname(ftab.s_function_exists);
        class_exists.set_fname(ftab.s_class_exists);

        preg_quote.set_fname(ftab.s_preg_quote);
        file_get_contents.set_fname(ftab.s_file_get_contents);
        pathinfo.set_fname(ftab.s_pathinfo);
        fopen.set_fname(ftab.s_fopen);
        fclose.set_fname(ftab.s_fclose);

        is_dir.set_fname(ftab.s_isdir);
        is_file.set_fname(ftab.s_isfile);

        opendir.set_fname(ftab.s_opendir);
        readdir.set_fname(ftab.s_readdir);
        closedir.set_fname(ftab.s_closedir);
        mkdir.set_fname(ftab.s_mkdir);

        realpath.set_fname(ftab.s_realpath);

        weakref_create.set_fname(ftab.s_weakref_create);
        weakref_get.set_fname(ftab.s_weakref_get);

        defined.set_fname(ftab.s_defined);
        define.set_fname(ftab.s_define);
        get_constant.set_fname(ftab.s_constant);
        get_dirname.set_fname(ftab.s_dirname);
        
        call_user_func_array.set_fname(ftab.s_call_user_func_array);
        php_sapi_name.set_fname(ftab.s_php_sapi_name);
        filemtime.set_fname(ftab.s_filemtime);
        glob.set_fname(ftab.s_glob);
        unlink.set_fname(ftab.s_unlink);
        fwrite.set_fname(ftab.s_fwrite);
        fread.set_fname(ftab.s_fread);

        fgets.set_fname(ftab.s_fgets);
        serialize.set_fname(ftab.s_serialize);
        unserialize.set_fname(ftab.s_unserialize);
        sha1.set_fname(ftab.s_sha1);
        is_readable.set_fname(ftab.s_isreadable);
    }


};
// as thread_local , won't be exporting the instance of this

thread_local TLfnTable TLFNs;


fn_call::fn_call() 
{
    fci_ = {0};
    cache_ = {0};  
    result_ = {0};

    fci_.retval = &result_;
}

fn_call::~fn_call()
{
}

void fn_call::wipe()
{
    zval* ptr = fci_.params;
    auto ct =   fci_.param_count;
    if (ptr && ct) {
        memset(ptr, 0, ct*sizeof(zval));
    }
    // in case previous result was not cleared
    result_ = {0};
}


void 
fn_call::set_named_args(HashTable* nargs)
{
    fci_.named_params = nargs; 
}
/** reset this from the constructor information */
void 
fn_call::set_fci(zend_object* obj, str_ptr method, HashTable* nargs)
{

    //method_name_ = method;

    fci_.size = sizeof(fci_);
    fci_.object = obj;

    // in case changing the object?
    //cache_.object = obj; 
    //showstr("method", method);

    zval *p = &fci_.function_name;
    *p = {0};
    // fci_.function_name is a COPY_VALUE
    ZVAL_STR(p, method);  

    fci_.named_params = nargs; 
}

void 
fn_call::set_obj(zend_object* obj)
{
    //TODO: ?why this is required both? 
    //Maybe first pass the cache_ value will be empty?
    
    fci_.object = obj;
    cache_.object = obj;
}

/**
 * set_fname must convert the C string
 *  to a zend_string* with persistent flag set.
 * 
 * For use at module global PHP function adapters.
 * These are created in linked "state_init" structures call to init().
 * Unset flag will fail for this situation.
 * 
 * Recommend not used for throw away objects.

void 
fn_call::set_fname(const char* name)
{

    set_fci(nullptr, zend_string_init(name,strlen(name),1), nullptr);
}
*/

void 
fn_call::set_fname(str_ptr name)
{
    set_fci(nullptr, name, nullptr);
}

void fn_call::throw_failed()
{
    val_ptr fn(&fci_.function_name);
    str_ptr name(fn.zstr());
    if (name.ok())
        zend_throw_error(zend_ce_error, "fn_call_failed for %s", name.data());
    else 
        zend_throw_error(zend_ce_error, "fn_call_failed, no name");
}

bool fnexists::call(str_ptr arg)
{
    ZVAL_STR(argsptr(), arg);
    val_rc result = call_fn();
    return val_ptr(result).zbool();
}

bool fn_class_exists::call(str_ptr arg)
{
    ZVAL_STR(argsptr(), arg);
    val_rc result = call_fn();
    return val_ptr(result).zbool();
}

str_rc 
fn_dirname::call(str_ptr name, int level)
{
    zval* pz = argsptr();

    ZVAL_STR(pz, name);
    ZVAL_LONG(pz+1, level);

    str_rc result = call_fn();
    return result;
}

str_rc 
pregquote::call(str_ptr str, str_ptr delimiter)
{
    zval* pz = argsptr();

    ZVAL_STR(pz,   (zend_string*) str);
    ZVAL_STR(pz+1, (zend_string*) delimiter);

    val_rc result = call_fn();
    return str_rc(val_ptr(result).zstr());
}


val_rc 
fopen(str_ptr path, str_ptr mode,
    bool use_include_path, val_ptr context)
{
    auto &fn = TLFNs.fopen;
    zval* pz = fn.argsptr();
    ZVAL_STR(pz, (zend_string*) path);
    pz++;
    ZVAL_STR(pz, (zend_string*) mode);
    pz++;
    ZVAL_BOOL(pz, use_include_path);
    pz++;
    if (context.ok())
    {
        ZVAL_COPY_VALUE(pz, context);
    }
    else {
        ZVAL_NULL(pz);
    }

    return fn.call_fn();
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
    auto& fn = TLFNs.sha1;
    zval* p = fn.argsptr();

    ZVAL_STR(p, value);
    p++;
    ZVAL_BOOL(p, binary);
    str_rc result = fn.call_fn();
    return result;
}

bool is_readable(str_ptr path)
{
    auto& fn = TLFNs.is_readable;

    zval* p = fn.argsptr();
    ZVAL_STR(p, path);
    val_rc result= fn.call_fn();
    return result.isTrue();
}

bool 
fclose(val_ptr fres)
{   
    auto& fn = TLFNs.fclose;

    ZVAL_COPY_VALUE(fn.argsptr(), fres);
    val_rc result = fn.call_fn();
    return val_ptr(result).isTrue();
}

fn_stripslashes::fn_stripslashes() : fn_call_args<1>()
{
    set_fname(STAB.stripslashes);
}

str_rc 
fn_stripslashes::call(str_ptr str)
{
    ZVAL_STR(argsptr(), str);
    return str_rc(call_fn());
}

str_rc 
file_content::call(str_ptr path, int offset, size_t len)
{
    //showmem("fn_name", &fci_.function_name);
    zval* pz = argsptr();

    ZVAL_STR(pz, (zend_string*) path);
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
    return str_rc(call_fn());
}

val_rc
PathInfo::call(str_ptr path, int flags)
{
    zval* pz = argsptr();
    ZVAL_STR(pz, path);
    ZVAL_LONG(pz+1, flags);
    return call_fn();
}

val_rc pathinfo(str_ptr path, int flags)
{
    return TLFNs.pathinfo.call(path,flags);
}

//bool callable_fn(val_rc& result, val_rc& callme, int argct = 0, zval* argv = nullptr);

/*
val_rc 
fn_simple_loader::call(str_ptr path)
{
    val_rc result;
    showmem("fn name", &fci_.function_name);
    ZVAL_STR(argsptr(), path);
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
    ZVAL_ARR(args, input);
    ZVAL_NEW_REF(args, args);
    args++;
    ZVAL_LONG(args,offset);
    args++;
    ZVAL_LONG(args, length);
    args++;
    if (replace.size())
    {
        ZVAL_ARR(args, replace);
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
    return TLFNs.preg_quote.call(expr, delimiter);
}

// TODO: The throw_failed won't get through. A bireturn?
val_rc
fn_call::call_fn()
{
    if (fci_.size==0)
    {
        zend_throw_error(zend_ce_error,"call_fn() fci is not initialized");
    }
    else
    {
        zend_result ok =  zend_call_function(&fci_, &cache_);
        if (ok != SUCCESS)
        {
            throw_failed();
        }
    }

    val_rc temp(std::move(result_));
    return temp;
}

str_rc 
addcslashes(str_ptr s, str_ptr escapes)
{
    fn_call_args<2>  fn;
    fn.set_fci(nullptr, STAB.addcslashes, nullptr);
    zval* pz = fn.argsptr();
    ZVAL_STR(pz, (zend_string*) s);
    ZVAL_STR(pz+1, (zend_string*) escapes);
    str_rc result = fn.call_fn();
    return result;
}

 str_rc 
 file_get_contents(str_ptr path, int offset, size_t len)
 {
    //zend_printf("file get contents for %s\n", path.data());

    str_rc result = TLFNs.file_get_contents.call(path, offset, len);

    //showmem("contents", result);
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
    return TLFNs.extension_loaded.call(name);
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
    ZVAL_STR(argsptr(), name);
    val_rc result;
    result = call_fn();
    //showmem("constant value", result);
    return result;
}

long 
fn_filemtime::call(str_ptr path)
{
    ZVAL_STR(argsptr(), path);
    val_rc result = call_fn();
    return result.zlong();
}

bool 
fn_defined::call(str_ptr name)
{
    ZVAL_STR(argsptr(), name);
    val_rc result = call_fn();
    return result.isTrue();
}

bool 
fn_define::call(str_ptr constant_name, str_ptr value)
{
    zval* args = argsptr();
    ZVAL_STR(args, constant_name);
    ZVAL_STR(args+1, value);
    val_rc result = call_fn();
    return result.isTrue();
}

bool 
fn_define::call(str_ptr constant_name, val_ptr value)
{
    zval* args = argsptr();
    ZVAL_STR(args, constant_name);
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
    ZVAL_STR(argsptr(), path);
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
    ZVAL_STR(args, path);
    ZVAL_LONG(args+1, permissions);
    ZVAL_BOOL(args+2, recurse);

    val_rc result = call_fn();
    return result.isTrue();
}

bool extnloaded::call(str_ptr name)
{
    ZVAL_STR(argsptr(), (zend_string*) name);
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
    ZVAL_STR(argsptr(), path);
    return call_fn();
}

bool 
function_exists(str_ptr name)
{
    return TLFNs.function_exists.call(name);
}

bool 
class_exists(str_ptr name)
{
    return TLFNs.class_exists.call(name);
}

str_rc
realpath(str_ptr path)
{
    return TLFNs.realpath.call(path);
}

str_rc 
dirname(str_ptr path, int level)
{
    return TLFNs.get_dirname.call(path, level);
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
    ZVAL_STR(pz, (zend_string*) str);
    ZVAL_COPY_VALUE(pz+1, encodings);
    ZVAL_BOOL(pz+2,strict);

    fn.set_fci(nullptr, STAB.mb_detect_encoding, nullptr);
    return fn.call_fn();
}

str_rc 
rawurlencode(str_ptr s)
{
    fn_call_args<1>  fn;
    ZVAL_STR(fn.argsptr(), s);

    fn.set_fci(nullptr, STAB.rawurlencode, nullptr);
    return fn.call_fn();

}

str_rc 
ucwords(str_ptr s)
{
    fn_call_args<1>  fn;
    ZVAL_STR(fn.argsptr(), s);
    fn.set_fci(nullptr, STAB.ucwords, nullptr);
    return fn.call_fn();
}

str_rc
strtr(str_ptr s, str_ptr from, str_ptr to)
{
    fn_call_args<3>  fn;
    zval* ap = fn.argsptr();

    ZVAL_STR(ap, s);
    ZVAL_STR(ap+1, from);
    ZVAL_STR(ap+2, to);

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
    ZVAL_STR(fn.argsptr(), path);
    val_rc result = fn.call_fn();
    return result.isTrue();
}

bool 
is_file(str_ptr path)
{
    auto& fn = TLFNs.is_file;
    ZVAL_STR(fn.argsptr(), path);
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
    ZVAL_STR(pz, name);
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
    ZVAL_STR(ap, data);
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
    ZVAL_STR(ap, path);
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
    ZVAL_STR(ap, data);
    ap++;
    if (options.size())
    {
        ZVAL_ARR(ap, options);
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

