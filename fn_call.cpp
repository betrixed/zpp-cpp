 /** 
 *   @file fn_call.cpp
  *  @brief Implementation of fn_call class for calling PHP functions from C++
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2025 Michael Rynn
    *  @license Artistic License 2.0
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

namespace zpp {

// static and externals
fntable FTAB;
strtable STAB;


fn_call::fn_call() 
{
    fci_ = {0};
    cache_ = {0};  
    fci_.retval = (zval*) result_;
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
    result_.set_null(); 
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
    //showstr("method", method);

    zval *p = &fci_.function_name;
    *p = {0};
    // fci_.function_name is a COPY_VALUE
    ZVAL_STR(p, method);  

    fci_.named_params = nargs; 
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
    result_.set_null();
}

bool fnexists::call(str_ptr arg)
{
    ZVAL_STR(argsptr(), (zend_string*)arg);
    val_rc result = call_fn();
    return val_ptr(result).zbool();
}

str_rc 
fn_dirname::call(str_ptr name)
{
    ZVAL_STR(argsptr(), name);
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

bool 
fn_fclose::call(val_ptr fres)
{
    ZVAL_COPY_VALUE(argsptr(), fres);
    val_rc result = call_fn();
    return val_ptr(result).isTrue();
}


val_rc 
fn_fopen::call(str_ptr path, str_ptr mode)
{
    zval* pz = argsptr();

    ZVAL_STR(pz, (zend_string*) path);
    ZVAL_STR(pz+1, (zend_string*) mode);
    return call_fn();
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


val_rc
array_pop(val_rc& array_ref)
{
    fn_call_args<1> fn;

    fn.set_fname(STAB.array_pop);
    htab_rw hw(array_ref); // must be writable
    array_ref.make_ref(); // must be reference
    ZVAL_COPY_VALUE(fn.argsptr(), array_ref);
    return fn.call_fn();
}

str_rc 
preg_quote(str_ptr expr, str_ptr delimiter)
{
    return FTAB.preg_quote.call(expr, delimiter);
}

val_rc&&
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
    //showmem("call_fn result", result_);
    return std::move(result_);
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

    str_rc result = FTAB.file_get_contents.call(path, offset, len);

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
    return FTAB.extension_loaded.call(name);
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

bool 
fn_defined::call(str_ptr name)
{
    ZVAL_STR(argsptr(), name);
    val_rc result = call_fn();
    return result.isTrue();
}

void 
fn_define::call(str_ptr name, val_ptr value)
{
    zval* args = argsptr();
    ZVAL_STR(args, name);
    ZVAL_COPY_VALUE(args+1, value);

    call_fn();
}

val_rc 
fn_fgetcsv::call(val_ptr file_res)
{
    ZVAL_COPY_VALUE(argsptr(), file_res);
    return call_fn();
}

bool extnloaded::call(str_ptr name)
{
    ZVAL_STR(argsptr(), (zend_string*) name);
    val_rc result = call_fn();
    return val_ptr(result).isTrue();
}

val_rc 
constant(str_ptr name)
{
    return FTAB.get_constant.call(name);
}

bool 
function_exists(str_ptr name)
{
    return FTAB.function_exists.call(name);
}

str_rc 
dirname(str_ptr path)
{
    return FTAB.get_dirname.call(path);
}

bool 
defined(str_ptr name)
{
    return FTAB.defined.call(name);
}

void 
define(str_ptr name, val_ptr value)
{
    FTAB.define.call(name, value);
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

void  // virtual
fntable::init()
{        
    zend_printf("fntable init\n");

    s_extension_loaded = "extension_loaded";
    s_function_exists = "function_exists";
    s_preg_quote = "preg_quote";
    s_file_get_contents = "file_get_contents";
    s_pathinfo = "pathinfo";
    s_call_user_func_array = "call_user_func_array";
    s_fgetcsv = "fgetcsv";
    s_fopen = "fopen";
    s_fclose = "fclose";
    s_constant = "constant";
    s_dirname = "dirname";
    s_getcwd = "getcwd";
    s_defined = "defined";
    s_define = "define";

    extension_loaded.set_fname(s_extension_loaded);
    function_exists.set_fname(s_function_exists);
    preg_quote.set_fname(s_preg_quote);
    file_get_contents.set_fname(s_file_get_contents);
    pathinfo.set_fname(s_pathinfo);
    fopen.set_fname(s_fopen);
    fclose.set_fname(s_fclose);
    defined.set_fname(s_defined);
    define.set_fname(s_define);

    get_constant.set_fname(s_constant);
    zend_printf("Set constant fn\n");
    get_dirname.set_fname(s_dirname);
    call_user_func_array.set_fname(s_call_user_func_array);

    state_init::init();

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
    result.set_null();

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
     //showmem("result", result);
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


}; // end namespace zpp


//fn_call.cpp
#endif

