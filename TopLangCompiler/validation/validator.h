//
//  validator.hpp
//  TopLangCompiler
//
//  Created by Lucas Goetz on 24/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//

#pragma once

#include "scope.h"
#include "array.h"
#include "parser.h"

namespace top {
    namespace validator {
        struct Validator {
            error::Error* err;
            
            array<Scope> scopes;
            array<Scope> unused_scopes;
            
            MemoryPool<1000, Type> type_pool;
            Type* basic_types[20];
        };
        
        void make_validation_error(Validator& validator, parser::AST*, error::ErrorID, string);
        
        void validate(Validator& validator, parser::AST*, error::Error*);
        
        Type* validate_node(Validator& validator, parser::AST* ast);
        void destroy(Validator& validator);
    }
}


