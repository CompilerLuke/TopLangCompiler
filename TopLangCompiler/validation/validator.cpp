//
//  validator.cpp
//  TopLangCompiler
//
//  Created by Lucas Goetz on 24/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//


#include "validator.h"

namespace top {
    namespace validation {
        void validate(Validator& validator, parser::Parser* parser) {
            validator.parser = parser;
        }
        
        void destroy(Validator& validator) {
            free_array(validator.scopes);
        }
    }
}
