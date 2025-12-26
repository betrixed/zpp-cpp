#ifndef PHPALLOC_CPP
#define PHPALLOC_CPP

//#include <cstdlib>
//#include <print>

#ifndef PHPALLOC_H
#include "phpalloc.h"
#endif

#include <cstddef>



#define CONCATENATE(s1, s2) s1##s2
#define EXPAND_THEN_CONCATENATE(s1, s2) CONCATENATE(s1, s2)
#define UNIQUE_IDENTIFIER(prefix) EXPAND_THEN_CONCATENATE(prefix, __LINE__)

#define DETECT_NAMESPACE \
struct UNIQUE_IDENTIFIER(namespace_detector_on_line_) { \
    void f() { look_at_the_class_name_above = 0; } \
};

//DETECT_NAMESPACE

/* global alloc operators fail to be linked and called by PHP extensions loader */

/*

// must be in global namespace


void*
operator new(std::size_t size) noexcept(false)
{
	zend_write("global op new\n", sizeof("global op new\n"));
	return emalloc(size);
} 

void 
operator delete(void* ptr) noexcept
{
    efree(ptr);
}

void*
operator new[](std::size_t size) noexcept(false)
{
	zend_write("global op new[]\n", sizeof("global op new[]\n"));
	if (size==0)
		size++;
	return emalloc(size);
}

void operator delete[](void* ptr) noexcept
{
   efree(ptr);
}
*/


void* 
PHPAlloc::operator new(size_t size)
{
    //zend_printf("PHPAlloc new\n");
    return emalloc(size);
}

void 
PHPAlloc::operator delete(void* ptr)
{
    //zend_printf("PHPAlloc delete\n");
    efree(ptr);
}


#endif