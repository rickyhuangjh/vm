#include <iostream>
#include <memory>
#include <stdexcept>
#include <tuple>
#include <vector>
#include "command.h"
#include "controller.h"
#include "move_command.h"
#include "edit_command.h"
#include "model.h"
#include "posn.h"
#include "file.h"

#include "clip.h"

using std::get;
using std::vector;
using std::tuple;
using std::unique_ptr;

const char ESCAPE_CHAR = 27;

const char BACKSPACE_CHAR = 7;


const Posn FIRST_SPACE = Posn{0, 0};

Abstract_Edit_Command::Abstract_Edit_Command(Editor& editor):
    Abstract_Command{editor} {}

Abstract_Edit_Command::Abstract_Edit_Command(const int multiplier, Editor& editor):
    Abstract_Command{multiplier, editor} {}

// Insert commands

void insert_string(File& file, Posn posn, vector<tuple<Posn, int, char>>& changed, const string& s) {
    for (size_t i=0; i<s.length(); ++i) {
        file.insert_char(Posn{posn.row, posn.col + i}, s[i]);
        changed.push_back(tuple<Posn, int, char>(posn, 1, s[i]));
    }

}

void insert_char(File& file, Posn posn, vector<tuple<Posn, int, char>>& changed, char ch) {
    file.insert_char(posn, ch);
    changed.push_back(tuple<Posn, int, char>(posn, 1, ch));
}

void delete_char(File& file, Posn posn, vector<tuple<Posn, int, char>>& changed) {
    char deleted_char = file[posn.row][posn.col];
    file.delete_char(posn);
    changed.push_back(tuple<Posn, int, char>(posn, -1, deleted_char));
}




void Abstract_Edit_Command::engage_insert_mode() {

    editor.set_edit_mode(1);
    File& file = editor.get_file();
    Controller& controller = editor.get_controller();
    char ch;

    Posn cur_posn = editor.get_cursor_posn();
    while ((ch = controller.get_input()) != ESCAPE_CHAR) {

        if (ch == BACKSPACE_CHAR) {
            if (cur_posn == FIRST_SPACE) continue;
            if (cur_posn.col == 0) {
                --cur_posn.row;
                cur_posn.col = file[cur_posn.row].length() - 1;
                delete_char(file, cur_posn, changed);
                if (cur_posn.row < file.get_num_lines() - 1) {
                    string to_move = file[cur_posn.row + 1];
                    file[cur_posn.row] += std::move(to_move);
                    file.delete_line(cur_posn.row + 1);
                }
                
            } else {
                --cur_posn.col;
                delete_char(file, cur_posn, changed);
            }
        } else if (ch == '\t') {
            for (int i=0; i<4; ++i) {
                insert_char(file, cur_posn, changed, ' ');
                ++cur_posn.col;
            }
        } else {
            insert_char(file, cur_posn, changed, ch);
            ++cur_posn.col;

            if (ch == '\n') {
                string to_move = file[cur_posn.row].substr(cur_posn.col);
                file[cur_posn.row].erase(cur_posn.col);
                file.create_new_line(cur_posn.row + 1);
                ++cur_posn.row;
                cur_posn.col = 0;
                file[cur_posn.row] = std::move(to_move);
            }
        }

        editor.set_cursor_posn(file.bring_in_bounds(cur_posn));

        editor.refresh_display();

    }
    editor.set_edit_mode(0);
}

