//
//  main.cpp
//  CodeEditor
//
//  Created by Lucas Goetz on 26/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//

#include "stdio.h"
#include "editor.h"

using namespace top;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("\033[;31mExpecting filename argument!\033[0m\n");
        return 1;
    }
    
    const char* filename = argv[1];
    return editor::run_editor(filename);
}
