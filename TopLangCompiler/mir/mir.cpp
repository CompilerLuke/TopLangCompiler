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
#include <stdio.h>
#include "bytebuffer.h"

namespace top {
	namespace mir {
		void* read_bytecode_n(MIR& mir, unsigned int& i, unsigned int amount) {
			return read_buffer_n(mir.bytecode, i, amount);
		}

		template<typename T>
		T read_bytecode(MIR& mir, unsigned int& i) {
			return read_buffer<T>(mir.bytecode, i);
		}

		void* write_bytecode_n(MIR& mir, unsigned int amount) {
			last(mir.funcs).length += amount;
			return write_buffer_n(mir.bytecode, amount);
		}

		template<typename T>
		void write_bytecode(MIR& mir, T value) {
			last(mir.funcs).length += sizeof(T);
			write_buffer(mir.bytecode, value);
		}

		int convert(Converter& converter, parser::AST* ast, int reg = -1);
		
		int push_register(Converter& converter, int* reg, int n = 1) {
			if (reg && *reg >= 0) {
				int tmp = *reg;
				*reg = -1;
				return tmp;
			}

			int tmp = converter.used_registers;
			converter.used_registers += n;
			return tmp;
		}

		void pop_register(Converter& converter, int n = 1) {
			converter.used_registers--;
		}

		int convert_colon_assign(Converter& converter, parser::AST* ast) {
			MIR& mir = *converter.mir;

			int reg = push_register(converter, NULL);
			array_add(converter.vars, {ast->op.left->identifier.desc, reg});

			convert(converter, ast->op.right, reg);

			return -1;
		}

		int convert(Converter& converter, parser::AST* ast, int reg) {
			MIR& mir = *converter.mir;

			switch (ast->type) {
				case parser::AST::Block: {
					int target = -1;
					for (int i = 0; i < len(ast->children); i++) {
						target = convert(converter, ast->children[i]);
					}

					return target;
				}
				case parser::AST::Operator: {
					Opcode op = {};

					switch (ast->op.type) {
						case OperatorType::Add: op = Add_Int32; break;
						case OperatorType::Sub: op = Sub_Int32; break;
						case OperatorType::Mul: op = Mul_Int32; break;
						case OperatorType::Div: op = Div_Int32; break;
						case OperatorType::ColonAssign: return convert_colon_assign(converter, ast);
					}

					int a = convert(converter, ast->op.left);
					int b = convert(converter, ast->op.right);

					pop_register(converter, 2);
					int target = push_register(converter, &reg);

					write_bytecode(mir, op);
					write_bytecode(mir, target);
					write_bytecode(mir, a);
					write_bytecode(mir, b);

					return target;
				}
				case parser::AST::Literal: {
					int reg = push_register(converter, &reg);
					write_bytecode(mir, Push_Int32);
					write_bytecode(mir, reg);
					write_bytecode(mir, (int)ast->literal.integer);

					return reg;
				}
				case parser::AST::Identifier: {
					int reg = -1; 

					for (int i = 0; i < len(converter.vars); i++) {
						if (converter.vars[i].desc == ast->identifier.desc) {
							reg = converter.vars[i].reg;
							break;
						}
					}

					assert(reg != -1);

					return reg;
				}

				default: {
					printf("AST not supported!");
					exit(1);
				}
			}
		}

		void push_func(MIR& mir) {
			Func func = {};
			func.entry_point = mir.bytecode.length;

			array_add(mir.funcs, func);
		}

		void destroy(Converter& converter) {
			free_array(converter.vars);
		}

		MIR gen_mir(parser::AST* ast) {
			MIR mir;
			mir.funcs.allocator = LinearAllocator;
			mir.funcs.allocator_data = &mir.linear_allocator;
			mir.bytecode.allocator = LinearAllocator;
			mir.bytecode.allocator_data = &mir.linear_allocator;

			Converter converter = {};
			converter.mir = &mir;

			push_func(mir);

			int res = convert(converter, ast);
			if (res >= 0) {
				write_bytecode(mir, Ret_Int32);
				write_bytecode(mir, res);
			}

			destroy(converter);

			return mir;
		}

		void dump_op( const char* name, MIR& mir, unsigned int& i) {
			int target = read_bytecode<int>(mir, i);
			int a = read_bytecode<int>(mir, i);
			int b = read_bytecode<int>(mir, i);
			printf("%s(%i, %i, %i)\n", name, target, a, b);
		}

		void dump_mir(MIR& mir) {
			printf("\n==== MIR ====\n\n");

			for (int i = 0; i < len(mir.funcs); i++) {
				Func& func = mir.funcs[i];
				printf("Func(%i):\n\n", i);


				for (unsigned int i = func.entry_point; i < func.entry_point + func.length;) {
					Opcode op = read_bytecode<Opcode>(mir, i);

					switch (op) {
						case Push_Int32: {
							int target = read_bytecode<int>(mir, i);
							int a = read_bytecode<int>(mir, i);
							printf("Push_Int32(%i, %i)\n", target, a); 
							break;
						}
						case Add_Int32: dump_op("Add_Int32", mir, i); break;
						case Sub_Int32: dump_op("Sub_Int32", mir, i); break;
						case Mul_Int32: dump_op("Mul_Int32", mir, i); break;
						case Div_Int32: dump_op("Div_Int32", mir, i); break;
						case Ret_Int32: {
							int res = read_bytecode<int>(mir, i);
							printf("Ret_Int32(%i)", res);
						}

					}
				}
			}
		}

		void destroy(MIR& mir) {
			destroy(mir.linear_allocator);
		}
	}
}
