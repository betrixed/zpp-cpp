 /*  
  *  PHP extension C++ classes - zpp 
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  *  @license BSD 3-Clause License  
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

//#define DEBUG_INIT_ORDER

#ifdef DEBUG_INIT_ORDER
#include <cxxabi.h>  // For abi::__cxa_demangle
#endif

#define STRINGIZE(s) XSTR(s)
#define XSTR(s)  #s

namespace zpp {

// static and externals
//** Just once per compilation unit == extension binary */
 
state_list::state_list(const char* name)
: first_(nullptr), last_(nullptr), name_(name) 
{
}

void state_list::add_si(state_init *item)
{
    item->next_ = nullptr;
    if (last_)
    {
        last_->next_ = item;
    }
    else {
        first_ = item;
    }
    last_ = item;
}

void  
state_list::call_mod_init()
{
    error_return bad;

    #ifdef DEBUG_INIT_ORDER
        char outputbuf[100];
        size_t        iolen;
        int           status;

        zend_printf("List Init %s\n", name_);
    #endif
    state_init* link = first_;
    while(link)
    {
        state_init* si = link;

        #ifdef DEBUG_INIT_ORDER 
            iolen = 100;
            abi::__cxa_demangle(typeid(*si).name(), outputbuf, &iolen, &status);
            if (status==0)
            {
                //outputbuf[iolen] = 0x0;
                zend_printf("typeid %s\n", outputbuf);
            }
        #endif
        link = si->next_;
        if (si->registered_) 
        {
            // this can happen with dependencies, eg routeset, requires route_data::init
            //bad.error() << typeid(*si).name()  << " already registered\n";
        }
        else {
            si->init();
            si->registered_ = true;
        }
    }
    bad.throw_errors();

}

void  state_list::call_mod_end()
{
    state_init* link = first_;
    while(link)
    {
        state_init* si = link;
        link = si->next_;
        si->end();
    }
}

void  state_list::call_req_init()
{
    state_init* link = first_;
    while(link)
    {
        state_init* si = link;
        link = si->next_;
        si->init_req();
    }
}

void  state_list::call_req_end()
{
    state_init* link = first_;
    while(link)
    {
        state_init* si = link;
        link = si->next_;
        si->end_req();
    }
}

state_init::state_init() 
    : next_((state_init*)nullptr)
    , registered_(false)
{
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



void //virtual
state_init::init_req()
{
}

void //virtual
state_init::end_req()
{
}

state_init::~state_init()
{
}




}; // namespace
#endif
