//
//  editor.cpp
//  CodeEditor
//
//  Created by Lucas Goetz on 26/09/2019.
//  Copyright © 2019 Lucas Goetz. All rights reserved.
//

#include "editor.h"
#include "stdio.h"
#include "lexer.h"
#include "file.h"
#include <math.h>
#include <curses.h>
#include "helper.h"

#define ctrl(x)           ((x) & 0x1f)

namespace top {
    namespace editor {
        enum colors { Black, Red, Green, Yellow, Blue, Magenta, Cyan, White };
        
        constexpr int column_offset = 6;
        
        struct Editor {
            bool close;
            
            int last_gap_size = 10;
            int gap_length = 10;
            int gap_pos;
            int line;
            int column;
            int i;
            int line_offset;
            int target_column;
            
            array<char> source;
            array<char> save_source;
            
            error::Error err;
            lexer::Lexer lexer;
            
            string filename;
        };
        
        void destroy(Editor& editor) {
            free_array(editor.source);
            lexer::destroy(editor.lexer);
            lexer::destroy();
        }
        
        bool in_gap(Editor& editor, int i) {
            return editor.gap_pos <= i && i < editor.gap_pos + editor.gap_length;
        }
        
        void display_line(int line, int max_lines) {
            if (line != max_lines) {
                printw("%4i| ", line + 1);
            }
        }
        
        void position_cursor(Editor& editor) {
            move(editor.line - editor.line_offset, editor.column + column_offset);
        }
        
        void save(Editor& editor) {
            reserve(editor.save_source, editor.source.length - editor.gap_length);
            editor.save_source.length = editor.source.length - editor.gap_length;
            char* dst = editor.save_source.data;
            
            memcpy(dst, editor.source.data, editor.gap_pos);
            memcpy(dst + editor.gap_pos, editor.source.data + editor.gap_pos + editor.gap_length, editor.source.length - editor.gap_pos - editor.gap_length);
            
            FILE* file = io::open(editor.filename, io::FileMode::Write);
            if (file) {
                io::write_file(file, { editor.save_source.data, editor.save_source.length });
                io::destroy(file);
            }
        }
        
        void move_right(Editor& editor) {
            if (editor.i + 1 + editor.gap_length > len(editor.source)) return;
            
            char c = editor.source[editor.gap_pos + editor.gap_length];
            editor.source[editor.gap_pos] = c;
            editor.gap_pos++;
            
            if (c == '\n') {
                editor.column = 0;
                editor.line++;
            } else {
                editor.column++;
            }
            editor.i++;
        }
        
        int find_newline_back(Editor& editor, int start_offset, int offset, char c = '\n') {
            for (int i = editor.i + start_offset; i >= 0 && i < len(editor.source); i--) {
                if (!in_gap(editor, i) && editor.source[i] == c) { return i + offset; }
            }
            
            return 0;
        }
        
        int find_newline_front(Editor& editor, int i, int offset, char c = '\n') {
            for (int i = editor.i; i < len(editor.source); i++) {
                if (!in_gap(editor, i) && editor.source[i] == c) { return i + offset; }
            }
            
            return len(editor.source);
        }
        
        void move_left(Editor& editor) {
            if (editor.i <= 0) return;
            char c = editor.source[editor.gap_pos - 1];
            editor.source[editor.gap_pos + editor.gap_length - 1] = c;
            editor.gap_pos--;

            if (c == '\n') {
                editor.line--;
                editor.column = editor.i - find_newline_back(editor, -2, 1);
            }
        
            if (editor.column > 0) {
                editor.column--;
                editor.i--;
            }
        }
        
        void move_left(Editor& editor, int num) {
            for (int i = 0; i < num; i++) move_left(editor);
        }
        
        void move_right(Editor& editor, int num) {
            for (int i = 0; i < num; i++) move_right(editor);
        }
        
