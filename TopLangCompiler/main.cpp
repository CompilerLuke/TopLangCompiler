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
    string filename = "main.top";
    
    FILE* file = io::open("main.top", io::FileMode::Read);
    if (!file) {
        printf("\033[1;31mCould not open file '%s'\033[0m\n", filename);
        return 1;
    }
    
    string input = io::read_file(file);
    if (input.length == 0) {
        printf("\033[;31mCould not read file '%s'\033[0m\n", filename);
    }
    io::destroy(file);
    
    printf("========= Compiling =========\n\n");
    
    error::Error err;
    
    lexer::init();
    
    lexer::Lexer lexer;
    lexer::lex(lexer, input, filename,  &err);
    
    if (error::is_error(&err)) {
        error::log_error(&err);
        lexer::destroy(lexer);
        lexer::destroy();
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
        lexer::destroy();
        return 1;
    }
    
    parser::dump_ast(parser);
    
    printf("Occupied bytes: %lu\n", parser.linear_allocator.occupied);
    
    parser::destroy(parser);
    lexer::destroy(lexer);
    lexer::destroy();
    dealloc(MallocAllocator, NULL, input.data);

    return 0;
}
