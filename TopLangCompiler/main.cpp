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
#include "validator.h"
#include "mir.h"
#include "interpreter.h"

using namespace top;

int main(int argc, const char * argv[]) {
    const char* filename = "main.top";
    
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
    err.src = input;
    err.filename = filename;
    
    lexer::init();
    
    lexer::Lexer lexer;
    slice<lexer::Token> tokens = lexer::lex(lexer, input, &err);
    
    if (error::is_error(&err)) {
        error::log_error(&err);
        lexer::destroy(lexer);
        lexer::destroy();
        dealloc(MallocAllocator, NULL, input.data);
        return 1;
    }

    lexer::dump_tokens(lexer);
    
    parser::Parser parser;
    parser::AST* ast = parser::parse(parser, tokens, &err);
    
    if (error::is_error(&err)) {
        error::log_error(&err);
        parser::destroy(parser);
        lexer::destroy(lexer);
        lexer::destroy();
        dealloc(MallocAllocator, NULL, input.data);
        return 1;
    }
    
    parser::dump_ast(parser);
    
    validator::init_types();
    validator::Validator validator;
    validator::validate(validator, ast, &err);
    
    printf("\n=== Validation ===\n\n");
    
    if (error::is_error(&err)) {
        error::log_error(&err);
        validator::destroy(validator);
        parser::destroy(parser);
        lexer::destroy(lexer);
        lexer::destroy();
        dealloc(MallocAllocator, NULL, input.data);
        return 1;
    }
    
    mir::MIR mir = mir::gen_mir(parser.root);
    mir::dump_mir(mir);
    
    interpreter::interpret(mir);
    
    mir::destroy(mir);
    validator::destroy(validator);
    parser::destroy(parser);
    lexer::destroy(lexer);
    lexer::destroy();
    dealloc(MallocAllocator, NULL, input.data);

    return 0;
}
