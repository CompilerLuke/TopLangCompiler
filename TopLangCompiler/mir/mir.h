//
//  mir.h
//  TopLangCompiler
//
//  Created by Lucas Goetz on 25/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//

#pragma once

#include "array.h"
#include "linear_allocator.h"

namespace top {
    namespace parser {
        struct AST;
    }
    
    namespace mir {
        enum Opcode {
            Add_Int32, Sub_Int32, Mul_Int32, Div_Int32,
            Push_Int32
        };
        
        struct Func {
            array<char> bytecode;
        };
        
        void* read_bytecode(Func&, unsigned int& i, unsigned int amount);
        void* write_bytecode(Func&, unsigned int amount);
        
        struct MIR {
            struct LinearAllocator linear_allocator;
            array<Func> funcs;
        };
        
        struct Converter {
            MIR* mir;
        };
        
        MIR gen_mir(parser::AST*);
        void destroy(MIR&);
        void dump_mir(MIR&);
    }
}
