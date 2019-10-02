//
//  string.h
//  TopLangCompiler
//
//  Created by Lucas Goetz on 20/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//

#pragma once

#include <string.h>
#include <assert.h>

namespace top {
    struct string {
        const char* data = NULL;
        unsigned int length = 0;
        
        char operator[](unsigned int i) const {
            assert(i < length);
            return data[i];
        }
        
        bool operator==(string other) {
            if (other.length != length) return false;
            return strncmp(data, other.data, length) == 0;
        }
        
        inline string() {}
        inline string(const char* data, unsigned int length) : data(data), length(length) {}
        
        inline string(const char* cstr) {
            this->data = cstr;
            this->length = (unsigned int)strlen(cstr);
        }
    };
    
    inline unsigned int len(string s) {
        return s.length;
    }
    
    inline void to_cstr(string s, char* buffer, unsigned int length) {
        assert(s.length + 1 < length);
        memcpy(buffer, s.data, s.length);
        buffer[s.length] = '\0';
    }
};