        int dist_newline_back(Editor& editor, int i) {
            int move_left_by = 0;
            for (int i = editor.i; i >= 0; i--) {
                if (!in_gap(editor, i) && editor.source[i] == '\n') break;
                if (!in_gap(editor, i)) move_left_by++;
            }
            return move_left_by;
        }
        
        int dist_newline_front(Editor& editor, int i) {
            int move_right_by = 0;
            for (; i < editor.source.length; i++) {
                if (!in_gap(editor, i) && editor.source[i] == '\n') break;
                if (!in_gap(editor, i)) move_right_by++;
            }
            return move_right_by;
        }
        
        void move_up(Editor& editor) {
            if (editor.line == 0) return;
                
            int move_left_by = dist_newline_back(editor, editor.i);
            int prev_column = editor.target_column;
            move_left(editor, move_left_by + 1);
            
            int column = dist_newline_back(editor, editor.i);
            move_left(editor, column - min(prev_column, column));
        }

        void move_down(Editor& editor) {
            int move_right_by = dist_newline_front(editor, editor.i);
            
            int prev_column = editor.target_column;
            move_right(editor, move_right_by + 1);
            
            int column = dist_newline_front(editor, editor.i);
            move_right(editor, min(prev_column, column));
        }
        
        void reserve_gap(Editor& editor) {
            editor.gap_length = 2 * editor.last_gap_size;
            
            int new_length = editor.source.length + editor.gap_length;
            
            char* ptr = (char*) malloc(new_length);
            
            memcpy(ptr, editor.source.data, editor.gap_pos);
            memcpy(ptr + editor.gap_pos + editor.gap_length, editor.source.data + editor.gap_pos, editor.source.length - editor.gap_pos);
            free(editor.source.data);
            
            editor.source.data = ptr;
            editor.source.length = new_length;
            editor.source.capacity = new_length;
            
            editor.last_gap_size = editor.gap_length;
        }

        void render_tokens(Editor& editor, char* buffer, slice<lexer::Token> tokens, int offset) {
            if (len(editor.err.mesg) > 0) {
                move(editor.line - editor.line_offset, offset);
                
                for (int i = 0; i < strlen(buffer); i++) {
                    bool is_err = i >= editor.err.column && i < editor.err.column + editor.err.token_length;
                    
                    if (is_err) attron(A_UNDERLINE);
                    addch(buffer[i]);
                    if (is_err) attroff(A_UNDERLINE);
                }
            }
            
            for (int i = 0; i < len(tokens); i++) {
                lexer::Token& token = tokens[i];

                int attr = COLOR_PAIR(0);
                if (token.group == lexer::Keyword || token.type == lexer::True || token.type == lexer::False) attr = COLOR_PAIR(1);
                if (token.type == lexer::Int) attr = COLOR_PAIR(2);
                if (token.type == lexer::EndOfFile) continue;
                
                attron(attr);

                
                for (int c = 0; c < token.length_of_token; c++) {
                    move(editor.line - editor.line_offset, token.column + c + offset);
                    addch(buffer[token.column + c]);
                }
                
                attroff(attr);
            }
        }
        
        void render_line(Editor& editor) {
            int newline_start = find_newline_back(editor, 0, 1);
            int newline_end = find_newline_front(editor, 1, 0);
            
            if (newline_end < newline_start) newline_end = newline_start;
            
            move(editor.line - editor.line_offset, 0);
            display_line(editor.line, editor.line + 1);
            
            string line;
            
            move(editor.line - editor.line_offset, column_offset);
            clrtoeol();
            
            if (editor.i >= len(editor.source)) return;
            
            char buffer[100];
            
            if (editor.gap_pos >= newline_start && editor.gap_pos < newline_end) {
                string line;
                line.data = editor.source.data + newline_start;
                line.length = editor.gap_pos - newline_start;
                to_cstr(line, buffer, 100);
                
                int len = line.length;
            
                line.data = editor.source.data + editor.gap_pos + editor.gap_length;
                line.length = newline_end - editor.gap_pos - editor.gap_length;
                to_cstr(line, buffer + len, 100 - len);
            } else {
            
                string line;
                line.data = editor.source.data + newline_start;
                line.length = newline_end - newline_start;
                to_cstr(line, buffer, 100);
            }
        
            array<lexer::Token> tokens = editor.lexer.tokens;
            tokens.length = 0;
            editor.lexer = {}; //todo reuse tokens array
            editor.lexer.tokens = tokens;
            editor.err.mesg = "";
            
            render_tokens(editor, buffer, lexer::lex(editor.lexer, buffer, &editor.err), column_offset);
        }
        
