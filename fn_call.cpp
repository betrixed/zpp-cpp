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

 
#ifndef ZEND_API_H
extern "C" {
    #include <zend_API.h>
}
#endif

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
/*
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


class fn_glob : public fn_call_args<2> {
public:
    htab_rc call(str_ptr name, int flags=0);
};

class fn_mkdir : public fn_call_args<3> {
public:
    bool call(str_ptr path, int permissions = 0755, bool recurse = false);
};
*/
class TLfnTable {
public:
    TLfnTable() : configured_(false) {}

    bool  configured_;

    //fn_simple_loader simple_loader;
    fn_call          addcslashes;
    fn_call          array_pop;
    fn_call          array_splice;
    fn_call          call_user_func_array;
    fn_call          class_exists;
    fn_call          closedir;
    fn_call          constant;  
    fn_call          define;
    fn_call          defined;
    fn_call          dirname;
    fn_call          extension_loaded;
    fn_call          fclose;
    fn_call          fgetcsv;
    fn_call          fgets;
    fn_call          file_get_contents;
    fn_call          file_put_contents;
    fn_call          filemtime;
    fn_call          fopen;
    fn_call          fread;
    fn_call          function_exists;
    fn_call          fwrite;
    fn_call          getcwd;
    fn_call          glob;
    fn_call          is_dir;
    fn_call          is_file;
    fn_call          is_readable;
    fn_call          mb_detect_encoding;
    fn_call          mb_detect_order;
    fn_call          mkdir;
    fn_call          opendir;
    fn_call          pathinfo;
    fn_call          php_sapi_name;
    fn_call          preg_quote;
    fn_call          rawurlencode;
    fn_call          readdir;
    fn_call          realpath; 
    fn_call          serialize;

    fn_call          session_status;
    fn_call          session_write_close;

    fn_call          sha1;
    fn_call          stripslashes;
    fn_call          strtr;
    fn_call          ucwords;
    fn_call          unlink;
    fn_call          unserialize;
    fn_call          weakref_create;
    fn_call          weakref_get;

