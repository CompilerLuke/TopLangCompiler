//
//  parser.cpp
//  TopLangCompiler
//
//  Created by Lucas Goetz on 20/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//

#include "parser.h"
#include <stdio.h>
#include "helper.h"

namespace top {
    namespace parser {
        lexer::Token* next(Parser& parser) {
            parser.token = &parser.lexer->tokens[++parser.i];
            return parser.token;
        }
        
        lexer::Token* look_back(Parser& parser) {
            return &parser.lexer->tokens[parser.i - 1];
        }
        
        lexer::Token* look_forward(Parser& parser) {
            return &parser.lexer->tokens[min(parser.lexer->tokens.length - 1, parser.i + 1)];
        }
        
        lexer::Token* current(Parser& parser) {
            return &parser.lexer->tokens[parser.i];
        }
        
        bool parse_error(Parser& parser) {
            return error::is_error(parser.lexer->err);
        }
        
        void make_parse_error(Parser& parser, lexer::Token* token, error::ErrorID id, string group, string mesg) {
            if (parse_error(parser)) return;
            
            error::Error* error = parser.lexer->err;
            error->column = token->column;
            error->line = token->line;
            error->group = group;
            error->mesg = mesg;
            error->token_length = token->length_of_token;
            error->id = id;
            error->filename = parser.lexer->filename;
            error->src = parser.lexer->input;
        }
    
        const char ignore_indent = 1;
        const char ignore_newline = 2;
        const char ignore_both = ignore_indent | ignore_newline;
        
        AST* expression(Parser& parser, char ignore, unsigned int rbp = 0);
        AST* statement(Parser& parser);
        void consume_terminator(Parser& parser);
        void consume_indent_and_newline(Parser& parser, char ignore);
        
        void expect(Parser& parser, lexer::TokenGroup group, lexer::TokenType type, error::ErrorID id, string err_group, string err_mesg) {
            if (parser.token->group != group || parser.token->type != type) {
                lexer::Token* token = parser.token;
                for (int i = parser.i; token->group == lexer::Terminator && i > 1; i--) {
                    token = &parser.lexer->tokens[i];
                }
                
                make_parse_error(parser, token, id, err_group, err_mesg);
            }
        }
        
        void expect_next(Parser& parser, lexer::TokenGroup group, lexer::TokenType type, error::ErrorID id, string err_group, string err_mesg) {
            next(parser);
            expect(parser, group, type, id, err_group, err_mesg);
        }
        
        void expect_advance(Parser& parser, lexer::TokenGroup group, lexer::TokenType type, error::ErrorID id, string err_group, string err_mesg) {
            expect(parser, group, type, id, err_group, err_mesg);
            next(parser);
        }
        
        AST* int_literal(Parser& parser, lexer::Token* token) {
            AST* ast = make_node(parser, AST::Literal);
            
            char buffer[100];
            to_cstr(token->value, buffer, 100);
            
            ast->literal.type = AST::LiteralData::Int;
            ast->literal.integer = atoll(buffer);
            
            return ast;
        }
        
        AST* identifier(Parser& parser, lexer::Token* token) {
            AST* ast = make_node(parser, AST::Identifier);
            ast->identifier = token->value;
            
            return ast;
        }
        
        AST* tuple(Parser& parser, lexer::Token* token) {
            AST* ast = make_node(parser, AST::Tuple);
            array_add(ast->children, expression(parser, ignore_both));
            
            expect_advance(parser, lexer::Symbol, lexer::Close_Paren, error::SyntaxError, "Syntax Error", "Expecting )");
            return ast;
        }
        
        void error_missing_pass(Parser& parser, lexer::Token* token) {
            make_parse_error(parser, token, error::SyntaxError, "Syntax Error", "Expecting keyword pass");
        }
        
        void expect_colon(Parser& parser) {
            expect_advance(parser, lexer::Symbol, lexer::Colon, error::SyntaxError, "Syntax Error", "Expecting :");
        }
        
        AST* else_expr(Parser& parser, lexer::Token* token) {
            AST* ast = make_node(parser, AST::Else);
            
            expect_colon(parser);
            if (parse_error(parser)) return NULL;
            
            ast->else_expr.body = expression(parser, 0);
            return ast;
        }
        
