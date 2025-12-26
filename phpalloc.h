#ifndef PHPALLOC_H
#define PHPALLOC_H

#include <new>

extern "C" {
    
    /* #include <php.h>
    #include <Zend/zend.h>  
    #include <Zend/zend_alloc.h>

    */

    /** Somewhere in PHP headers */
    /** is found most the awful use of C-Macro definitions */

    #ifdef _emalloc
    #undef _emalloc
    #endif

    extern void* _emalloc(size_t);
    extern void  _efree(void* ptr);
}

struct PHPAlloc {
public:
    void* operator new(size_t size);

    void operator delete(void* ptr);
};

#endif
