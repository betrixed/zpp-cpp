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

zval_own&& 
fn_call::call1(zval *vp)
{
    assert(argct_==1);
    ZVAL_COPY_VALUE(argv_+0,vp);
    return call_fn();
}

zval_own&& 
fn_call::call2(zval *arg0, zval* arg1)
{
    assert(argct_==2);
    ZVAL_COPY_VALUE(argv_+0,arg0);
    ZVAL_COPY_VALUE(argv_+1,arg1);
    return call_fn();
}

zval_own&& 
fn_call::call3(zval *arg0, zval* arg1, zval* arg2)
{
     assert(argct_==3);
    ZVAL_COPY_VALUE(argv_+0,arg0);
    ZVAL_COPY_VALUE(argv_+1,arg1);
    ZVAL_COPY_VALUE(argv_+2,arg2);
    return call_fn();
}

zval_own&& 
fn_call::call4(zval *arg0, zval* arg1, zval* arg2, zval* arg3)
{
    assert(argct_==4);
    ZVAL_COPY_VALUE(argv_+0,arg0);
    ZVAL_COPY_VALUE(argv_+1,arg1);
    ZVAL_COPY_VALUE(argv_+2,arg2);
    ZVAL_COPY_VALUE(argv_+3,arg3);
    return call_fn();
}

zval_own&& 
fn_call::call5(zval *arg0, zval* arg1, zval* arg2, zval* arg3, zval* arg4)
{
     assert(argct_==5);
    ZVAL_COPY_VALUE(argv_+0,arg0);
    ZVAL_COPY_VALUE(argv_+1,arg1);
    ZVAL_COPY_VALUE(argv_+2,arg2);
    ZVAL_COPY_VALUE(argv_+3,arg3);
    ZVAL_COPY_VALUE(argv_+4,arg4);
    return call_fn();
}



void 
fn_call::set_arg(size_t ix, zval* vp)
{
    if ((ix < 1) || (ix > argct_))
    {
    	zend_throw_error(zend_ce_exception, "zpp::fn_call index %ld is out of bounds ", ix);
    }
    zval* p = argv_ + (ix-1);
    ZVAL_COPY_VALUE(p,vp);
}

/** reset this from the constructor information */
void 
fn_call::set_fci(zend_object* obj, const zstr_base& method, HashTable* nargs)
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
    ZVAL_STR(p, method.ptr());  
    fci_.retval =  result_.ptr(); 
    fci_.param_count = argct_; //  set by constructor
    fci_.params = argv_; //  set by constructor
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
fn_call::set_fname(const zstr_base& name)
{
    set_fci(nullptr, name, nullptr);
}

void fn_call::throw_failed()
{
    zstr_ptr name(&fci_.function_name);
    zend_throw_error(zend_ce_error, "fn_call_failed for %s", name.data());
}

bool fnexists::call(const zval_own& arg)
{
    return args1(arg);
}

zstr_own 
pregquote::call(const zval_own& arg1, const zval_own& arg2)
{
    return args2(arg1, arg2);
}

zstr_own 
file_content::call(const zval_own& path, int offset, size_t len)
{
    //showmem("fn_name", &fci_.function_name);

    zval_own zoff(offset);
    zval_own zlen;
    if (len > 0)
    {
        zlen.set((zend_long)len); // else null;
    }
    zval_own zfalse;
    zfalse.setbool(false);

    zval_own resource;

    return zstr_own(args5(path, zfalse, resource, zoff, zlen));
}

zstr_own 
preg_quote(const zstr_base& expr, const zstr_base& delimiter)
{
    return FTAB.preg_quote.call(expr, delimiter);
}

zval_own&& 
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

zstr_own 
addcslashes(const zstr_base& s, const zstr_base& escapes)
{
    zstr_temp fname("addcslashes");
    zval_own arg1(s);
    zval_own arg2(escapes);

    fn_call_args<2>  fn;

    fn.set_fci(nullptr, fname, nullptr);
    fn.args2(arg1,arg2);
    zval_own result = fn.call_fn();
    return zstr_pass(result.zstr());
}

 zstr_own 
 file_get_contents(const zstr_base& path, int offset, size_t len)
 {
    //zend_printf("file get contents for %s\n", path.data());

    zval_own result = FTAB.file_get_contents.call(path, offset, len);

    //showmem("contents", result);
    return zstr_own(std::move(result));
 }

bool extension_loaded(const zstr_base& name)
{
    return FTAB.extension_loaded.call(name);
}


bool extnloaded::call(const zval_own& name)
{
    args1(name);
    return call_fn().isTrue();
}

bool 
function_exists(const zstr_base& name)
{
    return FTAB.function_exists.call(name);
}

zval_own 
mb_detect_order(const zval_own& encoding)
{
    fn_call_args<1>  fn;

    fn.set_fci(nullptr, STAB.mb_detect_order, nullptr);
    fn.args1(encoding);
    return fn.call_fn();
}
    
zval_own 
mb_detect_encoding(const zstr_base& str, const zval_own& encodings, bool strict)
{
    fn_call_args<3>  fn;

    zval_own arg1(str);
    zval_own arg3;
    arg3.setbool(strict);

    fn.set_fci(nullptr, STAB.mb_detect_encoding, nullptr);
    fn.args3(arg1, encodings, arg3);
    return fn.call_fn();
}

zstr_own 
rawurlencode(const zstr_base& s)
{
    fn_call_args<1>  fn;
    zstr_temp fname("rawurlencode");
    zval_own arg1(s);
    fn.set_fci(nullptr, fname, nullptr);
    fn.set_arg(1,arg1);
    zval_own result = fn.call_fn();
    return zstr_pass(result.zstr());

}

void  // virtual
fntable::init()
{        
    //zend_printf("fntable init\n");

    s_extension_loaded = zstr_perm("extension_loaded");
    s_function_exists = zstr_perm("function_exists");
    s_preg_quote = zstr_perm("preg_quote");
    s_file_get_contents = zstr_perm("file_get_contents");

    extension_loaded.set_fname(s_extension_loaded);
    function_exists.set_fname(s_function_exists);
    preg_quote.set_fname(s_preg_quote);
    file_get_contents.set_fname(s_file_get_contents);
}

void  // virtual
strtable::init()
{
    construct_key = zstr_perm("__construct");
    mb_detect_order = zstr_perm("mb_detect_order");
    mb_detect_encoding = zstr_perm("mb_detect_encoding");
    setdate = zstr_perm("setdate");
    settime = zstr_perm("settime");
    diff = zstr_perm("diff");
    date = zstr_perm("date");
    strtotime = zstr_perm("strtotime");

}

args_spread::args_spread(htab_ptr args)
{
    argct_ = args.size();
    if (argct_)
    {
        argv_ = new zval_init[argct_];
        // general but inefficient
        htab_walk wk;
        auto& value = wk.value();
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


bool callable_fn(zval_own& result, zval_own& callme, int argct, zval* argv)
{
    result.set_null();

    zval*    pzobj = nullptr;

    if (call_user_function( 
        CG(function_table), 
        (zval*) nullptr, 
        (zval*) callme.ptr(), 
        (zval*) result.ptr(), 
        argct, 
        argv) != SUCCESS)
    {
        // TODO: exception message callme toString ??
        zend_throw_error(zend_ce_error, "Invalid callable", 0);
        return false;
    }
    return true;
}



bool call_spread_fn(zval_own& result, zval_own& callme, htab_ptr args)
{
    args_spread spread(args);
    return callable_fn(result, callme, spread.arg_ct(), spread.arg_v());
}

}; // end namespace zpp


//fn_call.cpp
#endif

