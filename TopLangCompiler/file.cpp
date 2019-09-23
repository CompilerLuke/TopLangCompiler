//
//  file.cpp
//  TopLangCompiler
//
//  Created by Lucas Goetz on 22/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//

#include <stdio.h>
#include "file.h"
#include <limits.h>

namespace top {
    namespace io {
        FILE* open(string src, FileMode mode) {
            const char* fmode = "";
            if (mode == FileMode::Read) fmode = "r";
            if (mode == FileMode::Write) fmode = "w";
            
            char src_buffer[100];
            to_cstr(src, src_buffer, 100);
            
            return fopen(src_buffer, fmode);
            
        }
        
        string read_file(FILE* file, Allocator allocator, void* allocator_data) {
            fseek(file, 0, SEEK_END);
            long fsize = ftell(file);
            fseek(file, 0, SEEK_SET);  /* same as rewind(f); */
            
            char* buffer = alloc<char>(allocator, allocator_data, fsize + 1);
            fread(buffer, 1, fsize, file);
            
            assert(fsize < INT_MAX);
            return { buffer, (unsigned int) fsize };
        }
        
        void destroy(FILE* file) {
            fclose(file);
        }
    }
}