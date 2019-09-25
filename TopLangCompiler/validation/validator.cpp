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
        
        void validate_operator(Validator& validator, parser::AST* ast) {
            Type* a = validate_node(validator, ast->op.left);
            Type* b = validate_node(validator, ast->op.right);
            
            duck_type(validator, ast, a, b);
        }
        
        bool validation_error(Validator& validator) {
            return validator.err->mesg.length != 0;
        }
        
        Type* validate_node(Validator& validator, parser::AST* ast) {
            switch (ast->type) {
                case parser::AST::Block: validate_block(validator, ast); break;
                case parser::AST::Operator: validate_operator(validator, ast); break;
                case parser::AST::Literal: validate_literal(validator, ast); break;
                default: make_validation_error(validator, ast, error::SyntaxError, "Compiler Bug: Unsupported ast!");
            }
            
            return ast->ast_type;
        }
        
        void validate(Validator& validator, parser::AST* ast, error::Error* err) {
            validator.err = err;
            
            validate_node(validator, ast);
            
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
        }
    }
}
