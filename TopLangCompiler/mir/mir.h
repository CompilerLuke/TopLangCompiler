//
//  mir.h
//  TopLangCompiler
//
//  Created by Lucas Goetz on 25/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//

#pragma once

#include "array.h"
#include "linear_allocator.h"

namespace top {
	namespace parser {
		struct AST;
	}

	namespace validator {
		struct VarDesc;
	}

	namespace mir {
		enum Opcode : char {
			Add_Int32, Sub_Int32, Mul_Int32, Div_Int32,
			Push_Int32,
			Ret_Int32
		};

		struct Func {
			int entry_point;
			int length;
		};

		struct Var {
			validator::VarDesc* desc;
			int reg;
		};

		struct MIR {
			struct LinearAllocator linear_allocator;
			array<char> bytecode;
			
			array<Func> funcs;
		};

		struct Converter {
			MIR* mir;
			int used_registers;
			
			array<Var> vars; //todo implement scopes
		};

		MIR gen_mir(parser::AST*);
		void destroy(MIR&);
		void dump_mir(MIR&);
	}
}
