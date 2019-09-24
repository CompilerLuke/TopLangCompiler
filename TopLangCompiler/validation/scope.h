//
//  scope.h
//  TopLangCompiler
//
//  Created by Lucas Goetz on 24/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//

#pragma once

#include "string.h"
#include "type.h"
#include "parser.h"

namespace top {
    namespace validation {
        struct Scope {
            array<FuncType*> func_defs;
            array<Type*> vars;
        };
    };
}
