//
//  lexer.h
//  TopLangCompiler
//
//  Created by Lucas Goetz on 20/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//

#pragma once

#include "array.h"
#include "string.h"
#include "error.h"

namespace top {
    namespace lexer {
        enum TokenGroup { Literal, Operator, Symbol, Terminator, Keyword };
        enum TokenType {
            AddOp, SubOp, MulOp, DivOp, AssignOp, InOp, ColonAssignOp, AddAssignOp, SubAssignOp, MulAssignOp, DivAssignOp,
            Int, Float, True, False,
            Open_Paren, Close_Paren, Colon, Open_Bracket, Close_Bracket, Comma,
            Identifier,
            Newline, EndOfFile, Open_Indent, Close_Indent,
            If, Else, Elif, While, For, Def, IntType, Mut, Var
        };
        
        struct Token {
            TokenGroup group;
            TokenType type;
            string value;
            unsigned int lbp;
            
            int line;
            int column;
            int length_of_token;
        };
        
        struct Lexer {
            string input;
            string filename;
            
            size_t reserved = 0;
            
            array<Token> tokens;
            string tok;

            int i = 0;
            int line = 0;
            int column = 0;
            int indent = 0;
            int indent_diff = 0;
            
            error::Error* err = NULL;
        };
        
        void init();
        void destroy();
        
        string token_type_to_string(TokenType);
        void lex(Lexer& lexer, string input, string filename, error::Error*);
        void print_token(const Token& token);
        void destroy(Lexer& lexer);
    }
};
