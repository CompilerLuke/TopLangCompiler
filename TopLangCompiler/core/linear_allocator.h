//
//  linear_allocator.hpp
//  TopLangCompiler
//
//  Created by Lucas Goetz on 21/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//

#pragma once

#include "allocator.h"
#include "array.h"

namespace top {
    struct LinearAllocator {
        unsigned int block_size = 65536;
        
        bool can_grow = false;
        
        size_t alignment = 0;
        size_t occupied = 0;
        
        array<void*> unused_blocks;
        array<void*> used_blocks;
    };

    void* LinearAllocator(void* allocator_data, AllocationType type, void* ptr, size_t size, size_t old_size);
    void reset(struct LinearAllocator&, size_t level);
    void destroy(struct LinearAllocator&);
};
