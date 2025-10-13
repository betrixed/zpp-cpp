#ifndef ALLOC_PHPREQ_H
#define ALLOC_PHPREQ_H

#include <vector>

extern "C" {
    #include <php.h>
    #include <Zend/zend.h>  
	#include <Zend/zend_alloc.h>
}

template <typename T>
class alloc_phpreq {
public:
	using value_type = T;

	alloc_phpreq() = default;

	template <typename U>
    alloc_phpreq(const alloc_phpreq<U>&) {}

	T* allocate(std::size_t n) {
		size_t sz = n * sizeof(T);
		T* data = static_cast<T*>(emalloc(sz));
		//zend_printf("Allocated %ld, %lx\n", sz, data);
    	return data;
	}

	T* deallocate(T* p, std::size_t n) noexcept {
		//zend_printf("Free %lx fsize %ld\n", p, n);
    	efree(p);
	}

	template <typename U, typename... Args>
    void construct(U* p, Args&&... args)
    {
        ::new ((void*)p) U(std::forward<Args>(args)...);
    }

    template <typename U>
    void destroy(U* p) noexcept
    {
        if (p) 
        {
            p->~U();
        }
    }


	template <typename U>
    struct rebind {
        using other = alloc_phpreq<U>;
    };
};

template <typename T, typename U>
bool operator==(const alloc_phpreq<T>&, const alloc_phpreq<U>&) {
    return true;
}

template <typename T, typename U>
bool operator!=(const alloc_phpreq<T>&, const alloc_phpreq<U>&) {
    return false;
}

#endif