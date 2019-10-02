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
    namespace validator {
        struct VarDesc {
            bool immutable = false;
            string name;
            Type* type;
        };
        
        struct Scope {
            array<FuncType*> func_defs;
            array<VarDesc*> vars;
        };
        
        VarDesc* make_var(struct Validator&, string name, parser::AST* ast);
		VarDesc* get_var(struct Validator&, string name, parser::AST* ast);

        void push_scope(struct Validator&);
        void pop_scope(struct Validator&);
        
        void make_scopes(struct Validator&);
        void destroy_scopes(struct Validator&);
    };
}
