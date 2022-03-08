#pragma once

#include <stdint.h>
#include <cstddef>
#include <assert.h>
#include "SpectreDef.h"

#if defined(WIN32)
#include <malloc.h>
#endif


BEGIN_NAMESPACE_SPECTRE

inline void* aligned_alloc(size_t size, size_t align) noexcept 
{
    assert(align && !(align & align - 1));

    void* p = nullptr;

    // must be a power of two and >= sizeof(void*)
    while (align < sizeof(void*)) {
        align <<= 1u;
    }

#if defined(_WIN32)
    p = ::_aligned_malloc(size, align);
#else
    ::posix_memalign(&p, align, size);
#endif
    return p;
}

inline void* aligned_realloc(void* block,size_t size, size_t align) noexcept
{
	assert(align && !(align & align - 1));

	void* p = nullptr;

	// must be a power of two and >= sizeof(void*)
	while (align < sizeof(void*)) {
		align <<= 1u;
	}

#if defined(_WIN32)
	p = ::_aligned_realloc(block,size, align);
#else
	::posix_memalign(&p, align, size);
#endif
	return p;
}


inline void aligned_free(void* p) noexcept
{
#if defined(WIN32)
    ::_aligned_free(p);
#else
    ::free(p);
#endif
}

inline void* heap_alloc(size_t size, size_t align = alignof(std::max_align_t)) noexcept
{
	return aligned_alloc(size, align);
}

inline void* heap_realloc(void* block,size_t size, size_t align = alignof(std::max_align_t)) noexcept
{
	return aligned_realloc(block,size, align);
}


inline void free(void* p) noexcept
{
    aligned_free(p);
}

END_NAMESPACE_SPECTRE