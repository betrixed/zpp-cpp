 /*  
  *  PHP extension C++ classes - zpp 
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  */

#ifndef FN_CALL_CPP
#define FN_CALL_CPP

#ifndef FN_CALL_H
#include "fn_call.h"
#endif

#ifndef HTAB_WALK_H
#include "htab_walk.h"
#endif

namespace zpp {

// static and externals
fntable FTAB;
strtable STAB;


fn_call::fn_call() 
	: argct_(0), argv_(nullptr), autowipe_(false)
{
}

fn_call::~fn_call()
{

}
/*
zval_mgr&& 
fn_call::call1(zval *vp)
{
    assert(argct_==1);
    ZVAL_COPY_VALUE(argv_[0],vp);
    return call_fn();
}

zval_mgr&& 
fn_call::call2(zval *arg0, zval* arg1)
{
    assert(argct_==2);
    ZVAL_COPY_VALUE(argv_[0],arg0);
    ZVAL_COPY_VALUE(argv_[1],arg1);
    return call_fn();
}

zval_mgr&& 
fn_call::call3(zval *arg0, zval* arg1, zval* arg2)
{
    assert(argct_==3);
    ZVAL_COPY_VALUE(argv_[0],arg0);
    ZVAL_COPY_VALUE(argv_[1],arg1);
    ZVAL_COPY_VALUE(argv_[2],arg2);
    return call_fn();
}

zval_mgr&& 
fn_call::call4(zval *arg0, zval* arg1, zval* arg2, zval* arg3)
{
    assert(argct_==4);
    ZVAL_COPY_VALUE(argv_[0],arg0);
    ZVAL_COPY_VALUE(argv_[1],arg1);
    ZVAL_COPY_VALUE(argv_[2],arg2);
    ZVAL_COPY_VALUE(argv_[3],arg3);
    return call_fn();
}

zval_mgr&& 
fn_call::call5(zval *arg0, zval* arg1, zval* arg2, zval* arg3, zval* arg4)
{
     assert(argct_==5);
    ZVAL_COPY_VALUE(argv_[0],arg0);
    ZVAL_COPY_VALUE(argv_[1],arg1);
    ZVAL_COPY_VALUE(argv_[2],arg2);
    ZVAL_COPY_VALUE(argv_[3],arg3);
    ZVAL_COPY_VALUE(argv_[4],arg4);
    return call_fn();
}
*/



/** reset this from the constructor information */
void 
fn_call::set_fci(zend_object* obj, zstr_user method, HashTable* nargs)
{
    fci_ = {0};
    cache_ = {0}; // ensure fully wiped

    //method_name_ = method;

    fci_.size = sizeof(fci_);
    fci_.object = obj;
    //showstr("method", method);
    

    zval *p = &fci_.function_name;
    *p = {0};
    // fci_.function_name is a COPY_VALUE
    ZVAL_STR(p, method);  
    fci_.retval =  (zval*)result_; 
    fci_.param_count = argct_; //  set by constructor
    fci_.params = (zval*) argv_; //  set by constructor
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
fn_call::set_fname(zstr_user name)
{
    set_fci(nullptr, name, nullptr);
}

void fn_call::throw_failed()
{
    zval_user fn(&fci_.function_name);
    zstr_user name(fn.zstr());
    if (name.ok())
        zend_throw_error(zend_ce_error, "fn_call_failed for %s", name.data());
    else 
        zend_throw_error(zend_ce_error, "fn_call_failed, no name");
}

bool fnexists::call(zstr_user arg)
{
    ZVAL_STR(argsptr(), (zend_string*)arg);
    zval_mgr result = call_fn();
    return zval_user(result).zbool();
}

zstr_mgr 
pregquote::call(zstr_user str, zstr_user delimiter)
{
    zval* pz = argsptr();

    ZVAL_STR(pz, (zend_string*) str);
    ZVAL_STR(pz+1, (zend_string*) delimiter);

    zval_mgr result = call_fn();
    return zstr_mgr(zval_user(result).zstr());
}

zstr_mgr 
file_content::call(zstr_user path, int offset, size_t len)
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
    return zstr_mgr(call_fn());
}

zval_mgr
PathInfo::call(zstr_user path, int flags)
{
    zval* pz = argsptr();
    ZVAL_STR(pz, path);
    ZVAL_LONG(pz+1, flags);
    return call_fn();
}



zstr_mgr 
preg_quote(zstr_user expr, zstr_user delimiter)
{
    return FTAB.preg_quote.call(expr, delimiter);
}

zval_mgr&& 
fn_call::call_fn()
{
    if (fci_.size==0)
    {
        zend_throw_error(zend_ce_error,"call_fn() fci is not initialized");
        return std::move(result_);
    }
    /*
        showobj("call_fn(obj)", fci_.object);
        showmem("call_fn()", &fci_.function_name);
        zend_printf("fn handler %lx\n", cache_.function_handler);
        zend_printf("argct %ld params %lx np %lx\n", 
        fci_.param_count, fci_.params, fci_.named_params);
    */

    zend_result ok =  zend_call_function(&fci_, &cache_);

    if (ok != SUCCESS)
    {
        throw_failed();
    }
    //showmem("call_fn result", result_);
    return std::move(result_);
}

zstr_mgr 
addcslashes(zstr_user s, zstr_user escapes)
{
    fn_call_args<2>  fn;
    fn.set_fci(nullptr, STAB.addcslashes, nullptr);
    zval* pz = fn.argsptr();
    ZVAL_STR(pz, (zend_string*) s);
    ZVAL_STR(pz+1, (zend_string*) escapes);
    zstr_mgr result = fn.call_fn();
    return result;
}

