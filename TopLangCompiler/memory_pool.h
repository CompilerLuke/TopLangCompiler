//
//  memory_pool.h
//  TopLangCompiler
//
//  Created by Lucas Goetz on 20/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//

#include <stdlib.h>
#include "allocator.h"

namespace top {
    template<unsigned int N, typename T>
    struct MemoryPool {
        union Slot {
            T value;
            Slot* next;
        };
        
        struct MemoryBlock {
            Slot slots[N];
            MemoryBlock* next = NULL;
        };
        
        Slot* next = NULL;

        MemoryBlock* block = NULL;
        
        Allocator allocator = NULL;
        void* allocator_data = NULL;
    };
    
    template<unsigned int N, typename T>
    void make_block(MemoryPool<N, T>& pool, typename MemoryPool<N, T>::MemoryBlock& block) {
        for (int i = 0; i < N - 1; i++) {
            block.slots[i].next = &block.slots[i + 1];
        }
        if (!pool.next) {
            block.slots[N - 1].next = &block.slots[0];
        } else {
            block.slots[N - 1].next = pool.next;
        }
        pool.next = &block.slots[0];
        
        block.next = pool.block;
        pool.block = &block;
    }
    
    template<unsigned int N, typename T>
    void destroy(MemoryPool<N, T>& pool) {
        auto block = pool.block;
        while (block) {
            block = block->next;
        }
    }

    template<unsigned int N, typename T>
    T* pool_alloc(MemoryPool<N, T>& pool) {
        if (!pool.next) {
            if (pool.allocator == NULL) {
                pool.allocator = MallocAllocator;
            }
            make_block(pool, *alloc<typename MemoryPool<N, T>::MemoryBlock>(pool.allocator, pool.allocator_data));
        }
       
        auto ptr = pool.next;
        pool.next = ptr->next;
        
        memset(ptr, 0, sizeof(T));
        return (T*)ptr;
    }

    template<unsigned int N, typename T>
    void pool_dealloc(MemoryPool<N, T>& pool, T* ptr) {
        auto slot = (typename MemoryPool<N, T>::Slot*)ptr;
        
        slot->next = pool.next;
        pool.next = slot;
    }
};

