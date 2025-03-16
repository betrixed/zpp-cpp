#ifndef GLOBALS_H
#define GLOBALS_H

/**
 *  Class definitions of Global and Globals are adapted from PHP-CPP
 */
#ifndef GLOBAL_H
#include "global.h"
#endif

#ifndef GLOBALS_H
#include "globals.h"
#endif

namespace zpp {

class Global;

/**
 * Access the special $GLOBALS array.
 * to set, or get a string keyed value
 */
class Globals
{
public:
    /**
     *  Disable copy and move operations
     */
    Globals(const Globals &globals) = delete;
    Globals(Globals &&globals) = delete;

    virtual ~Globals() {}

    /**
     *  Get access to a global variable
     *  @param  name
     *  @return Global
     */


    Global operator[](zstr_user name);

private:
    /**
     *  Constructor
     */
    Globals() {}

public:
    /**
     *  Get the one and only instance
     *  @return Globals
     */
    static Globals &instance();
};

/**
 *  We always have one instance of the GLOBALS instance
 *  @var    Globals
 */
extern Globals &GLOBALS;


}; // namespace

#endif
