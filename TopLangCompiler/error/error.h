//
//  error.hpp
//  TopLangCompiler
//
//  Created by Lucas Goetz on 20/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//

#pragma once

#include "string.h"

namespace top {
    namespace error {
        enum ErrorID { UnknownToken, SyntaxError, IndentationError };
        
        struct Error {
            string group;
            string mesg;
            
            string filename;
            string src;
            
            unsigned int line = 0;
            unsigned int column = 0;
            unsigned int token_length = 0;
            
            ErrorID id;
        };
        
        inline bool is_error(Error* error) { return error->mesg.length > 0; }
        
        void log_error(Error* error);
    }
}
