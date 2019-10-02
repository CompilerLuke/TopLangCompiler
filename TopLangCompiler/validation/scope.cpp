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
        VarDesc* make_var(Validator& validator, string name, parser::AST* ast) {
            Scope& scope = last(validator.scopes);
            
            for (int i = 0; i < scope.vars.length; i++) { //todo use hash table
                if (scope.vars[i]->name == name) {
                    char* err = alloc<char>(LinearAllocator, &validator.err->allocator, 100);
                    char cname[100];
                    to_cstr(name, cname, 100);
                    sprintf(err, "Redefinition of variable %s", cname); //todo show more variable is defined
                    
                    make_validation_error(validator, ast, error::RedefinitionError, err);
                    return NULL;
                }
            }

			VarDesc* desc = pool_alloc(validator.var_desc_pool);
			desc->name = name;

			array_add(scope.vars, desc);

			return desc;
        }

		VarDesc* get_var(Validator& validator, string name, parser::AST* ast) {
			for (int i = len(validator.scopes) - 1; i >= 0; i--) {
				Scope& scope = validator.scopes[i];

				for (int i = 0; i < scope.vars.length; i++) {
					if (scope.vars[i]->name == name) return scope.vars[i];
				}
			}

			char* err = alloc<char>(LinearAllocator, &validator.err->allocator, 100); //todo use string buffer
			char cname[100];
			to_cstr(name, cname, 100);
			sprintf(err, "Unknown variable %s", cname); //todo show more variable is defined

			make_validation_error(validator, ast, error::UnknownVariable, err);
			return NULL;
		}
        
        Scope make_scope() {
            return {};
        }

        void push_scope(Validator& validator) {
			Scope scope = {};
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