        AST* if_expr(Parser& parser, lexer::Token* token) {
            AST* ast = make_node(parser, AST::If);
            ast->if_expr.condition = expression(parser, ignore_newline);
            
            expect_colon(parser);
            if (parse_error(parser)) return NULL;
            
            if (parser.token->type == lexer::Newline) {
                error_missing_pass(parser, parser.token);
                return NULL;
            }
            
            ast->if_expr.body = expression(parser, 0);
            
            if (look_forward(parser)->type == lexer::Else || look_forward(parser)->type == lexer::Elif) {
                lexer::Token* t = next(parser);
                next(parser);
                ast->if_expr.or_else = t->type == lexer::Else ? else_expr(parser, t) : if_expr(parser, t);
            }
            
            return ast;
        }
        
        AST* indent_block(Parser& parser, lexer::Token* token) {
            AST* ast = make_node(parser, AST::Block);
            
            if (parser.token->type == lexer::Close_Indent) {
                error_missing_pass(parser, token);
                return NULL;
            }
            
            while (true) {
                array_add(ast->children, expression(parser, 0));
                if (parse_error(parser)) return NULL;
                
                if (parser.token->type == lexer::Close_Indent || parser.token->type == lexer::EndOfFile) break;
                else {
                    consume_terminator(parser);
                }
            }
            
            return ast;
        }
        
        AST* block(Parser& parser, lexer::Token* token) {
            AST* ast = make_node(parser, AST::Block);
            
            while (parser.token->type != lexer::Close_Bracket && parser.token->type != lexer::EndOfFile) {
                array_add(ast->children, expression(parser, false));
                if (parse_error(parser)) return NULL;
                
                if (parser.token->type == lexer::Close_Bracket || parser.token->type == lexer::EndOfFile) break;
                else {
                    consume_terminator(parser);
                }
            }
            
            next(parser);
            
            return ast;
        }
        
        AST* bool_literal(Parser& parser, lexer::Token* token) {
            AST* ast = make_node(parser, AST::Literal);
            ast->literal.type = AST::LiteralData::Bool;
            ast->literal.boolean = token->type == lexer::True;
            return ast;
        }
        
        AST* nud(Parser& parser, lexer::Token* token, char ignore) {
            if (token->group == lexer::Literal) {
                if (token->type == lexer::Int) return int_literal(parser, token);
                if (token->type == lexer::True) return bool_literal(parser, token);
                if (token->type == lexer::False) return bool_literal(parser, token);
            }
            
            if (token->group == lexer::Symbol) {
                if (token->type == lexer::Identifier) return identifier(parser, token);
                if (token->type == lexer::Open_Paren) return tuple(parser, token);
                if (token->type == lexer::Open_Bracket) return block(parser, token);
            }
            
            if (token->group == lexer::Keyword) {
                if (token->type == lexer::If) return if_expr(parser, token);
                if (token->type == lexer::Else && token->type == lexer::Elif) {
                    make_parse_error(parser, parser.token, error::SyntaxError, "Syntax Error", "(Help) You probably forgot an if statement");
                    return NULL;
                }
            }
            
            if (token->group == lexer::Terminator) {
                if (token->type == lexer::Open_Indent) return indent_block(parser, token);
            }
            
            if (token->group == lexer::Operator) {
                
                make_parse_error(parser, token, error::SyntaxError, "Syntax Error", "Operator does not support unary");
            }
            
            make_parse_error(parser, token, error::SyntaxError, "SyntaxError", "Could not parse syntax");
            return NULL;
        }
        
        AST* led(Parser& parser, lexer::Token* token, AST* left, char ignore) {
            if (token->group == lexer::Operator) {
                AST* ast = make_node(parser, AST::Operator);
                
                ast->op.type = (OperatorType)token->type;
                ast->op.left = left;
                ast->op.right = expression(parser, ignore, token->lbp);
                return ast;
            }
            
            make_parse_error(parser, token, error::SyntaxError, "SyntaxError", "Could not parse syntax");
            
            return NULL;
        }
        
        void consume_indent_and_newline(Parser& parser, char ignore) {
            while (
                (ignore & ignore_indent && (parser.token->type == lexer::Open_Indent || parser.token->type == lexer::Close_Indent))
                || (ignore & ignore_newline && parser.token->type == lexer::Newline)) {
                next(parser);
            }
        }
        