void Abstract_Edit_Command::engage_replace_mode() {
    editor.set_edit_mode(2);
    File& file = editor.get_file();
    Controller& controller = editor.get_controller();

    char ch;
    while ((ch = controller.get_input()) != ESCAPE_CHAR) {

        Posn cur_posn = editor.get_cursor_posn();
        if (cur_posn.col >= file[cur_posn.row].length() - 1) continue;

        if (ch == BACKSPACE_CHAR) continue;
        else if (ch == '\t') {
            for (int i=0; i<4; ++i) {
                insert_char(file, cur_posn, changed, ' ');
                ++cur_posn.col;
            }
        } else {
            if (ch != '\n') delete_char(file, cur_posn, changed);
            insert_char(file, cur_posn, changed, ch);
            ++cur_posn.col;

            if (ch == '\n') {
                string to_move = file[cur_posn.row].substr(cur_posn.col);
                file[cur_posn.row].erase(cur_posn.col);
                file.create_new_line(cur_posn.row + 1);
                ++cur_posn.row;
                cur_posn.col = 0;
                file[cur_posn.row] = std::move(to_move);
            }
        }

        editor.set_cursor_posn(cur_posn);

        editor.refresh_display();
    }
    editor.set_edit_mode(0);
}

bool Abstract_Edit_Command::undo() {
    File& file = editor.get_file();
    while (!changed.empty()) {
        auto [posn, action, ch] = changed.back();
        if (action == 1) {
            if (ch == '\n' && posn.row < file.get_num_lines() - 1) {
                string to_move = file[posn.row + 1];
                file[posn.row] += std::move(to_move);
                file.delete_line(posn.row + 1);
            }
            file.delete_char(posn);
        } else if (action == -1) {
            file.insert_char(posn, ch);
            if (ch == '\n') {
                string to_move = file[posn.row].substr(posn.col + 1);
                file[posn.row].erase(posn.col + 1);
                posn = Posn{posn.row + 1, 0};
                file.create_new_line(posn.row);
                file[posn.row] = std::move(to_move);
            }
        } else {
            if (ch == '\n') {
                string to_move = file[posn.row + 1];
                file[posn.row] += std::move(to_move);
                file.delete_line(posn.row + 1);
            }
            file.delete_char(posn);

            file.insert_char(posn, ch);
            if (ch == '\n' && posn.row < file.get_num_lines() - 1) {
                string to_move = file[posn.row].substr(posn.col + 1);
                file[posn.row].erase(posn.col + 1);
                posn = Posn{posn.row + 1, 0};
                file.create_new_line(posn.row);
                file[posn.row] = std::move(to_move);
            }
        }
        changed.pop_back();
    }

    editor.set_cursor_posn(editor.get_file().bring_in_bounds(editor.get_cursor_posn()));

    return true;
}

Before_Cursor_Insert_Edit_Command::Before_Cursor_Insert_Edit_Command(Editor& editor):
    Abstract_Edit_Command{editor} {}

void Before_Cursor_Insert_Edit_Command::execute() {
    engage_insert_mode();
}

Begin_Line_Insert_Edit_Command::Begin_Line_Insert_Edit_Command(Editor& editor):
    Abstract_Edit_Command{editor} {}

void Begin_Line_Insert_Edit_Command::execute() {
    Begin_Line_Move_Command(editor).execute();
    engage_insert_mode();
}

After_Cursor_Insert_Edit_Command::After_Cursor_Insert_Edit_Command(Editor& editor):
    Abstract_Edit_Command{editor} {}

void After_Cursor_Insert_Edit_Command::execute() {
    Posn cur_posn = editor.get_cursor_posn();
    ++cur_posn.col;
    if (editor.get_file().is_in_bounds(cur_posn)) editor.set_cursor_posn(cur_posn);
    engage_insert_mode();
}

End_Line_Insert_Edit_Command::End_Line_Insert_Edit_Command(Editor& editor):
    Abstract_Edit_Command{editor} {}

void End_Line_Insert_Edit_Command::execute() {
    End_Line_Move_Command(editor).execute();
    engage_insert_mode();
}

New_Line_Below_Insert_Edit_Command::New_Line_Below_Insert_Edit_Command(Editor& editor):
    Abstract_Edit_Command{editor} {}

