//
//  main.cpp
//  TopLangCompiler
//
//  Created by Lucas Goetz on 20/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//

#include <iostream>
#include "lexing/lexer.h"
#include <string.h>

#include "core/array.h"
#include "parsing/parser.h"

#include "error/error.h"
#include "core/file.h"
#include "validation/validator.h"
#include "mir/mir.h"
#include "interpreter/interpreter.h"

using namespace top;

int main(int argc, char *argv[], char *envp[]) 
{
    const char* filename = "main.top";
    
    FILE* file = io::open("main.top", io::FileMode::Read);
    if (!file) {
        printf("%sCould not open file '%s'%s\n", error::red, filename, error::black);
        return 1;
    }
    
    string input = io::read_file(file);
    if (input.length == 0) {
        printf("%sCould not read file '%s'%s\n", error::red, filename, error::black);
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
