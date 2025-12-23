#ifndef ALLOC_NEW_CPP
#define ALLOC_NEW_CPP

extern "C" {
    #include <php.h>
    #include <Zend/zend.h>  
	#include <Zend/zend_alloc.h>
}

// must be in global namespace
#define CONCATENATE(s1, s2) s1##s2
#define EXPAND_THEN_CONCATENATE(s1, s2) CONCATENATE(s1, s2)
#define UNIQUE_IDENTIFIER(prefix) EXPAND_THEN_CONCATENATE(prefix, __LINE__)

#define DETECT_NAMESPACE \
struct UNIQUE_IDENTIFIER(namespace_detector_on_line_) { \
    void f() { look_at_the_class_name_above = 0; } \
};

//DETECT_NAMESPACE

void*
operator new(std::size_t size) noexcept(false)
{
	zend_printf("global op new called, size = %zu\n", size);
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
	zend_printf("global op new[] called, size = %zu\n", size);
	if (size==0)
		size++;
	return emalloc(size);
}

void operator delete[](void* ptr) noexcept
{
   efree(ptr);
}


#endif