    void init(const fntable& ftab)
    {
        if (configured_)
        {
            return;
        }

        configured_ = true;

        addcslashes.set_fci(ftab.s_addcslashes);
        array_pop.set_fci(ftab.s_array_pop);
        array_splice.set_fci(ftab.s_array_splice);
        call_user_func_array.set_fci(ftab.s_call_user_func_array);
        class_exists.set_fci(ftab.s_class_exists);

        closedir.set_fci(ftab.s_closedir);
        constant.set_fci(ftab.s_constant);
        define.set_fci(ftab.s_define);
        defined.set_fci(ftab.s_defined);
        dirname.set_fci(ftab.s_dirname);

        extension_loaded.set_fci(ftab.s_extension_loaded);
        fclose.set_fci(ftab.s_fclose);
        fgetcsv.set_fci(ftab.s_fgetcsv);
        fgets.set_fci(ftab.s_fgets);
        file_get_contents.set_fci(ftab.s_file_get_contents);
        file_put_contents.set_fci(ftab.s_file_put_contents);

        filemtime.set_fci(ftab.s_filemtime);
        fopen.set_fci(ftab.s_fopen);
        fread.set_fci(ftab.s_fread);
        function_exists.set_fci(ftab.s_function_exists);
        fwrite.set_fci(ftab.s_fwrite);

        getcwd.set_fci(ftab.s_getcwd);
        glob.set_fci(ftab.s_glob);
        is_dir.set_fci(ftab.s_isdir);
        is_file.set_fci(ftab.s_isfile);
        is_readable.set_fci(ftab.s_isreadable);

        mb_detect_encoding.set_fci(ftab.s_mb_detect_encoding);
        mb_detect_order.set_fci(ftab.s_mb_detect_order);
        mkdir.set_fci(ftab.s_mkdir);
        opendir.set_fci(ftab.s_opendir);
        pathinfo.set_fci(ftab.s_pathinfo);

        php_sapi_name.set_fci(ftab.s_php_sapi_name);
        preg_quote.set_fci(ftab.s_preg_quote);
        rawurlencode.set_fci(ftab.s_rawurlencode);
        readdir.set_fci(ftab.s_readdir);
        realpath.set_fci(ftab.s_realpath);

        session_status.set_fci(ftab.session_status_fn);
        session_write_close.set_fci(ftab.session_write_close_fn);
        
        serialize.set_fci(ftab.s_serialize);
        sha1.set_fci(ftab.s_sha1);
        stripslashes.set_fci(ftab.s_stripslashes);
        strtr.set_fci(ftab.s_strtr);
        ucwords.set_fci(ftab.s_ucwords);

        unlink.set_fci(ftab.s_unlink);
        unserialize.set_fci(ftab.s_unserialize);
        weakref_create.set_fci(ftab.s_weakref_create);
        weakref_get.set_fci(ftab.s_weakref_get);
        
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
    result_ = {0};
    if (cfi_.fci_.size==0)
    {
        throw_failed();
    }
    else if (zend_call_function(&cfi_.fci_,  &cfi_.cache_) == SUCCESS)
    {
        return true;
    }
    throw_failed();
    return false;
}

val_rc  
fn_result::mixed()
{
    val_rc result;
    if (call_fn())
    {
        // Does ZVAL_COPY_VALUE, 
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
        result.adopt(val_ptr(&result_).zstr());
    }
    return result;
}

obj_rc  
fn_result::obj()
{
    obj_rc result;
    if (call_fn())
    {
        result.adopt(val_ptr(&result_).zobject());
    }
    return result;
}

htab_rc  
fn_result::array()
{
    htab_rc result;
    if (call_fn())
    {
        result.adopt(val_ptr(&result_).zarray());
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

zend_long  
fn_result::zlong()
{
    if (call_fn())
    {
        return val_ptr(&result_).zlong();
    }
    return false;
}


void fn_call::init_f()
{
    fci_ = {0};
    cache_ = {0}; 
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


fn_call::fn_call(zend_string* method) 
{   
    init_f();
    set_fci(method,nullptr);
}

fn_call::fn_call()
{
   init_f();
}

fn_call::fn_call(zend_string* method, zend_object* obj)
{
    init_f();
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
    fn_params<2> fn(TLFNs.sha1);
    zval* pz = fn.argsptr();

    val_ptr::string_bind(pz, value);
    ZVAL_BOOL(pz+1, binary);

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
    fn_params<1> fn(TLFNs.stripslashes);
    val_ptr::string_bind(fn.argsptr(), str);
    return fn.str();
}


int file_put_contents(str_ptr filename, val_ptr data,
    int flags, val_ptr context) 
{
    fn_params<4> fn(TLFNs.file_put_contents);

    zval* pz = fn.argsptr();

    val_ptr::string_bind(pz, filename);
    ZVAL_COPY_VALUE(pz+1, data);
    ZVAL_LONG(pz+2, flags);
    if (context)
        ZVAL_COPY_VALUE(pz+3, context);
    else 
        ZVAL_NULL(pz+3);

    val_rc result = fn.mixed();
    return result.zlong();
}

str_rc file_get_contents(
    str_ptr path, 
    bool include_path, 
    val_ptr context, 
    int offset, 
    size_t len
    )
{
    fn_params<5>   fn(TLFNs.file_get_contents);
    
    zval* pz = fn.argsptr();

    val_ptr::string_bind(pz, path); 

    ZVAL_BOOL(pz+1, include_path);
    if (context.isNull()) {
        ZVAL_NULL(pz+2); // resource arg
    }
    else {
        ZVAL_COPY_VALUE(pz+2, context);
    }

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
 str_rc 
 file_content(str_ptr path, int offset, size_t len)
 {
    //return file_get_contents(path, false, val_ptr(), offset, len);

    
    fn_params<5>   fn(TLFNs.file_get_contents);
    
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

str_rc 
file_extension(str_ptr path)
{
    val_rc result = pathinfo(path, PathInfo::EXTENSION);
    return result.zstr();
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
    fn_params<1> fn(TLFNs.array_pop);

    //zend_printf("\nprepare array_pop\n");
    htab_rw hw(array_ref); // must be writable
    array_ref.make_ref(); // must be reference

    //zend_printf("\nprepare 2 array_pop\n");
    ZVAL_COPY_VALUE(fn.argsptr(), array_ref);
    return fn.mixed();
}


htab_rc
array_splice(htab_rc& input, int offset, int length, htab_ptr replace)
{
    fn_params<4> fn(TLFNs.array_splice);
    zval* pz = fn.argsptr();

    htab_rw hw(input); // make writable
    val_ptr::array_bind(pz, input);
    ZVAL_NEW_REF(pz, pz);
    ZVAL_LONG(pz+1,offset);
    ZVAL_LONG(pz+2, length);
    if (replace.size())
    {
        val_ptr::array_bind(pz+3, replace);
    }
    else {
        ZVAL_EMPTY_ARRAY(pz+3);
    }
    
    return fn.array();
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
    fn_params<2>  fn(TLFNs.addcslashes);

    zval* pz = fn.argsptr();
    val_ptr::string_bind(pz, (zend_string*) s);
    val_ptr::string_bind(pz+1, (zend_string*) escapes);
    
    return fn.str();
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

str_rc
getcwd()
{
    fn_noparams fn(TLFNs.getcwd);
    return fn.str();
}

val_rc 
constant(str_ptr name)
{
    fn_params<1>  fn(TLFNs.constant);
    val_ptr::string_bind(fn.argsptr(), name);
    return fn.mixed();
}

long 
filemtime(str_ptr path)
{
    fn_params<1> fn(TLFNs.filemtime);

    val_ptr::string_bind(fn.argsptr(), path);
    return fn.zlong();
}

bool 
defined(str_ptr name)
{
    fn_params<1>  fn(TLFNs.defined);
    val_ptr::string_bind(fn.argsptr(), name);
    return fn.zbool();
}

bool 
define(str_ptr constant_name, str_ptr value)
{
    fn_params<2>  fn(TLFNs.define);
    zval* pz = fn.argsptr();
    val_ptr::string_bind(pz, constant_name);
    val_ptr::string_bind(pz+1, value);
    return fn.zbool();
}


bool 
define(str_ptr constant_name, val_ptr value)
{
    fn_params<2>  fn(TLFNs.define);
    zval* pz = fn.argsptr();
    val_ptr::string_bind(pz, constant_name);
    ZVAL_COPY_VALUE(pz+1, value);
    return fn.zbool();
}

val_rc 
fgetcsv(val_ptr file_res, htab_ptr named_args)
{
    fn_params<1>  fn(TLFNs.fgetcsv, named_args);
    fn.named_args(named_args);
    ZVAL_COPY_VALUE(fn.argsptr(), file_res);
    return fn.mixed();
}



val_rc 
opendir(str_ptr path)
{
    fn_params<1>  fn(TLFNs.opendir);
    val_ptr::string_bind(fn.argsptr(), path);
    return fn.mixed();
}

val_rc 
readdir(val_ptr dh)
{
    fn_params<1> fn(TLFNs.readdir);

    ZVAL_COPY_VALUE(fn.argsptr(), dh);
    return fn.mixed();
}

void 
closedir(val_ptr dh)
{
    fn_params<1> fn(TLFNs.closedir);
    ZVAL_COPY_VALUE(fn.argsptr(), dh);
    fn.call_fn();
}

bool
mkdir(str_ptr path, int permissions, bool recurse)
{
    fn_params<3> fn(TLFNs.mkdir);
    zval* pz = fn.argsptr();
    val_ptr::string_bind(pz, path);
    ZVAL_LONG(pz+1, permissions);
    ZVAL_BOOL(pz+2, recurse);
    return fn.zbool();
}

str_rc 
realpath(str_ptr path)
{
    fn_params<1> fn(TLFNs.realpath);
    val_ptr::string_bind(fn.argsptr(), path);
    return fn.str();
}


str_rc 
dirname(str_ptr path, int level)
{
    fn_params<2>   dirname(TLFNs.dirname);
    val_ptr::string_bind(&dirname.params[0], path);
    ZVAL_LONG(&dirname.params[1], level);
    return dirname.str();
}

str_rc 
mb_detect_order(const val_rc& encoding)
{
    fn_params<1>  fn(TLFNs.mb_detect_order);
    ZVAL_COPY_VALUE(fn.argsptr(), encoding);
    return fn.str();
}
    
str_rc 
mb_detect_encoding(str_ptr str, const val_rc& encodings, bool strict)
{
    fn_params<3>  fn(TLFNs.mb_detect_encoding);

    zval* pz = fn.argsptr();
    val_ptr::string_bind(pz, (zend_string*) str);
    ZVAL_COPY_VALUE(pz+1, encodings);
    ZVAL_BOOL(pz+2,strict);
    return fn.str();
}

str_rc 
rawurlencode(str_ptr s)
{
    fn_params<1>  fn(TLFNs.rawurlencode);
    val_ptr::string_bind(fn.argsptr(), s);
    return fn.str();
}

str_rc 
ucwords(str_ptr s)
{
    fn_params<1>  fn(TLFNs.ucwords);
    val_ptr::string_bind(fn.argsptr(), s);
    return fn.str();
}

str_rc
strtr(str_ptr s, str_ptr from, str_ptr to)
{
    fn_params<3>  fn(TLFNs.strtr);
    zval* ap = fn.argsptr();

    val_ptr::string_bind(ap, s);
    val_ptr::string_bind(ap+1, from);
    val_ptr::string_bind(ap+2, to);
    return fn.str();
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
    s_addcslashes = "addcslashes";
    s_array_pop = "array_pop";
    s_array_splice = "array_splice";
    s_call_user_func_array = "call_user_func_array";
    s_class_exists = "class_exists";
    
    s_closedir = "closedir";
    s_constant = "constant";
    s_define = "define";
    s_defined = "defined";
    s_dirname = "dirname";

    s_extension_loaded = "extension_loaded";
    s_fclose = "fclose";
    s_fgetcsv = "fgetcsv";
    s_fgets = "fgets";
    s_file_get_contents = "file_get_contents";
    s_file_put_contents = "file_put_contents";

    s_filemtime = "filemtime";
    s_fopen = "fopen";
    s_fread = "fread";
    s_function_exists = "function_exists";
    s_fwrite = "fwrite";
    
    s_getcwd = "getcwd";
    s_glob = "glob";
    s_isdir = "is_dir";
    s_isfile = "is_file";
    s_isreadable = "is_readable";
    
    s_mb_detect_encoding = "mb_detect_encoding";
    s_mb_detect_order = "mb_detect_order";
    s_mkdir = "mkdir";
    s_opendir = "opendir";
    s_pathinfo = "pathinfo";
    
    s_php_sapi_name = "php_sapi_name";
    s_preg_quote = "preg_quote";
    s_rawurlencode = "rawurlencode";
    s_readdir = "readdir";
    s_realpath = "realpath";

    s_serialize = "serialize";
    s_sha1 = "sha1";
    s_stripslashes = "stripslashes";
    s_strtr = "strtr";
    s_ucwords = "ucwords";

    s_unlink = "unlink";
    s_unserialize = "unserialize";
    s_weakref_create = "weakreference::create";
    s_weakref_get = "get";

    session_status_fn = "session_status";
    session_write_close_fn = "session_write_close";
}

void  // virtual
strtable::init()
{
    construct_key = "__construct";
    setdate = "setdate";
    settime = "settime";
    diff = "diff";
    date = "date";
    strtotime = "strtotime";
    invoke_fn = "__invoke";


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


bool 
call_spread_fn(
    val_rc& result, 
    val_rc& callme, 
    htab_ptr args)
{
    args_spread spread(args);
    return callable_fn(result, callme, spread.arg_ct(), spread.arg_v());
}


/** result is reference */
bool callable_fn(
    val_rc& result,
    val_rc& callme, 
    int argct, 
    zval* argv)
{

//zend_result _call_user_function_impl(zval *object, zval *function_name, 
    //zval *retval_ptr, uint32_t param_count, zval params[], HashTable *named_params) /* {{{ */
    zval fname = {0};
    ZVAL_STR(&fname, STAB.invoke_fn);

    if (_call_user_function_impl( 
        callme, //object
        &fname,
        result,  // result storage
        argct, 
        argv,
        (HashTable*) nullptr // 
        ) != SUCCESS)
    {
        // TODO: exception message callme toString ??
        zend_throw_error(zend_ce_error, "Invalid callable");
        return false;
    }
    return true;
}





val_rc 
call_user_func_array(zval* arg1, zval* arg2)
{
    fn_params<2> fn(TLFNs.call_user_func_array);
    zval* pz = fn.argsptr();
    ZVAL_COPY_VALUE(pz, arg1);
    ZVAL_COPY_VALUE(pz+1, arg2);
    return fn.mixed();
}

str_rc php_sapi_name()
{
    fn_noparams  fn(TLFNs.php_sapi_name);
    return fn.str();
}

bool 
is_dir(str_ptr path)
{
    fn_params<1> fn(TLFNs.is_dir);

    val_ptr::string_bind(fn.argsptr(), path);
    return fn.zbool();
}

bool 
is_file(str_ptr path)
{
    fn_params<1> fn(TLFNs.is_file);
    val_ptr::string_bind(fn.argsptr(), path);
    return fn.zbool();
}

htab_rc 
glob(str_ptr wcard, int flags)
{
    fn_params<2> fn(TLFNs.glob);
    zval* pz = fn.argsptr();
    val_ptr::string_bind(pz, wcard);
    ZVAL_LONG(pz+1, flags);

    return fn.array();
}

str_rc 
fgets(val_ptr res, zend_long limit)
{
    fn_params<2> fn(TLFNs.fgets);
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
    return fn.str();
}

str_rc fread(val_ptr fres, int length)
{
    fn_params<2> fn(TLFNs.fread);

    zval *ap = fn.argsptr();

    ZVAL_COPY_VALUE(ap, fres);
    ap++;
    ZVAL_LONG(ap, length);
    return fn.str();
}

val_rc 
fwrite(val_ptr res, str_ptr data, zend_long len)
{
    val_rc result;

    fn_params<3> fn(TLFNs.fwrite);

    zval *ap = fn.argsptr();
    ZVAL_COPY_VALUE(ap, res);
    val_ptr::string_bind(ap+1, data);
    if (len > 0)
    {
        ZVAL_LONG(ap+2, len);
    }
    else 
    {
        ZVAL_NULL(ap+2);
    }
    return fn.mixed();
}

bool
unlink(str_ptr path, val_ptr context)
{
    fn_params<2> fn(TLFNs.unlink);

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
    return fn.zbool();
}

obj_rc 
weakref_create(obj_ptr wref)
{
    fn_params<1> fn(TLFNs.weakref_create);
    ZVAL_OBJ(fn.argsptr(), wref);
    return fn.obj();
}

obj_rc 
weakref_get(obj_ptr wref)
{
    fn_noparams fn(TLFNs.weakref_get);
    fn.set_obj(wref);
    return fn.obj();
}

str_rc 
serialize(val_ptr value)
{
    fn_params<1> fn(TLFNs.serialize);
    ZVAL_COPY_VALUE(fn.argsptr(), value);
    return fn.str();
}


val_rc 
unserialize(str_ptr data, htab_ptr options)
{
    fn_params<2> fn(TLFNs.unserialize);

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
    return fn.mixed();
}

int 
session_status()
{
    fn_noparams fn(TLFNs.session_status);
    return fn.zlong();
}

bool 
session_write_close()
{
    fn_noparams fn(TLFNs.session_write_close);
    return fn.zbool();
}

} // end namespace zpp
//fn_call.cpp
#endif

