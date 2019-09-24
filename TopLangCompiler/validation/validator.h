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
    namespace validation {
        struct Validator {
            parser::Parser* parser;
            array<Scope> scopes;
        };
        
        void push_scope(Validator& validator);
        void pop_scope(Validator& validator);
        
        void validate(Validator& validator, parser::Parser& parser);
    }
}


