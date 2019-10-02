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

#define OP_Int32(opcode, op) case mir::opcode: { \
        int target = read_buffer<int>(bytecode, inter.inst);  \
		int a = inter.registers[read_buffer<int>(bytecode, inter.inst)]; \
		int b = inter.registers[read_buffer<int>(bytecode, inter.inst)]; \
		inter.registers[target] = a op b; \
        break; \
    }
        
        void exec(Interpreter& inter) {
			array<char>& bytecode = inter.mir->bytecode;
            
            while(inter.inst < len(bytecode)) {
                mir::Opcode op = read_buffer<mir::Opcode>(bytecode, inter.inst);
                
                switch (op) {
                    OP_Int32(Add_Int32, +)
                    OP_Int32(Sub_Int32, -)
                    OP_Int32(Div_Int32, /)
                    OP_Int32(Mul_Int32, *)
                    case mir::Push_Int32: {
						int target = read_buffer<int>(bytecode, inter.inst);
                        int a = read_buffer<int>(bytecode, inter.inst);
						inter.registers[target] = a;
                        break;
                    }
					case mir::Ret_Int32: {
						int res = read_buffer<int>(bytecode, inter.inst);
						printf("Result = %i\n", inter.registers[res]);
					}
                }
            }
        }
        
        void interpret(mir::MIR& mir) {
            Interpreter inter = {};
            inter.mir = &mir;
           
			printf("\n=== Interpreter ===\n\n");
            exec(inter);
           

            destroy(inter);
        }
        
        void destroy(Interpreter& inter) {
        }
    }
}
