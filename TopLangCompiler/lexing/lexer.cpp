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
#include "core/array.h"
#include "core/helper.h"
#include "core/linear_allocator.h"

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
        
        struct KeywordDesc {
            string name;
            TokenGroup group;
            TokenType type;
            unsigned int lbp;
            bool has_value;
        };
        
        static_array<20, KeywordDesc> keywords;
        
        error::Error* make_error(Lexer& lexer) {
            error::Error* err = lexer.err;
            err->line = lexer.line;
            err->column = lexer.column;
            return err;
        }
        
        void match_token(Lexer& lexer) {
            for (int i = 0; i < keywords.length; i++) {
                if (lexer.tok == keywords[i].name) {
                    return add_tok(lexer, keywords[i].group, keywords[i].type, keywords[i].lbp, keywords[i].has_value);
                }
            }
            
            if (is_int(lexer.tok)) { add_tok(lexer, Literal, Int, 0, true); }
            else if (is_identifier(lexer.tok)) { add_tok(lexer, Symbol, Identifier, 0, true); }
            else {
                char buffer[100];
                to_cstr(lexer.tok, buffer, 100);
                
                error::Error* err = make_error(lexer);
                err->mesg = "Invalid token";
                err->id = error::UnknownToken;
                err->column -= lexer.tok.length;
                err->token_length = lexer.tok.length;
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
            char c;
            bool is_delimitter;
            TokenGroup group;
            TokenType type;
            unsigned int lbp;
            array<Delimitter> next_char;
        };
        
        struct LinearAllocator linear_allocator;
        Delimitter delimitters[256] = {};
        string type_to_string[256] = {};
        
        string token_type_to_string(TokenType type) {
            return type_to_string[(unsigned int)type];
        }
        
        void add_delimitter(char c, TokenGroup group, TokenType type, unsigned int lbp) {
            delimitters[c] = { c, true, group, type, lbp };
            type_to_string[type] = { &delimitters[c].c, 1 };
        }
        
        void add_delimitter(string s, TokenGroup group, TokenType type, unsigned int lbp) {
            Delimitter& d = delimitters[s[0]];
            d.next_char.allocator = LinearAllocator;
            d.next_char.allocator_data = &linear_allocator;

            array_add(d.next_char, { s[1], true, group, type, lbp });
            type_to_string[type] = s;
        }
        
        void add_keyword(string name, TokenGroup group, TokenType type, unsigned int lbp = 0, bool has_value = false) {
            KeywordDesc desc = {};
            desc.name = name;
            desc.group = group;
            desc.type = type;
            desc.lbp = lbp;
            desc.has_value = has_value;
            
            array_add(keywords, desc);
            type_to_string[type] = name;
        }
        
        void init() {
            linear_allocator.can_grow = false;
            linear_allocator.block_size = 1024;
            
            add_delimitter('(', Symbol, Open_Paren, 6);
            add_delimitter(')', Symbol, Close_Paren, 0);
            
            add_delimitter(':', Symbol, Colon, 0);
            add_delimitter(":=", Operator, ColonAssignOp, 5);
            
            add_delimitter('{', Symbol, Open_Bracket, 0);
            add_delimitter('}', Symbol, Close_Bracket, 0);
            add_delimitter(',', Symbol, Comma, 0);
            
            add_delimitter('+', Operator, AddOp, 10);
            add_delimitter('-', Operator, SubOp, 10);
            add_delimitter('*', Operator, MulOp, 20);
            add_delimitter('/', Operator, DivOp, 20);
            
            add_delimitter('=', Operator, AssignOp, 5);
            add_delimitter("+=", Operator, AddAssignOp, 5);
            add_delimitter("-=", Operator, SubAssignOp, 5);
            add_delimitter("*=", Operator, MulAssignOp, 10);
            add_delimitter("/=", Operator, DivAssignOp, 10);
            
            add_keyword("if", Keyword, If);
            add_keyword("elif", Keyword, Elif);
            add_keyword("else", Keyword, Else);
            add_keyword("true", Literal, True);
            add_keyword("false", Literal, False, 0);
            add_keyword("while", Keyword, While, 0);
            add_keyword("for", Keyword, For, 0);
            add_keyword("in", Operator, InOp, 5);
            add_keyword("def", Keyword, Def);
            add_keyword("int", Keyword, IntType);
            add_keyword("mut", Keyword, Mut);
            add_keyword("var", Keyword, Var);
        }
        
        void match_delimitter(Lexer& lexer, Delimitter& d) {
            reset_tok(lexer);
            
            for (int i = 0; i < d.next_char.length; i++) {
                bool in_range = lexer.i + 1 < lexer.input.length;
                if (in_range && lexer.input[lexer.i+1] == d.next_char[i].c) {
                    add_token(lexer, 2, lexer.column, d.next_char[i].group, d.next_char[i].type, d.next_char[i].lbp);
                    lexer.column++;
                    lexer.i++;
                    return;
                }
            }
            
            add_token(lexer, 1, lexer.column, d.group, d.type, d.lbp);
        }
    
        slice<Token> lex(Lexer& lexer, string input, error::Error* err) {
            lexer.input = input;
            lexer.reserved = 10;
            lexer.tok.data = lexer.input.data;
            lexer.err = err;
            lexer.line = 1;
            
            for (lexer.i = 0; lexer.i < lexer.input.length; lexer.i++, lexer.column++) {
                char c = lexer.input[lexer.i];
                
                Delimitter& d = delimitters[c];
                
                if (c == ' ') {
                    reset_tok(lexer);
                }
				else if (c == '\r') {
					reset_tok(lexer);
				}
                else if (c == '\t') {
                    error::Error* err = make_error(lexer);
                    err->mesg = "Use spaces instead of tabs";
                    err->id = error::IndentationError;
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
                                error::Error* err = make_error(lexer);
                                lexer.err->id = error::IndentationError;
                                
                                char* buffer = (char*)malloc(100);
                                snprintf(buffer, 100, "Inconsistent indentation, expecting an indent of %i spaces but got %i spaces", lexer.indent_diff, indent_off);
                                
                                err->mesg = { buffer, (unsigned int)strlen(buffer) };
                                err->column = max(0, num_indent - indent_off);
                                err->token_length = indent_off;
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
                    match_delimitter(lexer, d);
                }
                else {
                    lexer.tok.length++;
                }
                
                if (error::is_error(lexer.err)) return {};
            }
            
            reset_tok(lexer);
            add_token(lexer, 1, lexer.column, Terminator, EndOfFile, 0);
        
            return { lexer.tokens.data, lexer.tokens.length };
        }
        
        void print_token(const Token& token) {
            const char* group;
            const char* type;
			char type_buffer[100];
            
            if (token.group == lexer::Literal) group = "literal";
            if (token.group == lexer::Operator) group = "operator";
            if (token.group == lexer::Symbol) group = "symbol";
            if (token.group == lexer::Terminator) group = "terminator";
            if (token.group == lexer::Keyword) group = "keyword";
            
            string tok = token_type_to_string(token.type);
			if (tok.length > 0) {
				to_cstr(tok, type_buffer, 100);
				type = type_buffer;
			}
            if (token.type == lexer::Newline) type = "Newline";
            if (token.type == lexer::EndOfFile) type = "EOF";
            if (token.type == lexer::Identifier) type = "identifier";
            if (token.type == lexer::Int) type = "int";
            if (token.type == lexer::Open_Indent) type = "OpenIndent";
            if (token.type == lexer::Close_Indent) type = "CloseIndent";
            
            if (token.value.length) {
                char buffer[100];
                to_cstr(token.value, buffer, 100);
                printf("(%s, %s, %s)\n", group, type, buffer);
            } else {
                printf("(%s, %s)\n", group, type);
            }
            
        }
        
        void dump_tokens(Lexer& lexer) {
            printf("=== Lexer ===\n");
            
            for (int i = 0; i < len(lexer.tokens); i++) {
                lexer::print_token(lexer.tokens[i]);
            }
        }
        
        void destroy(Lexer& lexer) {
            free_array(lexer.tokens);
        }
        
        void destroy() {
            destroy(linear_allocator);
        }
    }
}