void New_Line_Below_Insert_Edit_Command::execute() {
    Posn cur_posn = editor.get_cursor_posn();
    File& file = editor.get_file();
    file.create_new_line(cur_posn.row + 1);
    ++cur_posn.row;
    cur_posn.col = 0;
    changed.push_back(tuple<Posn, int, char>(cur_posn, 1, '\n'));
    editor.set_cursor_posn(cur_posn);
    engage_insert_mode();
}

New_Line_Above_Insert_Edit_Command::New_Line_Above_Insert_Edit_Command(Editor& editor):
    Abstract_Edit_Command{editor} {}

void New_Line_Above_Insert_Edit_Command::execute() {
    Posn cur_posn = editor.get_cursor_posn();
    File& file = editor.get_file();
    file.create_new_line(cur_posn.row);
    cur_posn.col = 0;
    changed.push_back(tuple<Posn, int, char>(cur_posn, 1, '\n'));
    editor.set_cursor_posn(cur_posn);
    engage_insert_mode();
}

Char_Under_Cursor_Delete_Edit_Command::Char_Under_Cursor_Delete_Edit_Command(const int multiplier, Editor& editor):
    Abstract_Edit_Command{multiplier, editor} {}

void Char_Under_Cursor_Delete_Edit_Command::execute() {
    Posn cur_posn = editor.get_cursor_posn();
    File& file = editor.get_file();
    if (cur_posn.col < file[cur_posn.row].length() - 1) delete_char(file, cur_posn, changed);
}

Char_Before_Cursor_Delete_Edit_Command::Char_Before_Cursor_Delete_Edit_Command(const int multiplier, Editor& editor):
    Abstract_Edit_Command{multiplier, editor} {}

void Char_Before_Cursor_Delete_Edit_Command::execute() {
    Posn cur_posn = editor.get_cursor_posn();
    File& file = editor.get_file();
    if (cur_posn.col > 0) {
        --cur_posn.col;
        delete_char(file, cur_posn, changed);
        editor.set_cursor_posn(cur_posn);
    }
}

Char_Under_Cursor_Replace_Edit_Command::Char_Under_Cursor_Replace_Edit_Command(const int multiplier, Editor& editor, char ch):
    Abstract_Edit_Command{multiplier, editor}, replacement{ch} {}

void Char_Under_Cursor_Replace_Edit_Command::execute() {
    Posn cur_posn = editor.get_cursor_posn();
    File& file = editor.get_file();
    Controller& controller = editor.get_controller();
    if (cur_posn.col < file[cur_posn.row].length() - 1) {
        delete_char(file, cur_posn, changed);
        insert_char(file, cur_posn, changed, replacement);
        if (multiplier > 1) Single_Move_Command(1, editor, 0, 1).execute();
    }
}

Replace_Mode_Edit_Command::Replace_Mode_Edit_Command(Editor& editor):
    Abstract_Edit_Command{editor} {}

void Replace_Mode_Edit_Command::execute() {
    engage_replace_mode();
}

Direction_Motion_Delete_Edit_Command::Direction_Motion_Delete_Edit_Command(const int multiplier,
    Editor& editor, unique_ptr<Abstract_Move_Command> move_command):
    Abstract_Edit_Command{multiplier, editor}, move_command{std::move(move_command)} {}

void Direction_Motion_Delete_Edit_Command::execute() {
    File& file = editor.get_file();
    Controller& controller = editor.get_controller();
    Posn start_posn = editor.get_cursor_posn();
    for (int i=0; i<move_command->get_multiplier(); ++i) move_command->execute();
    Posn end_posn = editor.get_cursor_posn();

    Posn begin_delete_posn = (start_posn <= end_posn)? start_posn: end_posn;
    Posn finish_delete_posn = (start_posn <= end_posn)? end_posn: start_posn;

    Posn cur_posn = finish_delete_posn;
    while (begin_delete_posn < cur_posn) {
        if (cur_posn == FIRST_SPACE) break;
        if (cur_posn.col == 0) {
            --cur_posn.row;
            cur_posn.col = file[cur_posn.row].length() - 1;
            delete_char(file, cur_posn, changed);
            string to_move = file[cur_posn.row + 1];
            file[cur_posn.row] += std::move(to_move);
            file.delete_line(cur_posn.row + 1);
        } else {
            --cur_posn.col;
            delete_char(file, cur_posn, changed);
        }
    }
    editor.set_cursor_posn(cur_posn);
    editor.update_page_cursor();
    editor.refresh_display();
}


