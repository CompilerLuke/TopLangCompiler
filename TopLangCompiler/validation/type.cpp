//
//  type.cpp
//  TopLangCompiler
//
//  Created by Lucas Goetz on 25/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//

#include "type.h"
#include "validator.h"

namespace top {
    namespace validator {
        Type basic_types[10];
        
        const int BasicTypeInt = 0;
        
        
        void init_types() {
            
        }
        
        Type* int_type() {
            return &basic_types[BasicTypeInt];
        }
        
        void duck_type(Validator& validator, parser::AST* ast, Type* a, Type* b) {
            
        }
    }
}
