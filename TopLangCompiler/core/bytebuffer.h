//
//  bytebuffer.h
//  TopLangCompiler
//
//  Created by Lucas Goetz on 25/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//

#pragma once

namespace top {
    inline void* read_buffer_n(array<char>& arr, unsigned int& i, unsigned int amount) {
        void* ptr = arr.data + i;
        i += amount;
        return ptr;
    }

    template<typename T>
    inline T read_buffer(array<char>& arr, unsigned int& i) {
        return *(T*)read_buffer_n(arr, i, sizeof(T));
    }

    inline void* write_buffer_n(array<char>& arr, unsigned int amount) {
        arr.length += amount;
        if (len(arr) > arr.capacity) {
            reserve(arr, len(arr) == 0 ? 2 : max(len(arr), arr.capacity * 2));
        }
        
        return arr.data + arr.length - amount;
    }

    template<typename T>
    inline void write_buffer(array<char>& arr, T value) {
        *(T*)write_buffer_n(arr, sizeof(T)) = value;
    }
}
