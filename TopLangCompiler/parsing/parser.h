//
//  parser.hpp
//  TopLangCompiler
//
//  Created by Lucas Goetz on 20/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//

#pragma once

#include "lexer.h"
#include "array.h"
#include "memory_pool.h"
#include "linear_allocator.h"

namespace top {
    enum class OperatorType { Add, Sub, Mul, Div, Assign, In, ColonAssign, AddAssign, SubAssign, MulAssign, DivAssign };
    
    namespace parser {
        struct AST {
            enum ASTType { Operator, Literal, Identifier, Block, Tuple, If, Else, While, For, FuncCall, IfType, VarDecl, Func, IntType, Mut} type;
            
            struct OperatorData {
                OperatorType type;
                
                AST* left = NULL;
                AST* right = NULL;
            };
            
            struct LiteralData {
                enum {Int, Bool} type;
                
                union {
                    long long integer;
                    bool boolean;
                };
            };
            
            struct VarDeclData {
                string name;
                AST* type;
            };
            
            struct FuncData {
                string name;
                array<AST*> args;
                AST* return_type;
                AST* body;
            };
            
            struct IfData {
                AST* condition;
                AST* body;
                AST* or_else;
            };
            
            struct LoopData {
                AST* condition;
                AST* body;
            };
            
            struct ElseData {
                AST* body;
            };
            
            struct FuncCallData {
                AST* func;
                array<AST*> args;
            };
            
            union {
                OperatorData op;
                LiteralData literal;
                string identifier;
                IfData if_expr;
                ElseData else_expr;
                array<AST*> children;
                LoopData loop;
                FuncCallData func_call;
                VarDeclData var_decl;
                FuncData func;
                AST* mut;
            };
            
            struct Type* ast_type;
        };

        struct Parser {
            lexer::Lexer* lexer = NULL;
            AST* root = NULL;
            MemoryPool<1000, AST> pool = {};
            struct LinearAllocator linear_allocator = {};
            
            unsigned int i = 0;
            
            lexer::Token* token = NULL;
        };

        void parse(Parser&, lexer::Lexer*);
        void destroy(Parser&);
        
        void dump_ast(Parser&);

        AST* make_node(Parser&, AST::ASTType);
        void destroy_node(Parser&, AST*);
    }
};