        int find_indent(Editor& editor) {
            int num_indent = 0;
            
            for (int i = editor.i - 1; i >= 0; i--) {
                if (!in_gap(editor, i) && editor.source[i] != ' ') break;
                num_indent++;
            }
            return num_indent;
        }
        
        int line_indent(Editor& editor, int start_offset) {
            int num_indent = 0;
            for (int i = find_newline_back(editor, start_offset, 1); i < len(editor.source); i++) {
                if (!in_gap(editor, i) && editor.source[i] != ' ') break;
                num_indent++;
            }
            
            return num_indent;
        }
        
        char get_ch(Editor& editor, int i) {
            i += editor.i;
            if (i < 0 || i >= len(editor.source)) return 0;
            return editor.source[i];
        }
        
        int align_indent(int num_indent) {
            if (num_indent % 4 != 0) {
                num_indent = 4 + (num_indent - (num_indent % 4));
            }
            return num_indent;
        }
        
        void remove(Editor& editor, int num);
        
        void remove_indent(Editor& editor, int num_indent) {
            if (num_indent <= 0) return;
            
            int rem = num_indent % 4;
            remove(editor, rem == 0 ? 4 : rem);
        }
        
        void insert(Editor& editor, char c) {
            if (c == '}' || c == ')') {
                int num_indent = find_indent(editor);
                remove_indent(editor, num_indent);
            }
            
            if (editor.gap_length == 0) { reserve_gap(editor); }
            
            editor.source[editor.gap_pos] = c;
            editor.gap_length--;
            editor.gap_pos++;
            editor.i++;
            
            if (c == '\n') {
                editor.line++;
                editor.column = 0;

                int num_indent = align_indent(line_indent(editor, -2));
   
                char last_ch = get_ch(editor, -2);
                if (last_ch == ':' || last_ch == '{' || last_ch == '(') num_indent += 4;
                
                for (int i = 0; i < num_indent; i++) {
                    insert(editor, ' ');
                }
            } else {
                editor.column++;
            }
        }
        
        void insert(Editor& editor, string s) {
            for (int i = 0; i < len(s); i++) {
                insert(editor, s[i]);
            }
        }
        
        void remove(Editor& editor) {
            if (editor.i == 0) return;
            
            editor.gap_length++;
            editor.gap_pos--;
            
            if (editor.source[editor.i - 1] == '\n') {
                editor.line--;
                editor.column = editor.i - find_newline_back(editor, -2, 1);
            }
            
            editor.i--;
            editor.column--;
        }
        
        void remove(Editor& editor, int num) {
            for (int i = 0; i < num; i++) {
                remove(editor);
            }
        }
        
        void render_lines_screen(Editor& editor) {
            int i = editor.i;
            int line = editor.line;
            int lines_displayed = 0;
    
            if (editor.line >= LINES) {
                editor.line_offset = editor.line - LINES + 1;
            }
            
            if (editor.line <= LINES) {
                editor.line_offset = 0;
            }
            
            int max_line = editor.line_offset + LINES;
            
            editor.line = 0;
            for (editor.i = 0;
                 editor.i < len(editor.source) && editor.line < max_line; //todo ignores last newline
                 editor.i = find_newline_front(editor, editor.i, 0))
            {
                editor.i++;
                if (editor.line >= editor.line_offset) { render_line(editor); lines_displayed++; }
                editor.line++;
            }
    
            move(lines_displayed, 0);
            for (int i = 0; i < LINES - lines_displayed; i++) {
                printw("    |\n");
            }

            editor.i = i;
            editor.line = line;
            
            position_cursor(editor);
            
            refresh();
        }
        
