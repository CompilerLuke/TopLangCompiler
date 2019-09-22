//
//  file.h
//  TopLangCompiler
//
//  Created by Lucas Goetz on 22/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//

#pragma once

#include "string.h"
#include "allocator.h"

namespace top {
    namespace io {
        enum class FileMode { Read, Write };
        
        FILE* open(string, FileMode);
        string read_file(FILE*, Allocator alloc = MallocAllocator, void* alloc_data = NULL);
        void destroy(FILE*);
    }
};
