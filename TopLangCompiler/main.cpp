//
//  main.cpp
//  TopLangCompiler
//
//  Created by Lucas Goetz on 20/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//

#include <iostream>
#include "lexer.h"
#include <string.h>

#include "array.h"
#include "parser.h"

#include "error.h"

using namespace top;

int main(int argc, const char * argv[]) {
    // insert code here...

    string input = "10\n20\n30";
    
    printf("========= Compiling =========\n\n");
    
    error::Error err;
    
    lexer::Lexer lexer;
    lexer::lex(lexer, input, "<stdin>",  &err);
    
    if (error::is_error(&err)) {
        error::log_error(&err);
        lexer::destroy(lexer);
        return 1;
    }
    
    for (int i = 0; i < len(lexer.tokens); i++) {
        lexer::print_token(lexer.tokens[i]);
    }
    
    parser::Parser parser;
    parser::parse(parser, &lexer);
    
    if (error::is_error(&err)) {
        error::log_error(&err);
        parser::destroy(parser);
        lexer::destroy(lexer);
        return 1;
    }
    
    parser::dump_ast(parser);
    
    printf("Occupied bytes: %lu\n", parser.linear_allocator.occupied);
    
    parser::destroy(parser);
    lexer::destroy(lexer);

    return 0;
}
