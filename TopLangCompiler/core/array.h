//
//  array.h
//  TopLangCompiler
//
//  Created by Lucas Goetz on 20/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//

#pragma once

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define DEFINE_SUBSCRIPT() \
T& operator[](unsigned int i) { \
assert(i < length); \
return data[i]; \
} \
\
const T& operator[](unsigned int i) const { \
assert(i < length); \
return data[i]; \
}

#include "allocator.h"

namespace top {

    template<typename T>
    struct array {
        T* data = NULL;
        unsigned int length = 0;
        unsigned int capacity = 0;
        
        Allocator allocator = NULL;
        void* allocator_data = NULL;
        
        DEFINE_SUBSCRIPT()
    };

    template<unsigned int N, typename T>
    struct static_array {
        T data[N];
        unsigned int length = 0;
        
        DEFINE_SUBSCRIPT()
    };
    
    template<typename T>
    struct slice {
        T* data;
        unsigned int length = 0;
        
        DEFINE_SUBSCRIPT()
    };

    template<unsigned int N, typename T>
    struct hybrid_array {
        T buffer[N];
        T* data = buffer;

        unsigned int length = 0;
        unsigned int capacity = N;
        
        Allocator allocator;
        void* allocator_data;
        
        DEFINE_SUBSCRIPT()
    };

    template<typename T>
    inline void capacity(array<T>& arr) {
        return arr.capacity;
    }

    template<unsigned int N, typename T>
    inline unsigned int capacity(static_array<N, T>& arr) {
        return N;
    }

    template<unsigned int N, typename T>
    inline void capacity(hybrid_array<N, T>& arr) {
        return arr.capacity;
    }

    template<typename T>
    inline unsigned int len(T& arr) { return arr.length; }

    template<typename T>
    inline void reserve(array<T>& arr, unsigned int num) {
        if (num > arr.capacity) {
            if (!arr.allocator) arr.allocator = MallocAllocator;
            
            if (arr.data == NULL) arr.data = alloc<T>(arr.allocator, arr.allocator_data, num);
            else arr.data = resize<T>(arr.allocator, arr.allocator_data, arr.data, num, arr.capacity);
            arr.capacity = num;
        }
    }

    template<unsigned int N, typename T>
    inline void reserve(hybrid_array<N, T>& arr, unsigned int num) {
        if (num > arr.capacity) {
            if (!arr.allocator) arr.allocator = MallocAllocator;
            
            if (arr.data == NULL) arr.data = alloc<T>(arr.allocator, arr.allocator_data, num);
            else {
                if (arr.capacity <= N) {
                    memcpy(malloc(sizeof(T) * num), arr.buffer, num);
                } else {
                    arr.data = resize<T>(arr.allocator, arr.allocator_data, num);
                }
            }
            arr.capacity = num;
        }
    }

    template<typename T>
    inline void array_add(array<T>& arr, T value) {
        if (arr.length + 1 > arr.capacity) {
            reserve(arr, arr.capacity == 0 ? 2 : arr.capacity * 2);
        }
        arr.data[arr.length++] = value;
    }

    template<unsigned int N, typename T>
    inline void array_add(hybrid_array<N, T>& arr, T value) {
        if (arr.length + 1 > arr.capacity) {
            reserve(arr, arr.capacity == 0 ? 2 : arr.capacity * 2);
        }
        arr.data[arr.length++] = value;
    }

    template<unsigned int N, typename T>
    inline void array_add(static_array<N, T>& arr, T value) {
        unsigned int index = arr.length++;
        assert(arr.length < N);
        arr.data[index] = value;
    }
    
    template<typename T>
    inline T& last(array<T>& arr) {
        return arr[arr.length - 1];
    }
    
    template<unsigned int N, typename T>
    inline T& last(hybrid_array<N, T>& arr) {
        return arr[arr.length - 1];
    }
    
    template<unsigned int N, typename T>
    inline T& last(static_array<N, T>& arr) {
        return arr[arr.length - 1];
    }

    template<typename T>
    inline void free_array(array<T>& arr) {
        if (arr.data) dealloc<T>(arr.allocator, arr.allocator_data, arr.data);
    }

    template<unsigned int N, typename T>
    inline void free_array(hybrid_array<N, T>& arr) {
        if (arr.capacity > N) dealloc<T>(arr.allocator, arr.allocator_data, arr.data);
    }

    template<typename T>
    T pop(array<T>& arr) {
        return arr.data[--arr.length];
    }
};


