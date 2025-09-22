 /*  
  *  PHP extension C++ classes - zpp 
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  *  @license Artistic License 2.0  
  *  @file zpp/state_init.cpp
  *  @brief Base class for module and request initialization and shutdown.
  *  @details
  *  Derive from this class to get module and request init and shutdown calls.
  *  The constructor links the instance into a static list.
  *  The static methods init_all(), end_all(), init_request(), end_request()
  *  walk the list and call the virtual methods init(), end(), init_req(), end
  */

#ifndef STATE_INIT_CPP
#define STATE_INIT_CPP

#ifndef STATE_INIT_H
#include "state_init.h"
#endif

namespace zpp {

// static and externals

state_init* state_init::first_ = nullptr;
state_init* state_init::last_ = nullptr;

state_init::state_init() : next_((state_init*)nullptr)
{
    //zend_printf("state_init\n");
    if (first_ == nullptr)
    {
        first_ = this;
        last_ = this;
    }
    else {
        last_->next_ = this;
        last_ = this;
    }
}


int //static
state_init::init_all()
{
    #ifdef DEBUG_EXTRA
    //    zend_printf("init_all\n");
    #endif
    int result = 0;
    state_init* link = state_init::first_;

    while(link)
    {
        link->init();
        result++;
        link = link->next_;
    }
    return result;
}

void //static
state_init::end_all()
{
    #ifdef DEBUG_XTRA
    //    zend_printf("end_all\n");
    #endif
    state_init* link = state_init::first_;
    while(link)
    {
        state_init* temp = link;
        link = link->next_;
        temp->end();
    }
}

void //virtual
state_init::init()
{
   //zend_printf("static first_ %lx ", (long unsigned) &first_);
   //zend_printf("Init in module %lx type %s\n", (long unsigned) this, typeid(*this).name());
}

void //virtual
state_init::end()
{
}


// request start -  end;
void //static
state_init::init_request()
{
    #ifdef DEBUG_EXTRA
    //    zend_printf("init_request\n");
    #endif

    state_init* link = state_init::first_;
    while(link)
    {
        link->init_req();
        link = link->next_;
    }
}

void //static
state_init::end_request()
{
    #ifdef DEBUG_EXTRA
    //    zend_printf("end_request\n");
    #endif
    state_init* link = state_init::first_;
    while(link)
    {
        state_init* temp = link;
        link = link->next_;
        temp->end_req();
    }
}

void //virtual
state_init::init_req()
{
}

void //virtual
state_init::end_req()
{
}

/* not expecting any work here */
//virtual
state_init::~state_init()
{
}




}; // namespace
#endif