        int make_editor(Editor& editor, string filename) {
            FILE* file = io::open(filename, io::FileMode::Read);
            if (!file) {
                char buffer[100];
                to_cstr(filename, buffer, 100);
                printf("\033[1;31mCould not open file '%s'\033[0m\n", buffer);
                return 1;
            }
            
            editor = {};
            editor.filename = filename;
            
            string input = io::read_file(file);
            if (input.length == 0) {
                char buffer[100];
                to_cstr(filename, buffer, 100);
                printf("\033[;31mCould not read file '%s'\033[0m\n", buffer);
            }
            io::destroy(file);
            
            reserve(editor.source, input.length + editor.gap_length);
            memcpy(editor.source.data + editor.gap_length, input.data, input.length);
            editor.source.length = editor.source.capacity;
            
            editor.err.src = input;
            editor.err.filename = filename;
            
            return 0;
        }
        
        string command(Editor& editor, const char* prefix, char* buffer) {
            move(LINES - 1, 0);
            clrtoeol();
            printw(prefix);
            
            echo();
            wgetnstr(stdscr, buffer, 100);
            noecho();
            
            return { buffer, (unsigned int)strlen(buffer) };
        }

        
        void update_target_column(Editor& editor) {
            editor.target_column = editor.column;
        }
        
        int run_editor(string filename) {
            Editor editor = {};
            lexer::init();
            
            if (make_editor(editor, filename)) {
                return 1;
            }
            
            initscr();
            raw();
            keypad(stdscr, true);
            cbreak();
            start_color();
            
            use_default_colors();
            
            init_pair(1, COLOR_RED, -1);
            init_pair(2, COLOR_BLUE, -1);
            init_pair(3, -1, COLOR_RED);
    
            noecho();
            
            render_lines_screen(editor);
    
            for (;;) {
                int ch = getch();
                
                switch (ch) {
                    case EOF: goto END;
                    case KEY_UP: move_up(editor); break;
                    case KEY_DOWN: move_down(editor); break;
                    case KEY_RIGHT: move_right(editor); update_target_column(editor); break;
                    case KEY_LEFT: move_left(editor); update_target_column(editor); break;
                    case '\t': {
                        int num_indent = find_indent(editor);
                        for (int i = 0; i < (4 - num_indent % 4); i++) {
                            insert(editor, ' ');
                        }
                        update_target_column(editor);
                        break;
                    }
                    case ctrl('s'): save(editor); break;
                    case ctrl('c'): goto END;
                    case ctrl('o'): {
                        char buffer[100];
                        string cmd = command(editor, ": open ", buffer);
                        destroy(editor);
                        make_editor(editor, cmd);
                        
                        break;
                    }
                    case ctrl('w'): {
                        char buffer[100];
                        string cmd = command(editor, ": w", buffer);
                        
                        if (cmd == "q") {
                            save(editor);
                            goto END;
                        }
                        break;
                    }
                    
                    case ctrl('l'): {
                        int newline_end = find_newline_back(editor, 0, 0);
                        remove(editor, editor.i - newline_end);
                        update_target_column(editor);
                        break;
                    }
                    case 127: { //backspace
                        int num_indent = find_indent(editor);
                        
                        if (num_indent == 0) remove(editor, 1);
                        else remove_indent(editor, num_indent);
                        update_target_column(editor);
                        break;
                    }
                    default:
                        insert(editor, ch);
                        update_target_column(editor);
                        break;
                }
                
                render_lines_screen(editor);
            }
        
        END:
            endwin();
            destroy(editor);
            return 0;
        }
    };
}
