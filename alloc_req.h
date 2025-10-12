#ifndef ALLOC_REQ_H
#define ALLOC_REQ_H

#include <vector>

extern "C" {
    #include <php.h>
    #include <Zend/zend.h>  
}

template <typename T>
class alloc_rq_vec {
	using value_type = T;

	alloc_rq_vec() = default;

	template <typename U>
    alloc_rq_vec(const alloc_rq_vec<U>&) {}

	T* allocate(std::size_t n) {
    	return static_cast<T*>(emalloc(n * sizeofT));
	}

	T* deallocate(T* p, std::size_t n) noexcept {
    	efree(p);
	}

	template <typename U>
    struct rebind {
        using other = alloc_rq_vec<U>;
    };
};

template <typename T, typename U>
bool operator==(const alloc_rq_vec<T>&, const alloc_rq_vec<U>&) {
    return true;
}

template <typename T, typename U>
bool operator!=(const alloc_rq_vec<T>&, const alloc_rq_vec<U>&) {
    return false;
}

#endif