        AST* expression(Parser& parser, char ignore, unsigned int rbp) {
            consume_indent_and_newline(parser, ignore);
            lexer::Token* t = parser.token;
            
            if (t->type == lexer::EndOfFile) {
                make_parse_error(parser, t, error::SyntaxError, "SyntaxError", "Reached end of file");
                return NULL;
            }
            
            next(parser);
            consume_indent_and_newline(parser, ignore);
            AST* left = nud(parser, t, ignore);
            
            if (parse_error(parser)) return NULL;
            
            while(rbp < parser.token->lbp) {
                consume_indent_and_newline(parser, ignore);
                t = parser.token;
                next(parser);
                consume_indent_and_newline(parser, ignore);
                left = led(parser, t, left, ignore);
                if (parse_error(parser)) return NULL;
            }
            
            consume_indent_and_newline(parser, ignore);
            
            return left;
        }
    
        void consume_terminator(Parser& parser) {
            lexer::TokenType current_type = current(parser)->type;
            if (current_type == lexer::EndOfFile) return;
            if (current_type != lexer::Newline && current_type != lexer::Close_Indent) {
                if (current(parser)->type == lexer::Open_Indent) make_parse_error(parser, parser.token, error::IndentationError, "IndentationError", "Unexpected indent\n\n(Help) Use { } to create a new scope");
                else make_parse_error(parser, parser.token, error::SyntaxError, "SyntaxError", "Statement should terminate with newline");
            }
            
            do {
                next(parser);
            } while(parser.token->type == lexer::Newline);
        }
        
        AST* statement(Parser& parser) {
            AST* ast = expression(parser, false);
            if (parse_error(parser)) return NULL;
            consume_terminator(parser);
            return ast;
        }
        
        AST* make_node(Parser& parser, AST::ASTType type) {
            AST* ast = pool_alloc(parser.pool);
            ast->type = type;
            
            if (ast->type == AST::Block) {
                ast->children.allocator = LinearAllocator;
                ast->children.allocator_data = &parser.linear_allocator;
            }
            return ast;
        }
        
        void parse(Parser& parser, lexer::Lexer* lexer) {
            parser.lexer = lexer;
            parser.token = &lexer->tokens[0];
            
            parser.root = make_node(parser, AST::Block);
            
            while (parser.token->type != lexer::EndOfFile) {
                array_add(parser.root->children, statement(parser));
                if (parse_error(parser)) return;
            }
        }
        
        void destroy(Parser& parser) {
            destroy(parser.pool);
            destroy(parser.linear_allocator);
        }
        
        void destroy_node(Parser& parser, AST* ast) {
            pool_dealloc(parser.pool, ast);
        }
        
        void dump_ast(AST* ast, int indent = 0) {
            if (ast->type == AST::Operator) {
                dump_ast(ast->op.left, indent);
                
                if (ast->op.type == OperatorType::Add) printf(" + ");
                if (ast->op.type == OperatorType::Sub) printf(" - ");
                if (ast->op.type == OperatorType::Mul) printf(" * ");
                if (ast->op.type == OperatorType::Div) printf(" / ");
                if (ast->op.type == OperatorType::Assign) printf(" = ");
                
                dump_ast(ast->op.right, indent);
            }
            
            if (ast->type == AST::Literal) {
                if (ast->literal.type == AST::LiteralData::Int) {
                    printf("%lli", ast->literal.integer);
                }
                if (ast->literal.type == AST::LiteralData::Bool) {
                    printf(ast->literal.boolean ? "true" : "false");
                }
            }
            
            if (ast->type == AST::Identifier) {
                char buffer[100];
                to_cstr(ast->identifier, buffer, 100);
                printf("%s", buffer);
            }
            
            if (ast->type == AST::Block) {
                printf("{\n");
                for (int i = 0; i < len(ast->children); i++) {
                    printf("%.*s", indent + 4, "                    ");
                    dump_ast(ast->children[i], indent + 4);
                    printf("\n");
                }
                printf("%.*s}", indent, "                      ");
            }
            
            if (ast->type == AST::Tuple) {
                printf("(");
                for (int i = 0; i < len(ast->children); i++) {
                    dump_ast(ast->children[i], indent);
                }
                printf(")");
            }
            
            if (ast->type == AST::If) {
                printf("if ");
                dump_ast(ast->if_expr.condition, indent);
                printf(" : ");
                dump_ast(ast->if_expr.body, indent);
                
                if (ast->if_expr.or_else) {
                    printf("\n%.*s", indent, "             ");
                    
                    if (ast->if_expr.or_else->type == AST::If) printf("el");
                    
                    dump_ast(ast->if_expr.or_else, indent);
                }
            }
            
            if (ast->type == AST::Else) {
                printf("else: ");
                dump_ast(ast->else_expr.body, indent);
            }        
        }
        
        void dump_ast(Parser& parser) {
            dump_ast(parser.root);
            
            printf("\n");
        }
    }
};