 zstr_mgr 
 file_get_contents(zstr_user path, int offset, size_t len)
 {
    //zend_printf("file get contents for %s\n", path.data());

    zstr_mgr result = FTAB.file_get_contents.call(path, offset, len);

    //showmem("contents", result);
    return result;
 }

bool extension_loaded(zstr_user name)
{
    return FTAB.extension_loaded.call(name);
}



bool extnloaded::call(zstr_user name)
{
    ZVAL_STR(argsptr(), (zend_string*) name);
    zval_mgr result = call_fn();
    return zval_user(result).isTrue();
}

bool 
function_exists(zstr_user name)
{
    return FTAB.function_exists.call(name);
}

zstr_mgr 
mb_detect_order(const zval_mgr& encoding)
{
    fn_call_args<1>  fn;

    fn.set_fci(nullptr, STAB.mb_detect_order, nullptr);
    ZVAL_COPY_VALUE(fn.argsptr(), encoding);

    return fn.call_fn();
}
    
zstr_mgr 
mb_detect_encoding(zstr_user str, const zval_mgr& encodings, bool strict)
{
    fn_call_args<3>  fn;

    zval* pz = fn.argsptr();
    ZVAL_STR(pz, (zend_string*) str);
    ZVAL_COPY_VALUE(pz+1, encodings);
    ZVAL_BOOL(pz+2,strict);

    fn.set_fci(nullptr, STAB.mb_detect_encoding, nullptr);
    return fn.call_fn();
}

zstr_mgr 
rawurlencode(zstr_user s)
{
    fn_call_args<1>  fn;
    ZVAL_STR(fn.argsptr(), s);

    fn.set_fci(nullptr, STAB.rawurlencode, nullptr);
    return fn.call_fn();

}

zstr_mgr 
ucwords(zstr_user s)
{
    fn_call_args<1>  fn;
    ZVAL_STR(fn.argsptr(), s);
    fn.set_fci(nullptr, STAB.ucwords, nullptr);
    return fn.call_fn();
}

zstr_mgr
strtr(zstr_user s, zstr_user from, zstr_user to)
{
    fn_call_args<3>  fn;
    zval* ap = fn.argsptr();

    ZVAL_STR(ap, s);
    ZVAL_STR(ap+1, from);
    ZVAL_STR(ap+2, to);

    fn.set_fci(nullptr, STAB.strtr, nullptr);
    return fn.call_fn();
}

zval_mgr 
json_decode(zstr_user str, bool asArray,  int flags)
{
    if (asArray)
    {
        flags |= PHP_JSON_OBJECT_AS_ARRAY;
    }
    zval_mgr result;

    //zend_result check = 
    php_json_decode_ex(result, str.data(), str.size(), flags, 512);

    return result;
}

void  // virtual
fntable::init()
{        
    //zend_printf("fntable init\n");

    s_extension_loaded = "extension_loaded";
    s_function_exists = "function_exists";
    s_preg_quote = "preg_quote";
    s_file_get_contents = "file_get_contents";
    s_pathinfo = "pathinfo";
    s_call_user_func_array = "call_user_func_array";

    extension_loaded.set_fname(s_extension_loaded);
    function_exists.set_fname(s_function_exists);
    preg_quote.set_fname(s_preg_quote);
    file_get_contents.set_fname(s_file_get_contents);
    pathinfo.set_fname(s_pathinfo);
    call_user_func_array.set_fname(s_call_user_func_array);

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




}

/**
 * Dyanmic args setup, array values only
 */
args_spread::args_spread(htab_read args, int prefixct)
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
    zval_mgr& result, 
    zval_mgr& callme, 
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
    zval_mgr& result, 
    zval_mgr& callme, 
    htab_read args)
{
    args_spread spread(args);
    return callable_fn(result, callme, spread.arg_ct(), spread.arg_v());
}

FCall2::FCall2()
{  
}

FCall2::FCall2(zstr_user func)
{
    set_fname(func);
}

zval_mgr  
FCall2::call(zval* arg1, zval* arg2)
{
    ZVAL_COPY_VALUE(&params[0], arg1);
    ZVAL_COPY_VALUE(&params[1], arg2);
    return call_fn();
}


}; // end namespace zpp


//fn_call.cpp
#endif

