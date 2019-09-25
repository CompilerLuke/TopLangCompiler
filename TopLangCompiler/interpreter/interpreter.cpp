//
//  interpreter.cpp
//  TopLangCompiler
//
//  Created by Lucas Goetz on 25/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//

#include "interpreter.h"
#include "helper.h"
#include "bytebuffer.h"
#include <stdio.h>

namespace top {
    namespace interpreter {
        void* push_n_stack(Interpreter& inter, unsigned int amount) {
            return write_buffer_n(inter.stack, amount);
        }
        
        template<typename T>
        void push_stack(Interpreter& inter, T t) {
            write_buffer(inter.stack, t);
        }
        
        void* pop_n_stack(Interpreter& inter, unsigned int amount) {
            inter.stack.length -= amount;
            return inter.stack.data + inter.stack.length;
        }
        
        template<typename T>
        T pop_stack(Interpreter& inter) {
            return *(T*)pop_n_stack(inter, sizeof(T));
        }
        
#define OP_Int32(opcode, op) case mir::opcode: { \
        int b = pop_stack<int>(inter); \
        int a = pop_stack<int>(inter); \
        push_stack(inter, a op b); \
        break; \
    }
        
        void exec(Interpreter& inter) {
            array<char>& bytecode = inter.func_ptr->bytecode;
            
            while(inter.inst < len(bytecode)) {
                mir::Opcode op = read_buffer<mir::Opcode>(bytecode, inter.inst);
                
                switch (op) {
                    OP_Int32(Add_Int32, +)
                    OP_Int32(Sub_Int32, -)
                    OP_Int32(Div_Int32, /)
                    OP_Int32(Mul_Int32, *)
                    case mir::Push_Int32: {
                        int a = read_buffer<int>(bytecode, inter.inst);
                        push_stack(inter, a);
                        break;
                    }
                }
            }
        }
        
        void interpret(mir::MIR& mir) {
            Interpreter inter = {};
            inter.mir = &mir;
            inter.func_ptr = &mir.funcs[0];
            
            exec(inter);
            
            printf("\n=== Interpreter ===\n\n");
            printf("Result = %i\n", pop_stack<int>(inter));

            destroy(inter);
        }
        
        void destroy(Interpreter& inter) {
            free_array(inter.stack);
        }
    }
}
