 /*  
  *  PHP extension C++ classes - zpp 
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  */

#ifndef FN_CALL_CPP
#define FN_CALL_CPP

#ifndef FN_CALL_H
#include "zpp/fn_call.h"
#endif

namespace zpp {

// static and externals
fntable FTAB;
strtable STAB;


fn_call::fn_call() 
	: argct_(0), argv_(nullptr), track_(false)
{
}

fn_call::~fn_call()
{

}

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



void 
fn_call::set_arg(size_t ix, zval* vp)
{
    if ((ix < 1) || (ix > argct_))
    {
    	zend_throw_error(zend_ce_exception, "zpp::fn_call index %ld is out of bounds ", ix);
    }
    zval* p = argv_[ix-1];
    ZVAL_COPY_VALUE(p,vp);
}

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
        zend_throw_error(zend_ce_error, "fn_call_failed, no name", 0);
}

bool fnexists::call(zstr_user arg)
{
    zval_mgr val(arg);
    zval_mgr result = call1(val);
    return zval_user(result).zbool();
}

zstr_mgr 
pregquote::call(zstr_user str, zstr_user delimiter)
{
    zval_mgr sval(str);
    zval_mgr dval(delimiter);
    zval_mgr result = call2(sval, dval);
    return zstr_mgr(zval_user(result).zstr());
}

zstr_mgr 
file_content::call(zstr_user path, int offset, size_t len)
{
    //showmem("fn_name", &fci_.function_name);

    zval_mgr zoff(offset);
    zval_mgr zlen;
    zval_mgr zpath(path);

    if (len > 0)
    {
        zlen = (zend_long)len; // else null;
    }
    zval_mgr zfalse;
    zfalse.set_bool(false);

    zval_mgr resource;

    return zstr_mgr(call5(zpath, zfalse, resource, zoff, zlen));
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
        zend_throw_error(zend_ce_error,"call_fn() fci is not initialized",0);
        return std::move(result_);
    }
    if (track_)
    {
        //showobj("call_fn(obj)", fci_.object);
        //showmem("call_fn()", &fci_.function_name);
        //zend_printf("fn handler %lx\n", cache_.function_handler);
        //zend_printf("argct %ld params %lx np %lx\n", 
        //   fci_.param_count, fci_.params, fci_.named_params);

    }
    zend_result ok =  zend_call_function(&fci_, &cache_);

    if (ok != SUCCESS)
    {
        throw_failed();
    }

    return std::move(result_);
}

zstr_mgr 
addcslashes(zstr_user s, zstr_user escapes)
{
    zval_mgr arg1(s);
    zval_mgr arg2(escapes);

    fn_call_args<2>  fn;

    fn.set_fci(nullptr, STAB.addcslashes, nullptr);
    zstr_mgr result = fn.call2(arg1,arg2);
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
    zval_mgr zname(name);
    zval_mgr result = call1(zname);
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
    zval_mgr result(fn.call1(encoding));
    return result;
}
    
zstr_mgr 
mb_detect_encoding(zstr_user str, const zval_mgr& encodings, bool strict)
{
    fn_call_args<3>  fn;

    zval_mgr arg1(str);
    zval_mgr arg3;
    arg3.set_bool(strict);

    fn.set_fci(nullptr, STAB.mb_detect_encoding, nullptr);
    return fn.call3(arg1, encodings, arg3);
}

zstr_mgr 
rawurlencode(zstr_user s)
{
    fn_call_args<1>  fn;
    zval_mgr arg1(s);
    fn.set_fci(nullptr, STAB.rawurlencode, nullptr);
    fn.set_arg(1,arg1);
    zval_mgr result = fn.call_fn();
    return result;

}

void  // virtual
fntable::init()
{        
    //zend_printf("fntable init\n");

    s_extension_loaded = zstr_intern("extension_loaded");
    s_function_exists = zstr_intern("function_exists");
    s_preg_quote = zstr_intern("preg_quote");
    s_file_get_contents = zstr_intern("file_get_contents");

    extension_loaded.set_fname(s_extension_loaded);
    function_exists.set_fname(s_function_exists);
    preg_quote.set_fname(s_preg_quote);
    file_get_contents.set_fname(s_file_get_contents);
}

void  // virtual
strtable::init()
{
    construct_key = zstr_intern("__construct");
    mb_detect_order = zstr_intern("mb_detect_order");
    mb_detect_encoding = zstr_intern("mb_detect_encoding");
    setdate = zstr_intern("setdate");
    settime = zstr_intern("settime");
    diff = zstr_intern("diff");
    date = zstr_intern("date");
    strtotime = zstr_intern("strtotime");
    addcslashes = zstr_intern("addcslashes");
    rawurlencode = zstr_intern("rawurlencode");

}

args_spread::args_spread(htab_user args)
{
    argct_ = args.size();
    if (argct_)
    {
        argv_ = new zval_init[argct_];
        // general but inefficient
        htab_walk wk;
        auto value = wk.value();
        size_t ix = 0;

        for(wk.start(args); wk.ok(); wk.next(), ix++)
        {
            ZVAL_COPY_VALUE(argv_[ix], value);
        }
    }
    else {
        argv_ = nullptr;
    }
    
}

args_spread::~args_spread()
{
    if (argv_) {
        delete[] argv_;
    }
}


bool callable_fn(zval_mgr& result, zval_mgr& callme, int argct, zval* argv)
{
    result.set_null();

    zval*    pzobj = nullptr;

    showmem("argv", argv);
    showmem("result", result);

    if (call_user_function( 
        CG(function_table), 
        (zval*) nullptr, 
        (zval*) callme, 
        (zval*) result, 
        argct, 
        argv) != SUCCESS)
    {
        // TODO: exception message callme toString ??
        zend_throw_error(zend_ce_error, "Invalid callable", 0);
        return false;
    }
     showmem("result", result);
    return true;
}



bool 
call_spread_fn(zval_mgr& result, zval_mgr& callme, htab_user args)
{
    args_spread spread(args);
    return callable_fn(result, callme, spread.arg_ct(), spread.arg_v());
}

}; // end namespace zpp


//fn_call.cpp
#endif

