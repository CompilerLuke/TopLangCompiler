//
//  linear_allocator.cpp
//  TopLangCompiler
//
//  Created by Lucas Goetz on 21/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//

#include "linear_allocator.h"
#include <stdio.h>

namespace top {
    void* LinearAllocator(void* allocator_data, AllocationType type, void* ptr, size_t size, size_t old_size) { //todo respect alignment
        struct LinearAllocator* allocator = (struct LinearAllocator*)allocator_data;
        
        if (type == AllocationType::Alloc) {
            if (size > allocator->block_size) {
                void* ptr = malloc(size);
                array_add(allocator->used_blocks, ptr); //todo utilise all the space allocated when block is returned
                return ptr;
            }
            
            if (allocator->used_blocks.length > 0) {
                size_t space_left = allocator->used_blocks.length * allocator->block_size;
                if (allocator->occupied + size > space_left) {
                    allocator->occupied = allocator->used_blocks.length * allocator->block_size;
                    if (allocator->unused_blocks.length > 0) array_add(allocator->used_blocks, pop(allocator->unused_blocks));
                    else array_add(allocator->used_blocks, malloc(allocator->block_size));
                }
            } else {
                array_add(allocator->used_blocks, malloc(allocator->block_size));
            }
            
            void* ptr = (char*)allocator->used_blocks[allocator->used_blocks.length - 1] + allocator->occupied - ((allocator->used_blocks.length - 1) * allocator->block_size);
            
            allocator->occupied += size;
            return ptr;
        }
        
        if (type == AllocationType::Realloc) {
            void* new_ptr = LinearAllocator(allocator_data, AllocationType::Alloc, NULL, size, 0);
            memcpy(new_ptr, ptr, old_size);
            return new_ptr;
        }
        
        return NULL;
    }
    
    void reset(struct LinearAllocator& allocator, size_t amount) {
        size_t popoff = (allocator.occupied - amount) / allocator.block_size;
        
        for (int i = 0; i < popoff; i++) {
            array_add(allocator.unused_blocks, pop(allocator.used_blocks));
        }
        allocator.occupied = amount;
    }
    
    void destroy(struct LinearAllocator& allocator) {
        for (int i = 0; i < allocator.unused_blocks.length; i++) {
            free(allocator.unused_blocks[i]);
        }
        
        for (int i = 0; i < allocator.unused_blocks.length; i++) {
            free(allocator.used_blocks[i]);
        }
    }
}
