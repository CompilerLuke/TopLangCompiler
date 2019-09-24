//
//  allocator.cpp
//  TopLangCompiler
//
//  Created by Lucas Goetz on 20/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//

#include "allocator.h"

namespace top {
    void* MallocAllocator(void* allocator_data, AllocationType type, void* ptr, size_t size, size_t old_size) {
        if (type == AllocationType::Alloc) return malloc(size);
        if (type == AllocationType::Realloc) return realloc(ptr, size);
        if (type == AllocationType::Dealloc) free(ptr);
        return NULL;
    }
}


