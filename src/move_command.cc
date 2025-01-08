#include "command.h"
#include "cursor.h"
#include "move_command.h"
#include "posn.h"

const char ESCAPE_CHAR = 27;

const char BACKSPACE_CHAR = 7;

bool is_lnu(const char c) {
    return isalpha(c) || c == '_';
}

bool is_whitespace(const char c) {
    return c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\f' || c == '\v';
}

int first_non_blank_idx(const string& s) {
    size_t col_idx = 0;
    while (col_idx < s.length() && is_whitespace(s[col_idx])) ++col_idx;
    if (col_idx == s.length()) return 0;
    return col_idx;
}

// Abstract_Move_Command
Abstract_Move_Command::Abstract_Move_Command(Editor& editor):
    Abstract_Command{editor} {}
Abstract_Move_Command::Abstract_Move_Command(const int multiplier, Editor& editor):
    Abstract_Command{multiplier, editor} {}

bool Abstract_Move_Command::undo() { return false; }

Single_Move_Command::Single_Move_Command(const int multiplier, Editor& editor, const int downward, const int rightward):
    Abstract_Move_Command{multiplier, editor}, downward{downward}, rightward{rightward} {}

void Single_Move_Command::execute() {

    start_posn = editor.get_cursor_posn();
    Posn offset{downward, rightward};
    end_posn = start_posn + offset;
    editor.set_cursor_posn(end_posn);
    editor.update_page_cursor();
}



Begin_Next_Word_Move_Command::Begin_Next_Word_Move_Command(const int multiplier, Editor& editor):
    Abstract_Move_Command{multiplier, editor} {}

void Begin_Next_Word_Move_Command::execute() {
    start_posn = editor.get_cursor_posn();
    File& file = editor.get_file();
    char start_char = file[start_posn.row][start_posn.col];
    const bool IS_START_LNU = is_lnu(start_char);
    int col_idx;
    bool passed_whitespace = false;
    for (int line=start_posn.row; line<file.get_num_lines(); ++line) {
        col_idx = 0;
        if (line == start_posn.row) col_idx = start_posn.col;
        while (col_idx < file[line].length()) {
            char c = file[line][col_idx];
            if (is_whitespace(c)) {
                passed_whitespace = true;
                ++col_idx;
                continue;
            }
            if (IS_START_LNU != is_lnu(c) || passed_whitespace) {
                end_posn = Posn{line, col_idx};
                editor.set_cursor_posn(end_posn);
                editor.update_page_cursor();
                return;
            }
            ++col_idx;
        }
    }
}

Begin_Prev_Word_Move_Command::Begin_Prev_Word_Move_Command(const int multiplier, Editor& editor):
    Abstract_Move_Command{multiplier, editor} {}

void Begin_Prev_Word_Move_Command::execute() {
    start_posn = editor.get_cursor_posn();
    File& file = editor.get_file();
    char start_char = file[start_posn.row][start_posn.col];
    const bool IS_START_LNU = is_lnu(start_char);

    int col_idx;
    bool passed_whitespace = false;

     
    for (int line=start_posn.row; line >= 0; --line) {
        col_idx = file[line].length() - 1;
        if (line == start_posn.row) col_idx = start_posn.col;
        while (col_idx >= 0) {
            char c = file[line][col_idx];
            if (is_whitespace(c)) {
                passed_whitespace = true;
                --col_idx;
                continue;
            }
            if ((IS_START_LNU != is_lnu(c) || passed_whitespace) &&
                (col_idx == 0 || is_whitespace(file[line][col_idx-1]) ||
                (is_lnu(c) != is_lnu(file[line][col_idx-1])))) {
                end_posn = Posn{line, col_idx};
                editor.set_cursor_posn(end_posn);
                editor.update_page_cursor();
                return;
            }
            --col_idx;
        }
    }
}

End_Cur_Word_Move_Command::End_Cur_Word_Move_Command(const int multiplier, Editor& editor):
    Abstract_Move_Command{multiplier, editor} {}

void End_Cur_Word_Move_Command::execute() {
    start_posn = editor.get_cursor_posn();
    File& file = editor.get_file();

    size_t col_idx = 0;
    int line = 0;
    bool found = false;
    for (line=start_posn.row; line<file.get_num_lines(); ++line) {
        col_idx = 0;
        if (line == start_posn.row) col_idx = start_posn.col + 1;
        while (col_idx < file[line].length()) {
            char c = file[line][col_idx];
            if (col_idx >= file[line].length() - 2) found = true;
            if (!is_whitespace(c) && is_whitespace(file[line][col_idx+1])) found = true;
            if (!is_whitespace(c) && is_lnu(c) != is_lnu(file[line][col_idx+1])) found = true;
            if (found) break;
            
            ++col_idx;
        }
        if (found) break;
    }
    end_posn = Posn{line, col_idx};
    editor.set_cursor_posn(end_posn);
    editor.update_page_cursor();
}

Begin_Line_Move_Command::Begin_Line_Move_Command(Editor& editor):
    Abstract_Move_Command{editor} {}

void Begin_Line_Move_Command::execute() {
    start_posn = editor.get_cursor_posn();
    end_posn = Posn(start_posn.row, 0);
    editor.set_cursor_posn(end_posn);
    editor.update_page_cursor();
}

End_Line_Move_Command::End_Line_Move_Command(Editor& editor):
    Abstract_Move_Command{editor} {}

