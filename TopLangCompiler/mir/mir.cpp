//
//  mir.cpp
//  TopLangCompiler
//
//  Created by Lucas Goetz on 25/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//

#include "mir.h"
#include "parser.h"
#include "helper.h"
#include "stdio.h"
#include "bytebuffer.h"

namespace top {
    namespace mir {
        void* read_bytecode_n(Func& func, unsigned int& i, unsigned int amount) {
            return read_buffer_n(func.bytecode, i, amount);
        }
        
        template<typename T>
        T read_bytecode(Func& func, unsigned int& i) {
            return read_buffer<T>(func.bytecode, i);
        }
        
        void* write_bytecode_n(Func& func, unsigned int amount) {
            array<char>& bytecode = func.bytecode;
            bytecode.length += amount;
            if (len(bytecode) > bytecode.capacity) {
                reserve(bytecode, len(bytecode) == 0 ? 2 : max(len(bytecode), bytecode.capacity * 2));
            }
            
            return bytecode.data + bytecode.length - amount;
        }
        
        template<typename T>
        void write_bytecode(Func& func, T value) {
            write_buffer(func.bytecode, value);
        }
        
        void convert(Converter& converter, parser::AST* ast) {
            MIR& mir = *converter.mir;
            Func& func = mir.funcs[0];
            
            if (ast->type == parser::AST::Block) {
                for (int i = 0; i < len(ast->children); i++) {
                    convert(converter, ast->children[0]);
                }
            }
            else if (ast->type == parser::AST::Operator) {
                convert(converter, ast->op.left);
                convert(converter, ast->op.right);
                
                Opcode op = {};
                if (ast->op.type == OperatorType::Add) op = Add_Int32;
                if (ast->op.type == OperatorType::Sub) op = Sub_Int32;
                if (ast->op.type == OperatorType::Mul) op = Mul_Int32;
                if (ast->op.type == OperatorType::Div) op = Div_Int32;
                write_bytecode(func, op);
            }
            else if (ast->type == parser::AST::Literal) {
                write_bytecode(func, Push_Int32);
                write_bytecode(func, (int)ast->literal.integer);
            }
            else {
                printf("AST not supported!");
            }
        }
        
        void push_func(MIR& mir) {
            Func func = {};
            func.bytecode.allocator = LinearAllocator;
            func.bytecode.allocator_data = &mir.linear_allocator;
            
            array_add(mir.funcs, func);
        }
        
        MIR gen_mir(parser::AST* ast) {
            MIR mir;
            mir.funcs.allocator = LinearAllocator;
            mir.funcs.allocator_data = &mir.linear_allocator;
            
            Converter converter;
            converter.mir = &mir;
            
            push_func(mir);
            
            convert(converter, ast);
            
            return mir;
        }
        
        void dump_mir(MIR& mir) {
            printf("\n==== MIR ====\n\n");
            
            for (int i = 0; i < len(mir.funcs); i++) {
                Func& func = mir.funcs[i];
                printf("Func(%i):\n\n", i);
                
                for (unsigned int i = 0; i < len(func.bytecode);) {
                    Opcode op = read_bytecode<Opcode>(func, i);
                    
                    switch(op) {
                        case Push_Int32: printf("PushInt32(%i)\n", read_bytecode<int>(func, i)); break;
                        case Add_Int32: printf("AddInt32\n"); break;
                        case Sub_Int32: printf("SubInt32\n"); break;
                        case Div_Int32: printf("DivInt32\n"); break;
                        case Mul_Int32: printf("MulInt32\n"); break;
                    }
                }
            }
        }
        
        void destroy(MIR& mir) {
            destroy(mir.linear_allocator);
        }
    }
}
