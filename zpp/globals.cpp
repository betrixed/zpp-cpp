#ifndef GLOBALS_CPP
#define GLOBALS_CPP

#ifndef GLOBALS_H
#include "globals.h"
#endif

#ifndef GLOBAL_H
#include "global.h"
#endif

/**
 *  Namespace
 */
namespace zpp {

/**
 *  Get access to the globals single instance
 *  @return Globals
 */
Globals &Globals::instance()
{
    static Globals globals;
    return globals;
}

/**
 *  The one and only instance
 *  @var    Globals
 */
Globals &GLOBALS = Globals::instance();

/**
 *  Get access to a global variable
 *  @param  name
 *  @return Global
 */


Global Globals::operator[](zstr_user key)
{
    //showstr("string key &", key);

    zval *varvalue = zend_hash_find_ind(&EG(symbol_table), key);

    // check if the variable already exists
    if (!varvalue)
    {
        // the variable does not already exist, return a global object
        // that will automatically set the value when it is updated
        return Global(key);
    }
    else
    {
        // we are in the happy situation that the variable exists, we turn
        // this value into a reference value, and return that
        return Global(key, varvalue);
    }   
}


};//namespace

#endif
//globals.cpp


