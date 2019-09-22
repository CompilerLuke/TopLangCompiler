//
//  lexer.cpp
//  TopLangCompiler
//
//  Created by Lucas Goetz on 20/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//

#include <stdio.h>
#include <assert.h>
#include "lexer.h"
#include <string.h>
#include "string.h"
#include "array.h"
#include "helper.h"

namespace top {
    namespace lexer {
        void add_token(Lexer& lexer, int length_of_token, int column, TokenGroup group, TokenType type, unsigned int lbp, string value = {}) {
            Token token = {};
            token.type = type;
            token.group = group;
            token.value = value;
            token.lbp = lbp;
            token.line = lexer.line;
            token.length_of_token = length_of_token;
            token.column = column;
            
            assert(column >= 0);
            
            array_add(lexer.tokens, token);
        }
        
        bool is_digit(char c) {
            return '0' <= c && c <= '9';
        }
        
        bool is_int(string tok) {
            if (tok[0] == '0') return false;
            
            for (int i = 0; i < len(tok); i++) {
                if (!is_digit(tok[i])) return false;
            }
            
            return true;
        }
        
        bool is_character(char c) {
            return 'A' <= c && c <= 'z';
        }
        
        bool is_identifier(string tok) {
            if (!is_character(tok[0])) return false;
            
            for (int i = 1; i < len(tok); i++) {
                if (!is_character(tok[i]) && is_digit(tok[i])) return false;
            }
            
            return true;
        }
        
        void add_tok(Lexer& lexer, TokenGroup group, TokenType type, int lbp, bool has_value = false) {
            add_token(lexer, lexer.tok.length, lexer.column - lexer.tok.length, group, type, lbp, has_value ? lexer.tok : string());
        }
        
        void match_token(Lexer& lexer) {
            if (lexer.tok == "if") { add_tok(lexer, Keyword, If, 0); }
            else if (lexer.tok == "elif") { add_tok(lexer, Keyword, Elif, 0); }
            else if (lexer.tok == "else") { add_tok(lexer, Keyword, Else, 0); }
            else if (lexer.tok == "true") { add_tok(lexer, Literal, True, 0); }
            else if (lexer.tok == "false") { add_tok(lexer, Literal, False, 0); }
            else if (is_int(lexer.tok)) { add_tok(lexer, Literal, Int, 0, true); }
            else if (is_identifier(lexer.tok)) { add_tok(lexer, Symbol, Identifier, 0, true); }
            else {
                char buffer[100];
                to_cstr(lexer.tok, buffer, 100);
                
                lexer.err->mesg = "Invalid token";
                lexer.err->group = "Lexing";
                lexer.err->id = error::UnknownToken;
                lexer.err->column = lexer.column - lexer.tok.length;
                lexer.err->line = lexer.line;
                lexer.err->src = lexer.input;
                lexer.err->filename = lexer.filename;
                lexer.err->token_length = lexer.tok.length;
            }
        }
        
        void reset_tok(Lexer& lexer) {
            if (lexer.tok.length > 0) {
                match_token(lexer);
            }
            
            lexer.tok.data = lexer.input.data + lexer.i + 1;
            lexer.tok.length = 0;
        }
        
