//
//  interpreter.hpp
//  TopLangCompiler
//
//  Created by Lucas Goetz on 25/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//

#pragma once

#include "core/array.h"
#include "mir/mir.h"

namespace top {
    namespace interpreter {
        struct Interpreter {
			int registers[1000];
			mir::MIR* mir;

			int stack = 0;
            unsigned int inst = 0;
        };
        
        void interpret(mir::MIR&);
        void destroy(Interpreter&);
    }
}
