//
//  scope.cpp
//  TopLangCompiler
//
//  Created by Lucas Goetz on 25/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//

#include "scope.h"
#include "validator.h"
#include "stdio.h"

namespace top {
    namespace validator {
        void make_var(Validator& validator, VarDesc desc, parser::AST* ast) {
            Scope& scope = last(validator.scopes);
            
            for (int i = 0; i < scope.vars.length; i++) { //todo use hash table
                if (scope.vars[i].name == desc.name) {
                    char err[100]; //todo use string buffer
                    char name[100];
                    to_cstr(desc.name, name, 100);
                    sprintf(err, "Redefinition of variable %s", name);
                    
                    make_validation_error(validator, ast, error::RedefinitionError, err);
                    return;
                }
            }
        }
        
        Scope make_scope() {
            return {};
        }

        void push_scope(Validator& validator) {
            Scope scope;
            if (len(validator.unused_scopes) > 0) scope = pop(validator.unused_scopes);
            else scope = {};
            
            array_add(validator.scopes, scope);
        }
        
        void pop_scope(Validator& validator) {
            Scope scope = pop(validator.scopes);
            clear(scope.func_defs);
            clear(scope.vars);
            
            array_add(validator.unused_scopes, scope);
        }
        
        void make_scopes(Validator& validator) {
            
        }
        
        void array_scope_free(array<Scope>& scopes) {
            for (int i = 0; i < scopes.length; i++) {
                free_array(scopes[i].vars);
                free_array(scopes[i].func_defs);
            }
            
            free_array(scopes);
        }
        
        void destroy_scopes(Validator& validator) {
            array_scope_free(validator.scopes);
            array_scope_free(validator.unused_scopes);
        }
    }
}