        struct Delimitter {
            bool is_delimitter;
            TokenGroup group;
            TokenType type;
            unsigned int lbp;
        };
    
        
        void lex(Lexer& lexer, string input, string filename, error::Error* err) {
            lexer.input = input;
            lexer.filename = filename;
            lexer.reserved = 10;
            lexer.tok.data = lexer.input.data;
            lexer.err = err;
            lexer.line = 1;
            
            Delimitter delimitters[256] = {};
            
            delimitters['('] = { true, Symbol, Open_Paren, 0 };
            delimitters[')'] = { true, Symbol, Close_Paren, 0 };
            delimitters[':'] = { true, Symbol, Colon, 0 };
            delimitters['{'] = { true, Symbol, Open_Bracket, 0 };
            delimitters['}'] = { true, Symbol, Close_Bracket, 0};
            
            delimitters['+'] = { true, Operator, AddOp, 10 };
            delimitters['-'] = { true, Operator, SubOp, 10 };
            delimitters['*'] = { true, Operator, MulOp, 20 };
            delimitters['/'] = { true, Operator, DivOp, 20 };
            delimitters['='] = { true, Operator, AssignOp, 5};
            
            for (lexer.i = 0; lexer.i < lexer.input.length; lexer.i++, lexer.column++) {
                char c = lexer.input[lexer.i];
                
                Delimitter& d = delimitters[c];
                
                if (c == ' ') {
                    reset_tok(lexer);
                }
                else if (c == '\t') {
                    lexer.err->mesg = "Use spaces instead of tabs";
                    lexer.err->group = "Indentation Error";
                    lexer.err->column = lexer.column;
                    lexer.err->src = lexer.input;
                    lexer.err->line = lexer.line;
                    lexer.err->token_length = 1;
                }
                else if (c == '\n') {
                    reset_tok(lexer);
                    
                    int num_indent = 0;
                    while (lexer.i + 1 < lexer.input.length && lexer.input[lexer.i + 1] == '\n') {
                        lexer.i++;
                        lexer.line++;
                    }
                    
                    while (lexer.i + 1 < lexer.input.length && lexer.input[lexer.i + 1] == ' ') {
                        lexer.i++;
                        lexer.column++;
                        num_indent++;
                    }
                    
                    reset_tok(lexer);
                    
                    int diff = num_indent - lexer.indent;
                    
                    if (diff == 0) {
                        add_token(lexer, 1, lexer.column, Terminator, Newline, 0);
                        lexer.line++;
                        
                    } else {
                        lexer.line++;

                        if (lexer.indent_diff == 0) lexer.indent_diff = abs(diff);
                        else {
                            int indent_off = diff % lexer.indent_diff;
                            if (indent_off != 0) {
                                lexer.err->id = error::IndentationError;
                                
                                char* buffer = (char*)malloc(100);
                                snprintf(buffer, 100, "Inconsistent indentation, expecting an indent of %i spaces but got %i spaces", lexer.indent_diff, indent_off);
                                
                                lexer.err->mesg = { buffer, (unsigned int)strlen(buffer) };
                                lexer.err->group = "Indentation Error";
                                lexer.err->column = max(0, num_indent - indent_off);
                                lexer.err->src = lexer.input;
                                lexer.err->line = lexer.line;
                                lexer.err->token_length = indent_off;
                                
                            }
                        }
                        
                        lexer.column = max(0, num_indent - lexer.indent_diff);
                        
                        if (diff > 0) {
                            add_token(lexer, lexer.indent_diff, lexer.column, Terminator, Open_Indent, 0);
                        } else {
                            add_token(lexer, lexer.indent_diff, lexer.column, Terminator, Close_Indent, 0);
                        }
                    }
                    lexer.indent = num_indent;
                    lexer.column = num_indent - 1;
                }
                else if (d.is_delimitter) {
                    reset_tok(lexer);
                    add_token(lexer, 1, lexer.column, d.group, d.type, d.lbp);
                }
                else {
                    lexer.tok.length++;
                }
                
                if (error::is_error(lexer.err)) return;
            }
            
            reset_tok(lexer);
            add_token(lexer, 1, lexer.column, Terminator, EndOfFile, 0);
        }
        
        void print_token(const Token& token) {
            const char* group;
            const char* type;
            
            if (token.group == lexer::Literal) group = "literal";
            if (token.group == lexer::Operator) group = "operator";
            if (token.group == lexer::Symbol) group = "symbol";
            if (token.group == lexer::Terminator) group = "terminator";
            if (token.group == lexer::Keyword) group = "keyword";
            
            if (token.type == lexer::AddOp) type = "+";
            if (token.type == lexer::SubOp) type = "-";
            if (token.type == lexer::MulOp) type = "*";
            if (token.type == lexer::DivOp) type = "/";
            if (token.type == lexer::AssignOp) type = "=";
            if (token.type == lexer::Int) type = "int";
            if (token.type == lexer::Float) type = "float";
            if (token.type == lexer::Open_Paren) type = "(";
            if (token.type == lexer::Close_Paren) type = ")";
            if (token.type == lexer::Colon) type = ":";
            if (token.type == lexer::Newline) type = "Newline";
            if (token.type == lexer::EndOfFile) type = "EOF";
            if (token.type == lexer::Identifier) type = "identifier";
            if (token.type == lexer::If) type = "if";
            if (token.type == lexer::Elif) type = "elif";
            if (token.type == lexer::Else) type = "else";
            if (token.type == lexer::Open_Indent) type = "OpenIndent";
            if (token.type == lexer::Close_Indent) type = "CloseIndent";
            if (token.type == lexer::True) type = "true";
            if (token.type == lexer::False) type = "false";
            if (token.type == lexer::Open_Bracket) type = "{";
            if (token.type == lexer::Close_Bracket) type = "}";
            
            if (token.value.length) {
                char buffer[100];
                to_cstr(token.value, buffer, 100);
                printf("(%s, %s, %s)\n", group, type, buffer);
            } else {
                printf("(%s, %s)\n", group, type);
            }
            
        }
        
        void destroy(Lexer& lexer) {
            free_array(lexer.tokens);
        }
    }
}
