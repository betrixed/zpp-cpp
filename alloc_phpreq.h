#ifndef ALLOC_PHPREQ_H
#define ALLOC_PHPREQ_H

#include <vector>

extern "C" {
    #include <php.h>
    #include <Zend/zend.h>  
	#include <Zend/zend_alloc.h>
}
/**
 *  Allocator for C++ STL that uses PHP request memory allocations, emalloc and efree.
 *  C++ compilers don't report on what is missing from allocator templates.
 *  Thanks to 
 *  https://www.codeproject.com/articles/A-Custom-STL-std-allocator-Replacement-Improves-Performance
 */

template <typename T>
class alloc_phpreq {
public:
	typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;

	alloc_phpreq(){}
    ~alloc_phpreq(){}

    template <class U> struct rebind { typedef alloc_phpreq<U> other; };

	template <class U>alloc_phpreq(const alloc_phpreq<U>&) {}

    pointer address(reference x) const {return &x;}
    const_pointer address(const_reference x) const {return &x;}
    size_type max_size() const throw() {return size_t(-1) / sizeof(value_type);}

	pointer allocate(size_type n, alloc_phpreq<T>::const_pointer hint = 0)
    {
        return static_cast<pointer>(emalloc(n*sizeof(T)));
    }

	void deallocate(pointer p, size_type n)
    {
        efree(p);
    }

	void construct(pointer p, const T& val)
    {
        new(static_cast<void*>(p)) T(val);
    }

    void construct(pointer p)
    {
        new(static_cast<void*>(p)) T();
    }

    void destroy(pointer p)
    {
        p->~T();
    }
};

template <typename T, typename U>
bool operator==(const alloc_phpreq<T>&, const alloc_phpreq<U>&) {
    return true;
}

template <typename T, typename U>
bool operator!=(const alloc_phpreq<T>&, const alloc_phpreq<U>&) {
    return false;
}

class PHPAlloc {
public:
    void* operator new(size_t size)
    {
        //zend_printf("new DStack %ld\n",size);
        return emalloc(size);
    }

    void operator delete(void* ptr)
    {
        //zend_printf("delete DStack %lx\n",ptr);
        efree(ptr);
    }
};

#endif