Line_Delete_Edit_Command::Line_Delete_Edit_Command(const int multiplier, Editor& editor):
    Abstract_Edit_Command{multiplier, editor} {}

void Line_Delete_Edit_Command::execute() {
    File& file = editor.get_file();
    Posn cur_posn = editor.get_cursor_posn();
    Posn delete_posn = Posn{cur_posn.row, 0};
    const int chars_to_delete = file[delete_posn.row].length();
    for (int i=0; i<chars_to_delete; ++i) delete_char(file, delete_posn, changed);
    file.delete_line(delete_posn.row);
    editor.set_cursor_posn(file.bring_in_bounds(cur_posn));
    // First_Non_Blank_Char_Line_Move_Command(editor).execute();
    editor.update_page_cursor();
}

Direction_Motion_Replace_Edit_Command::Direction_Motion_Replace_Edit_Command(const int multiplier,
    Editor& editor, unique_ptr<Abstract_Move_Command> move_command):
    Abstract_Edit_Command{multiplier, editor},
    delete_command{Direction_Motion_Delete_Edit_Command(multiplier, editor, std::move(move_command))} {}

void Direction_Motion_Replace_Edit_Command::execute() {
    delete_command.execute();
    engage_insert_mode();
}

bool Direction_Motion_Replace_Edit_Command::undo() {
    Abstract_Edit_Command::undo();
    delete_command.undo();
    return true;
}

Line_Replace_Edit_Command::Line_Replace_Edit_Command(const int multiplier, Editor& editor):
    Abstract_Edit_Command{multiplier, editor}, delete_command{multiplier, editor} {}

void Line_Replace_Edit_Command::execute() {
    delete_command.execute();
    engage_insert_mode();
}

bool Line_Replace_Edit_Command::undo() {
    Abstract_Edit_Command::undo();
    delete_command.undo();
    return true;
}



Paste_After_Cursor_Insert_Edit_Command::Paste_After_Cursor_Insert_Edit_Command(const int multiplier, Editor& editor):
    Abstract_Edit_Command{multiplier, editor}, clip{editor.get_clip()} {}

void Paste_After_Cursor_Insert_Edit_Command::execute() {
    File& file = editor.get_file();
    Posn cur_posn = editor.get_cursor_posn();
    Posn paste_posn;
    if (clip.get_is_line()) {
        paste_posn = Posn(cur_posn.row + 1, 0);
        file.create_new_line(paste_posn.row);
        changed.push_back(tuple<Posn, int, char>(paste_posn, 1, '\n'));
    } else paste_posn = Posn(cur_posn.row, cur_posn.col + 1);
    insert_string(file, paste_posn, changed, clip.get_clip());
}

Paste_Before_Cursor_Insert_Edit_Command::Paste_Before_Cursor_Insert_Edit_Command(const int multiplier, Editor& editor):
    Abstract_Edit_Command{multiplier, editor}, clip{editor.get_clip()} {}

void Paste_Before_Cursor_Insert_Edit_Command::execute() {
    File& file = editor.get_file();
    Posn cur_posn = editor.get_cursor_posn();
    Posn paste_posn;
    if (clip.get_is_line()) {
        paste_posn = Posn(cur_posn.row, 0);
        file.create_new_line(paste_posn.row);
        changed.push_back(tuple<Posn, int, char>(paste_posn, 1, '\n'));
    } else paste_posn = Posn(cur_posn.row, cur_posn.col);
    insert_string(file, paste_posn, changed, clip.get_clip());
}


