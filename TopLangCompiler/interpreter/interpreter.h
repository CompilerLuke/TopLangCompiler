//
//  interpreter.hpp
//  TopLangCompiler
//
//  Created by Lucas Goetz on 25/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//

#pragma once

#include "array.h"
#include "mir.h"

namespace top {
    namespace interpreter {
        struct Interpreter {
            array<char> stack;
            mir::MIR* mir;
            mir::Func* func_ptr;
            unsigned int inst = 0;
        };
        
        void interpret(mir::MIR&);
        void destroy(Interpreter&);
    }
}
