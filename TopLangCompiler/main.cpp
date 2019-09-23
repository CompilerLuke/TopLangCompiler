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
#include "file.h"

using namespace top;

int main(int argc, const char * argv[]) {
    // insert code here...

    FILE* file = io::open("main.top", io::FileMode::Read);
    if (!file) {
        printf("\033[1;31mCould not read file %s\033[0m\n", "main.top");
        return 1;
    }
    
    string input = io::read_file(file);
    io::destroy(file);
    
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
    dealloc(MallocAllocator, NULL, input.data);

    return 0;
}
