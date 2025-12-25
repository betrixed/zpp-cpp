#ifndef PHPALLOC_H
#define PHPALLOC_H

#include <new>

/* This completedly fails to link for the PHP extension loader */
/*
extern void* operator new(std::size_t size) noexcept(false);
extern void  operator delete(void* ptr) noexcept;
extern void* operator new[](std::size_t size) noexcept(false);
extern void  operator delete[](void* ptr) noexcept;
*/

struct PHPAlloc {
public:
    void* operator new(size_t size);

    void operator delete(void* ptr);
};

#endif
