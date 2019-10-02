//
//  type.h
//  TopLangCompiler
//
//  Created by Lucas Goetz on 24/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//

#pragma once

#include "../core/array.h"
#include "parser.h"

namespace top {
    namespace validator {
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
        
        Type* int_type();
        void init_types();
        
        void duck_type(struct Validator&, parser::AST*, Type*, Type*);
    }
}
