//
//  type.h
//  TopLangCompiler
//
//  Created by Lucas Goetz on 24/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//

#pragma once

#include "array.h"
#include "parser.h"

namespace top {
    namespace validation {
        struct Type {
            enum TypeType { Int };
        };
        
        struct FuncArg {
            string name;
            Type* type = NULL;
            parser::AST* default_value = NULL;
        };
        
        struct FuncType {
            string name;
            array<FuncArg> args;
            Type* return_type;
            
            parser::AST* body;
        };
    }
}
