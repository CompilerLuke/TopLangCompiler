//
//  allocator.hpp
//  TopLangCompiler
//
//  Created by Lucas Goetz on 20/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//

#pragma once

#include <stdlib.h>

namespace top {
    enum class AllocationType { Alloc, Realloc, Dealloc };
    
    typedef void* (*Allocator)(void* allocator_data, AllocationType, void* ptr, size_t size, size_t old_size);
    
    void* MallocAllocator(void* allocator_data, AllocationType type, void* ptr, size_t size, size_t old_size);
    
    template<typename T>
    inline T* alloc(Allocator allocator, void* allocator_data, unsigned int num = 1) {
        return (T*)allocator(allocator_data, AllocationType::Alloc, NULL, sizeof(T) * num, 0);
    }
    
    template<typename T>
    inline T* resize(Allocator allocator, void* allocator_data, T* ptr, unsigned int num, unsigned int old_num) {
        return (T*)allocator(allocator_data, AllocationType::Realloc, ptr, sizeof(T) * num, sizeof(T) * old_num);
    }
    
    template<typename T>
    inline void dealloc(Allocator allocator, void* allocator_data, T* ptr, unsigned int num = 1) {
        allocator(allocator_data, AllocationType::Dealloc, ptr, sizeof(T) * num, 0);
    }
};
