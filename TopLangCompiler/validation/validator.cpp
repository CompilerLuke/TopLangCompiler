//
//  validator.cpp
//  TopLangCompiler
//
//  Created by Lucas Goetz on 24/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//


#include "validator.h"
#include "type.h"

namespace top {
    namespace validator {
		bool validation_error(Validator& validator) {
			return validator.err->mesg.length != 0;
		}

		#define ON_ERROR() if (validation_error(validator)) return;

        void validate_block(Validator& validator, parser::AST* ast) {
            for (int i = 0; i < len(ast->children); i++) {
                validate_node(validator, ast->children[i]);
            }
        }
        
        void validate_literal(Validator& validator, parser::AST* ast) {
            if (ast->literal.type == parser::AST::LiteralData::Int) {
                ast->ast_type = int_type();
            }
        }

		void colon_assign(Validator& validator, parser::AST* ast) {
			parser::AST* bind_to = ast->op.left;
			parser::AST* value = ast->op.right;

			if (bind_to->type != parser::AST::Identifier) {
				return make_validation_error(validator, bind_to, error::SyntaxError, "Expecting identifier");
			}

			Type* value_type = validate_node(validator, ast->op.right); //todo check if assigning none to variable
			
			ON_ERROR()
			VarDesc* desc = make_var(validator, bind_to->identifier.name, bind_to);
			ON_ERROR()

			desc->type = value_type;
			desc->immutable = true;
			bind_to->identifier.desc = desc;
		}
        
        void validate_operator(Validator& validator, parser::AST* ast) {
			switch (ast->op.type) {
				case OperatorType::ColonAssign: colon_assign(validator, ast); break;
				default: {
					Type* a = validate_node(validator, ast->op.left);
					Type* b = validate_node(validator, ast->op.right);
					ON_ERROR()

					duck_type(validator, ast, a, b);
				}
			}
        }

		void validate_identifier(Validator& validator, parser::AST* ast) {
			VarDesc* desc = get_var(validator, ast->identifier.name, ast);
			ast->identifier.desc = desc;
			ON_ERROR()
			ast->ast_type = desc->type;
		}
        
        Type* validate_node(Validator& validator, parser::AST* ast) {
            switch (ast->type) {
                case parser::AST::Block: validate_block(validator, ast); break;
                case parser::AST::Operator: validate_operator(validator, ast); break;
                case parser::AST::Literal: validate_literal(validator, ast); break;
				case parser::AST::Identifier: validate_identifier(validator, ast); break;
                default: make_validation_error(validator, ast, error::SyntaxError, "Compiler Bug: Unsupported ast!");
            }
            
            return ast->ast_type;
        }
        
        void validate(Validator& validator, parser::AST* ast, error::Error* err) {
            validator.err = err;
            
			push_scope(validator);
            validate_node(validator, ast);
			pop_scope(validator);

            destroy_scopes(validator);
        }
        
        void make_validation_error(Validator& validator, parser::AST* ast, error::ErrorID id, string mesg) {
            error::Error* err = validator.err;
            err->column = ast->token->column;
            err->line = ast->token->line;
            err->column = ast->token->column;
            err->token_length = ast->token->length_of_token;
            err->id = id;
            err->mesg = mesg;
        }
        
        void destroy(Validator& validator) {
            destroy(validator.type_pool);
			destroy(validator.var_desc_pool);
        }
    }
}