void End_Line_Move_Command::execute() {
    start_posn = editor.get_cursor_posn();
    File& file = editor.get_file();
    end_posn = Posn(start_posn.row, file[start_posn.row].length() - 1);
    editor.set_cursor_posn(end_posn);
    editor.update_page_cursor();
}

First_Non_Blank_Char_Line_Move_Command::First_Non_Blank_Char_Line_Move_Command(Editor& editor):
    Abstract_Move_Command{editor} {}

void First_Non_Blank_Char_Line_Move_Command::execute() {
    start_posn = editor.get_cursor_posn();
    File& file = editor.get_file();
    end_posn = Posn{start_posn.row, first_non_blank_idx(file[start_posn.row])};

    editor.set_cursor_posn(end_posn);
    editor.update_page_cursor();
}

Begin_File_Move_Command::Begin_File_Move_Command(Editor& editor):
    Abstract_Move_Command{editor} {}

void Begin_File_Move_Command::execute() {
    start_posn = editor.get_cursor_posn();
    File& file = editor.get_file();
    end_posn = Posn{0, first_non_blank_idx(file[0])};

    editor.set_cursor_posn(end_posn);
    editor.update_page_cursor();
}

End_File_Move_Command::End_File_Move_Command(Editor& editor):
    Abstract_Move_Command{editor} {}

void End_File_Move_Command::execute() {
    start_posn = editor.get_cursor_posn();
    File& file = editor.get_file();
    int last_line_idx = file.get_num_lines() - 1;
    
    end_posn = Posn(last_line_idx, file[last_line_idx].length() - 1);

    editor.set_cursor_posn(end_posn);
    editor.update_page_cursor();
}

Line_Num_Move_Command::Line_Num_Move_Command(const int line_num, Editor& editor):
    Abstract_Move_Command{editor}, line_num{line_num} {}

void Line_Num_Move_Command::execute() {
    start_posn = editor.get_cursor_posn();
    File& file = editor.get_file();
    if (line_num <= 0 || line_num >= file.get_num_lines()) return;
    end_posn = Posn(line_num - 1, first_non_blank_idx(file[line_num - 1]));

    editor.set_cursor_posn(end_posn);
    editor.update_page_cursor();
}

Next_Char_Line_Move_Command::Next_Char_Line_Move_Command(const int multiplier, Editor& editor):
    Abstract_Move_Command{multiplier, editor}, valid{false} {
    ch = editor.get_controller().get_input();
    if (ch != ESCAPE_CHAR && ch != BACKSPACE_CHAR) valid = true;
}

void Next_Char_Line_Move_Command::execute() {
    if (!valid) return;
    start_posn = editor.get_cursor_posn();
    File& file = editor.get_file();
    for (int col_idx=start_posn.col; col_idx<file[start_posn.row].length()-1; ++col_idx) {
        if (file[start_posn.row][col_idx] == ch) {
            editor.set_cursor_posn(Posn(start_posn.row, col_idx));
            return;
        }
    }
}

Prev_Char_Line_Move_Command::Prev_Char_Line_Move_Command(const int multiplier, Editor& editor):
    Abstract_Move_Command{multiplier, editor}, valid{false} {
    ch = editor.get_controller().get_input();
    if (ch != ESCAPE_CHAR && ch != BACKSPACE_CHAR) valid = true;
}

void Prev_Char_Line_Move_Command::execute() {
    if (!valid) return;
    start_posn = editor.get_cursor_posn();
    File& file = editor.get_file();
    for (int col_idx=start_posn.col; col_idx>=0; --col_idx) {
        if (file[start_posn.row][col_idx] == ch) {
            editor.set_cursor_posn(Posn(start_posn.row, col_idx));
            return;
        }
    }
}

Before_Next_Char_Line_Move_Command::Before_Next_Char_Line_Move_Command(const int multiplier, Editor& editor):
    Abstract_Move_Command{multiplier, editor}, valid{false} {
    ch = editor.get_controller().get_input();
    if (ch != ESCAPE_CHAR && ch != BACKSPACE_CHAR) valid = true;
}

void Before_Next_Char_Line_Move_Command::execute() {
    if (!valid) return;
    start_posn = editor.get_cursor_posn();
    File& file = editor.get_file();
    if (file[start_posn.row][start_posn.col] == ch) return;
    for (int col_idx=start_posn.col; col_idx<file[start_posn.row].length()-1; ++col_idx) {
        if (file[start_posn.row][col_idx] == ch && col_idx > 0) {
            editor.set_cursor_posn(Posn(start_posn.row, col_idx - 1));
            return;
        }
    }
}

Before_Prev_Char_Line_Move_Command::Before_Prev_Char_Line_Move_Command(const int multiplier, Editor& editor):
    Abstract_Move_Command{multiplier, editor}, valid{false} {
    ch = editor.get_controller().get_input();
    if (ch != ESCAPE_CHAR && ch != BACKSPACE_CHAR) valid = true;
}

void Before_Prev_Char_Line_Move_Command::execute() {
    if (!valid) return;
    start_posn = editor.get_cursor_posn();
    File& file = editor.get_file();
    if (file[start_posn.row][start_posn.col] == ch) return;
    for (int col_idx=start_posn.col; col_idx>=0; --col_idx) {
        if (file[start_posn.row][col_idx] != ch) {
            continue;
        }

        if (col_idx > 0) editor.set_cursor_posn(Posn(start_posn.row, col_idx - 1));
        else editor.set_cursor_posn(Posn(start_posn.row, col_idx));
        return;

    }
}

