#ifndef GLOBAL_CPP
#define GLOBAL_CPP

#include "zpp/global.h"


namespace zpp {

/**
 *  Move constructor
 *  @param  global
 */
Global::Global(Global &&global) noexcept :
    gval_(global.gval_,true),
    name_(global.name_),
    exists_(global.exists_)
{
    //zend_printf("Global( g&&)");
}

/**
 *  Constructor for non-existing var
 *
 *  @param  name    Name for the variable that does not exist
 */

Global::Global(zstr_user name) :
    gval_(),
    name_(name),
    exists_(false) 
{}


Global::Global(zstr_user name, zval_user val) :
    gval_((zval*)val, true),
    name_(name),
    exists_(true) {}

/**
 *  Destructor
 */
Global::~Global()
{
    //showmem("~Global gval_", gval_);
    //showstr("~Global name",  name_);
}

/**
 *  Function that is called when the value is updated
 *  @return Value
 */
Global&  Global::update()
{
    // skip if the variable already exists
    if (exists_) return *this;

    // avoid reference a count calamity
    gval_.addref(); // necessary
    name_.addref(); // also necessary
    
    // make it exist in real $GLOBALS

    zend_symtable_update_ind(&EG(symbol_table), name_, gval_);

    exists_ = true;

    return *this;
}


};//namespace

#endif
//global.cpp

